use crate::ast::ast::*;
use crate::utils::error::{Error, Spanned};
use rpds::HashTrieMap;

#[derive(Debug, Clone)]
struct Context {
    func_table: HashTrieMap<String, FuncType>,
    var_table: HashTrieMap<String, (VarType, i64)>,
    func_scope_ty: Option<PrimitiveType>,
    loop_scope: bool,
    level: i64,
}

impl Context {
    fn new() -> Self {
        Self {
            func_table: HashTrieMap::new(),
            var_table: HashTrieMap::new(),
            func_scope_ty: None,
            loop_scope: false,
            level: 0,
        }
    }

    fn add_builtin_funcs(&self) -> Self {
        let mut new_ctx = self.clone();
        new_ctx.func_table = new_ctx.func_table
            .insert("putint".to_string(), FuncType {
                params: vec![VarType::Primitive(PrimitiveType::Int)],
                return_ty: PrimitiveType::Void,
            })
            .insert("putch".to_string(), FuncType {
                params: vec![VarType::Primitive(PrimitiveType::Int)],
                return_ty: PrimitiveType::Void,
            })
            .insert("putarray".to_string(), FuncType {
                params: vec![VarType::Primitive(PrimitiveType::Int), VarType::Array(PrimitiveType::Int, vec![0])],
                return_ty: PrimitiveType::Void,
            })
            .insert("getint".to_string(), FuncType {
                params: vec![],
                return_ty: PrimitiveType::Int,
            })
            .insert("getch".to_string(), FuncType {
                params: vec![],
                return_ty: PrimitiveType::Int,
            })
            .insert("getarray".to_string(), FuncType {
                params: vec![VarType::Primitive(PrimitiveType::Int), VarType::Array(PrimitiveType::Int, vec![0])],
                return_ty: PrimitiveType::Void,
            });
        new_ctx
    }

    fn new_scope(&self) -> Self {
        let mut new_ctx = self.clone();
        new_ctx.level += 1;
        new_ctx
    }

    fn add_func<'src>(&self, name: Spanned<&'src str>, ty: FuncType) -> Result<Self, Error> {
        if self.func_table.contains_key(name.0) {
            return Err(Error::custom(
                name.1.clone(),
                format!("function '{}' already declared", name.0),
            ));
        }
        let mut new_ctx = self.clone();
        new_ctx.func_table = new_ctx.func_table.insert(name.0.to_string(), ty);
        Ok(new_ctx)
    }

    fn add_var<'src>(&self, name: Spanned<&'src str>, ty: VarType) -> Result<Self, Error> {
        if let Some((_, level)) = self.var_table.get(name.0) {
            if *level == self.level {
                return Err(Error::custom(
                    name.1.clone(),
                    "variable already declared in this scope",
                ));
            }
        }
        let mut new_ctx = self.clone();
        new_ctx.var_table = new_ctx
            .var_table
            .insert(name.0.to_string(), (ty, self.level));
        Ok(new_ctx)
    }
}

fn visit_var_decl<'src>(
    ctx: &Context,
    var_decl: &Spanned<VarDecl<'src>>,
) -> Result<Context, Error> {
    match &var_decl.0.ty {
        PrimitiveType::Void => {
            return Err(Error::custom(var_decl.1.clone(), "variable declared void"))
        }
        _ => {}
    }

    let mut new_ctx = ctx.clone();

    for (def, span) in &var_decl.0.defs {
        let ty = match &def.dims {
            Some(dims) => {
                for dim in dims {
                    if *dim <= 0 {
                        return Err(Error::custom(
                            span.clone(),
                            format!("size '{}' of array '{}' should be positive", def.name.0, *dim),
                        ));
                    }
                }
                VarType::Array(PrimitiveType::Int, dims.clone())
            }
            None => VarType::Primitive(PrimitiveType::Int),
        };
        if let Some(init) = &def.init {
            let init_ty = visit_expr(&new_ctx, init)?;
            if ty != init_ty {
                return Err(Error::custom(
                    span.clone(),
                    format!(
                        "invalid type for initialization, expected {}, found {}",
                        ty, init_ty
                    ),
                ));
            }
        }

        new_ctx = new_ctx.add_var(def.name, ty)?;
    }
    Ok(new_ctx)
}

fn visit_func_decl<'src>(
    ctx: &Context,
    func_decl: &Spanned<FuncDecl<'src>>,
) -> Result<Context, Error> {
    let mut new_func_ctx = ctx.new_scope();
    let mut params_ty = Vec::new();
    for param in &func_decl.0.params {
        match &param.0.ty {
            PrimitiveType::Void => {
                return Err(Error::custom(
                    param.1.clone(),
                    "invalid use of type 'void' in parameter declaration",
                ));
            }
            _ => {}
        }
        let ty = match &param.0.dims {
            Some(dims) => {
                for dim in &dims[1..] {
                    if *dim <= 0 {
                        return Err(Error::custom(
                            param.1.clone(),
                            format!("size '{}' of array '{}' is negative", param.0.name.0, *dim),
                        ));
                    }
                }
                VarType::Array(param.0.ty.clone(), dims.clone())
            }
            None => VarType::Primitive(param.0.ty.clone()),
        };
        params_ty.push(ty.clone());
        new_func_ctx = new_func_ctx.add_var(param.0.name, ty)?;
    }

    let func_ty = FuncType {
        params: params_ty,
        return_ty: func_decl.0.return_ty.clone(),
    };

    new_func_ctx = new_func_ctx.add_func(func_decl.0.name, func_ty.clone())?;
    new_func_ctx.func_scope_ty = Some(func_decl.0.return_ty.clone());

    let has_return = visit_block(&new_func_ctx, &func_decl.0.body)?;

    if func_decl.0.return_ty != PrimitiveType::Void && !has_return {
        return Err(Error::custom(
            func_decl.1.clone(),
            "function does not return a value",
        ));
    }

    Ok(ctx.add_func(func_decl.0.name, func_ty)?)
}

fn visit_block<'src>(ctx: &Context, block: &Spanned<Block<'src>>) -> Result<bool, Error> {
    let mut new_ctx = ctx.clone();
    let mut has_return = false;
    for stmt in &block.0.stmts {
        match stmt {
            BlockItem::Stmt(stmt) => {
                has_return |= visit_stmt(&new_ctx, stmt)?;
            }
            BlockItem::Decl(var_decl) => {
                new_ctx = visit_var_decl(&new_ctx, var_decl)?;
            }
        }
    }
    Ok(has_return)
}

fn visit_stmt<'src>(ctx: &Context, stmt: &Spanned<Stmt<'src>>) -> Result<bool, Error> {
    let mut has_return = false;
    match &stmt.0 {
        Stmt::Expr(expr) => {
            visit_expr(&ctx, expr)?;
        }
        Stmt::Block(block) => {
            has_return |= visit_block(&ctx.new_scope(), block)?;
        }
        Stmt::If { cond, then, els } => {
            visit_expr(&ctx, cond)?;
            has_return |= visit_stmt(&ctx, then)?;
            if let Some(els) = els {
                has_return |= visit_stmt(&ctx, els)?;
            }
        }
        Stmt::While { cond, body } => {
            visit_expr(&ctx, cond)?;
            let mut new_ctx = ctx.clone();
            new_ctx.loop_scope = true;
            has_return |= visit_stmt(&new_ctx, body)?;
        }
        Stmt::Return(expr) => {
            has_return = true;
            match (&ctx.func_scope_ty, expr) {
                (Some(PrimitiveType::Void), Some(_)) => {
                    return Err(Error::custom(
                        stmt.1.clone(),
                        "void function returning a value",
                    ));
                }
                (Some(PrimitiveType::Int), None) => {
                    return Err(Error::custom(stmt.1.clone(), "int function returning void"));
                }
                _ => {}
            };
            if let Some(expr) = expr {
                visit_expr(&ctx, expr)?;
            }
        }
        Stmt::Break => {
            if !&ctx.loop_scope {
                return Err(Error::custom(
                    stmt.1.clone(),
                    "break statement not within loop",
                ));
            }
        }
        Stmt::Continue => {
            if !&ctx.loop_scope {
                return Err(Error::custom(
                    stmt.1.clone(),
                    "continue statement not within loop",
                ));
            }
        }
        Stmt::Assign { lhs, rhs } => {
            let ty_lhs = visit_lval(&ctx, lhs)?;
            let ty_rhs = visit_expr(&ctx, rhs)?;
            match (ty_lhs.clone(), ty_rhs.clone()) {
                (
                    VarType::Primitive(PrimitiveType::Int),
                    VarType::Primitive(PrimitiveType::Int),
                ) => {}
                _ => {
                    return Err(Error::custom(
                        stmt.1.clone(),
                        format!(
                            "invalid type for assignment, found {} and {}",
                            ty_lhs.clone(),
                            ty_rhs.clone()
                        ),
                    ));
                }
            }
        }
    }
    Ok(has_return)
}

fn visit_lval<'src>(ctx: &Context, lval: &Spanned<LVal<'src>>) -> Result<VarType, Error> {
    match &lval.0 {
        LVal::Var(name) => {
            if let Some((ty, _)) = ctx.var_table.get(name.0) {
                Ok(ty.clone())
            } else {
                Err(Error::custom(
                    name.1.clone(),
                    format!("variable '{}' not declared", name.0),
                ))
            }
        }
        LVal::Array { name, index } => {
            if let Some((ty, _)) = ctx.var_table.get(name.0) {
                match ty {
                    VarType::Array(ty, dims) => {
                        if dims.len() < index.len() {
                            return Err(Error::custom(
                                lval.1.clone(),
                                format!(
                                    "array '{}' expects at most {} dimensions, found {}",
                                    name.0,
                                    dims.len(),
                                    index.len()
                                ),
                            ));
                        }
                        for idx in index {
                            let ty = visit_expr(ctx, idx)?;
                            match &ty {
                                VarType::Primitive(PrimitiveType::Int) => {}
                                _ => {
                                    return Err(Error::custom(
                                        idx.1.clone(),
                                        format!("expects int for array index, found {}", &ty),
                                    ));
                                }
                            }
                        }
                        let new_dims = dims[index.len()..].to_vec();
                        if new_dims.is_empty() {
                            Ok(VarType::Primitive(ty.clone()))
                        } else {
                            Ok(VarType::Array(ty.clone(), new_dims))
                        }
                    }
                    _ => Err(Error::custom(
                        lval.1.clone(),
                        format!("'{}' is not an array", name.0),
                    )),
                }
            } else {
                Err(Error::custom(
                    name.1.clone(),
                    format!("array '{}' not declared", name.0),
                ))
            }
        }
    }
}

fn visit_expr<'src>(ctx: &Context, expr: &Spanned<Expr<'src>>) -> Result<VarType, Error> {
    Ok(match &expr.0 {
        Expr::Lit(_) => VarType::Primitive(PrimitiveType::Int),
        Expr::LVal(lval) => visit_lval(ctx, lval)?,
        Expr::Call { func, args } => {
            if !ctx.func_table.contains_key(func.0) {
                return Err(Error::custom(
                    func.1.clone(),
                    format!("function '{}' not declared", func.0),
                ));
            }
            let FuncType { params, return_ty } = ctx.func_table.get(func.0).unwrap();
            if params.len() != args.len() {
                return Err(Error::custom(
                    expr.1.clone(),
                    format!(
                        "function '{}' expects {} arguments, found {}",
                        func.0,
                        params.len(),
                        args.len()
                    ),
                ));
            }
            for (arg, param) in args.iter().zip(params.iter()) {
                let ty = visit_expr(ctx, arg)?;
                if ty != *param {
                    return Err(Error::custom(
                        arg.1.clone(),
                        format!(
                            "invalid type for argument, expected {}, found {}",
                            param, ty
                        ),
                    ));
                }
            }
            VarType::Primitive(return_ty.clone())
        }
        Expr::UnOpExpr { op: _, rhs } => {
            let ty = visit_expr(ctx, rhs)?;
            match ty {
                VarType::Primitive(PrimitiveType::Int) => {}
                _ => {
                    return Err(Error::custom(
                        rhs.1.clone(),
                        format!("expects int for unary operator, found {}", ty),
                    ));
                }
            }
            VarType::Primitive(PrimitiveType::Int)
        }
        Expr::BinOpExpr { lhs, op: _, rhs } => {
            let ty_lhs = visit_expr(ctx, lhs)?;
            let ty_rhs = visit_expr(ctx, rhs)?;
            match (&ty_lhs, &ty_rhs) {
                (
                    VarType::Primitive(PrimitiveType::Int),
                    VarType::Primitive(PrimitiveType::Int),
                ) => {}
                _ => {
                    return Err(Error::custom(
                        lhs.1.clone(),
                        format!(
                            "expects ints for binary operator, found {} and {}",
                            &ty_lhs,
                            &ty_rhs
                        ),
                    ));
                }
            }
            VarType::Primitive(PrimitiveType::Int)
        }
    })
}

pub fn check<'src>(module: &Spanned<Module<'src>>) -> Result<(), Error> {
    let mut ctx = Context::new().add_builtin_funcs();
    for decl in &module.0.decls {
        match decl {
            Decl::FuncDecl(func_decl) => {
                ctx = visit_func_decl(&ctx, func_decl)?;
            }
            Decl::VarDecl(var_decl) => {
                ctx = visit_var_decl(&ctx, var_decl)?;
            }
        }
    }
    Ok(())
}
