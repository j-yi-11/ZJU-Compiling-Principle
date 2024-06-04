use crate::ast::ast::*;
use crate::utils::error::{Error, Spanned};
use accipit::ir::builders::IRBuilder;
use accipit::ir::structures::Module as Program;
use accipit::ir::structures::{BasicBlock, BlockRef, Function, FunctionRef, Value, ValueRef};
use accipit::ir::types::Type;
use accipit::ir::values::{self, ConstantInt, ConstantUnit, GlobalVar};
use rpds::HashTrieMap;
use std::cell::RefCell;
use std::rc::Rc;

#[derive(Debug)]
struct Var {
    ty: VarType,
    addr: ValueRef,
}

#[derive(Clone)]
struct Context {
    func_table: HashTrieMap<String, FuncType>,
    var_table: HashTrieMap<String, Var>,

    ret_val: Option<ValueRef>,

    break_bb: Option<BlockRef>,
    cont_bb: Option<BlockRef>,

    builder: Rc<RefCell<IRBuilder>>,
}

impl Context {
    fn new(builder: Rc<RefCell<IRBuilder>>) -> Self {
        Self {
            func_table: HashTrieMap::new(),
            var_table: HashTrieMap::new(),

            ret_val: None,

            break_bb: None,
            cont_bb: None,

            builder,
        }
    }

    fn add_builtin_funcs(&self) -> Self {
        let mut new_ctx = self.clone();
        new_ctx.func_table = new_ctx
            .func_table
            .insert(
                "putint".to_string(),
                FuncType {
                    params: vec![VarType::Primitive(PrimitiveType::Int)],
                    return_ty: PrimitiveType::Void,
                },
            )
            .insert(
                "putch".to_string(),
                FuncType {
                    params: vec![VarType::Primitive(PrimitiveType::Int)],
                    return_ty: PrimitiveType::Void,
                },
            )
            .insert(
                "putarray".to_string(),
                FuncType {
                    params: vec![
                        VarType::Primitive(PrimitiveType::Int),
                        VarType::Array(PrimitiveType::Int, vec![0]),
                    ],
                    return_ty: PrimitiveType::Void,
                },
            )
            .insert(
                "getint".to_string(),
                FuncType {
                    params: vec![],
                    return_ty: PrimitiveType::Int,
                },
            )
            .insert(
                "getch".to_string(),
                FuncType {
                    params: vec![],
                    return_ty: PrimitiveType::Int,
                },
            )
            .insert(
                "getarray".to_string(),
                FuncType {
                    params: vec![
                        VarType::Primitive(PrimitiveType::Int),
                        VarType::Array(PrimitiveType::Int, vec![0]),
                    ],
                    return_ty: PrimitiveType::Void,
                },
            );
        let mut builder = self.builder.borrow_mut();
        builder.emit_function(
            "putint".to_string(),
            vec![(Some("x".to_string()), Type::get_i32())],
            Type::get_unit(),
            true,
        );

        builder.emit_function(
            "putch".to_string(),
            vec![(Some("x".to_string()), Type::get_i32())],
            Type::get_unit(),
            true,
        );

        builder.emit_function(
            "putarray".to_string(),
            vec![
                (Some("n".to_string()), Type::get_i32()),
                (Some("arr".to_string()), Type::get_pointer(Type::get_i32())),
            ],
            Type::get_unit(),
            true,
        );

        builder.emit_function("getint".to_string(), vec![], Type::get_i32(), true);

        builder.emit_function("getch".to_string(), vec![], Type::get_i32(), true);

        builder.emit_function(
            "getarray".to_string(),
            vec![
                (Some("n".to_string()), Type::get_i32()),
                (Some("arr".to_string()), Type::get_pointer(Type::get_i32())),
            ],
            Type::get_unit(),
            true,
        );
        new_ctx
    }

    fn add_func(&self, name: &str, ty: FuncType) -> Self {
        let mut new_ctx = self.clone();
        new_ctx.func_table = new_ctx.func_table.insert(name.to_string(), ty);
        new_ctx
    }

    fn add_var(&self, name: &str, ty: VarType, addr: ValueRef) -> Self {
        let mut new_ctx = self.clone();
        new_ctx.var_table = new_ctx.var_table.insert(name.to_string(), Var { ty, addr });
        new_ctx
    }
}

fn visit_var_decl<'src>(
    ctx: &Context,
    var_decl: &Spanned<VarDecl<'src>>,
    is_global: bool,
) -> Context {
    let mut new_ctx = ctx.clone();
    for (def, span) in &var_decl.0.defs {
        let (ty, ty_ir, size) = match &def.dims {
            Some(dims) => (
                VarType::Array(PrimitiveType::Int, dims.clone()),
                Type::get_i32(),
                dims.iter().fold(1, |acc, x| acc * x) as usize,
            ),
            None => (VarType::Primitive(PrimitiveType::Int), Type::get_i32(), 1),
        };

        if is_global {
            let mut global_var = GlobalVar::new_value(ty_ir, size);
            global_var.name = Some(def.name.0.to_string());
            let global_val = ctx.builder.borrow_mut().insert_global_symbol(global_var);
            new_ctx = new_ctx.add_var(def.name.0, ty, global_val);
        } else {
            let addr = ctx.builder.borrow_mut().emit_alloca(
                Some(def.name.0.to_string()),
                ty_ir,
                size,
                None,
            );
            if let Some(init) = &def.init {
                let init_val = visit_expr(&new_ctx, init);
                ctx.builder
                    .borrow_mut()
                    .emit_store(None, init_val, addr, None);
            }
            new_ctx = new_ctx.add_var(def.name.0, ty, addr);
        }
    }
    new_ctx
}

fn visit_func_decl<'src>(ctx: &Context, func_decl: &Spanned<FuncDecl<'src>>) -> Context {
    let mut new_func_ctx = ctx.clone();
    let mut params_ty = Vec::new();
    let mut params_ty_ir = Vec::new();
    for param in &func_decl.0.params {
        let ty = match &param.0.dims {
            Some(dims) => VarType::Array(param.0.ty.clone(), dims.clone()),
            None => VarType::Primitive(param.0.ty.clone()),
        };
        let ty_ir = match &param.0.dims {
            Some(dims) => Type::get_pointer(Type::get_i32()),
            None => Type::get_i32(),
        };
        params_ty.push(ty.clone());
        params_ty_ir.push((Some(param.0.name.0.to_string()), ty_ir));
    }

    let func_ty = FuncType {
        params: params_ty.clone(),
        return_ty: func_decl.0.return_ty.clone(),
    };

    let return_ty_builder = match &func_decl.0.return_ty {
        PrimitiveType::Int => Type::get_i32(),
        PrimitiveType::Void => Type::get_unit(),
    };

    ctx.builder.borrow_mut().emit_function(
        func_decl.0.name.0.to_string(),
        params_ty_ir.clone(),
        return_ty_builder,
        false,
    );

    let entry_bb = ctx
        .builder
        .borrow_mut()
        .emit_basic_block(Some("entry".to_string()));
    ctx.builder.borrow_mut().set_insert_point(entry_bb);
    for ((name, ty_ir), ty) in params_ty_ir.into_iter().zip(params_ty.into_iter()) {
        match &ty {
            VarType::Primitive(PrimitiveType::Int) => {
                let name_with_addr = name.as_ref().map(|name| format!("{}.addr", name));
                let addr =
                    ctx.builder
                        .borrow_mut()
                        .emit_alloca(name_with_addr.clone(), ty_ir, 1, None);
                new_func_ctx = new_func_ctx.add_var(name.as_ref().unwrap(), ty.clone(), addr);
                let arg = ctx
                    .builder
                    .borrow_mut()
                    .get_value_ref(&name.unwrap())
                    .unwrap();
                ctx.builder.borrow_mut().emit_store(None, arg, addr, None);
            }
            VarType::Array(PrimitiveType::Int, dims) => {
                let arg = ctx
                    .builder
                    .borrow_mut()
                    .get_value_ref(name.as_ref().unwrap())
                    .unwrap();
                new_func_ctx = new_func_ctx.add_var(name.as_ref().unwrap(), ty.clone(), arg);
            }
            _ => unreachable!(),
        }
    }
    match &func_decl.0.return_ty {
        PrimitiveType::Int => {
            let ret_val_addr = ctx.builder.borrow_mut().emit_alloca(
                Some("ret_val.addr".to_string()),
                Type::get_i32(),
                1,
                None,
            );
            new_func_ctx.ret_val = Some(ret_val_addr);
        }
        _ => {}
    }

    new_func_ctx = new_func_ctx.add_func(func_decl.0.name.0, func_ty.clone());

    visit_block(&new_func_ctx, &func_decl.0.body);

    let exit_bb = ctx
        .builder
        .borrow_mut()
        .emit_basic_block(Some("exit".to_string()));
    ctx.builder.borrow_mut().fixup_terminator_jump(exit_bb);
    ctx.builder.borrow_mut().set_insert_point(exit_bb);

    if let Some(ret_val_addr) = new_func_ctx.ret_val.as_ref() {
        let ret_val = ctx
            .builder
            .borrow_mut()
            .emit_load(None, ret_val_addr.clone(), None);
        ctx.builder.borrow_mut().fixup_terminator_return(ret_val);
    } else {
        let unit_val = ctx
            .builder
            .borrow_mut()
            .insert_literal_value(ConstantUnit::new_value());
        ctx.builder.borrow_mut().fixup_terminator_return(unit_val);
    }

    ctx.add_func(func_decl.0.name.0, func_ty)
}

fn visit_block<'src>(ctx: &Context, block: &Spanned<Block<'src>>) {
    let mut new_ctx = ctx.clone();
    for stmt in &block.0.stmts {
        match stmt {
            BlockItem::Stmt(stmt) => {
                visit_stmt(&new_ctx, stmt);
            }
            BlockItem::Decl(decl) => {
                new_ctx = visit_var_decl(&new_ctx, decl, false);
            }
        }
    }
}

fn visit_stmt<'src>(ctx: &Context, stmt: &Spanned<Stmt<'src>>) {
    match &stmt.0 {
        Stmt::Expr(expr) => {
            visit_expr(ctx, expr);
        }
        Stmt::Block(block) => {
            visit_block(ctx, block);
        }
        Stmt::If { cond, then, els } => {
            let cond_val = visit_expr(ctx, cond);
            let then_bb = ctx
                .builder
                .borrow_mut()
                .emit_basic_block(Some("if_then".to_string()));
            let end_bb = ctx
                .builder
                .borrow_mut()
                .emit_basic_block(Some("if_end".to_string()));
            if let Some(els) = els {
                let else_bb = ctx
                    .builder
                    .borrow_mut()
                    .emit_basic_block(Some("if_else".to_string()));
                ctx.builder
                    .borrow_mut()
                    .fixup_terminator_branch(cond_val, then_bb, else_bb);
                ctx.builder.borrow_mut().set_insert_point(then_bb);
                visit_stmt(ctx, then);
                ctx.builder.borrow_mut().fixup_terminator_jump(end_bb);
                ctx.builder.borrow_mut().set_insert_point(else_bb);
                visit_stmt(ctx, els);
                ctx.builder.borrow_mut().fixup_terminator_jump(end_bb);
                ctx.builder.borrow_mut().set_insert_point(end_bb);
            } else {
                ctx.builder
                    .borrow_mut()
                    .fixup_terminator_branch(cond_val, then_bb, end_bb);
                ctx.builder.borrow_mut().set_insert_point(then_bb);
                visit_stmt(ctx, then);
                ctx.builder.borrow_mut().fixup_terminator_jump(end_bb);
                ctx.builder.borrow_mut().set_insert_point(end_bb);
            }
        }
        Stmt::While { cond, body } => {
            let mut new_ctx = ctx.clone();
            let cond_bb = ctx
                .builder
                .borrow_mut()
                .emit_basic_block(Some("while_cond".to_string()));
            let body_bb = ctx
                .builder
                .borrow_mut()
                .emit_basic_block(Some("while_body".to_string()));
            let end_bb = ctx
                .builder
                .borrow_mut()
                .emit_basic_block(Some("while_end".to_string()));
            ctx.builder.borrow_mut().fixup_terminator_jump(cond_bb);
            ctx.builder.borrow_mut().set_insert_point(cond_bb);
            let cond_val = visit_expr(ctx, cond);
            ctx.builder
                .borrow_mut()
                .fixup_terminator_branch(cond_val, body_bb, end_bb);
            ctx.builder.borrow_mut().set_insert_point(body_bb);
            new_ctx.break_bb = Some(end_bb);
            new_ctx.cont_bb = Some(cond_bb);
            visit_stmt(&new_ctx, body);
            ctx.builder.borrow_mut().fixup_terminator_jump(cond_bb);
            ctx.builder.borrow_mut().set_insert_point(end_bb);
        }
        Stmt::Return(expr) => {
            if let Some(expr) = expr {
                let ret_val_addr = ctx.ret_val.unwrap();
                let ret_val = visit_expr(ctx, expr);
                ctx.builder
                    .borrow_mut()
                    .emit_store(None, ret_val, ret_val_addr, None);
            }
            let exit_bb = ctx
                .builder
                .borrow_mut()
                .get_or_insert_placeholder_block_ref("exit");
            ctx.builder.borrow_mut().fixup_terminator_jump(exit_bb);
            let new_bb = ctx
                .builder
                .borrow_mut()
                .emit_basic_block(Some("after_return".to_string()));
            ctx.builder.borrow_mut().set_insert_point(new_bb);
        }
        Stmt::Break => {
            let break_bb = ctx.break_bb.unwrap();
            ctx.builder.borrow_mut().fixup_terminator_jump(break_bb);
            let new_bb = ctx
                .builder
                .borrow_mut()
                .emit_basic_block(Some("after_break".to_string()));
            ctx.builder.borrow_mut().set_insert_point(new_bb);
        }
        Stmt::Continue => {
            let cont_bb = ctx.cont_bb.unwrap();
            ctx.builder.borrow_mut().fixup_terminator_jump(cont_bb);
            let new_bb = ctx
                .builder
                .borrow_mut()
                .emit_basic_block(Some("after_continue".to_string()));
            ctx.builder.borrow_mut().set_insert_point(new_bb);
        }
        Stmt::Assign { lhs, rhs } => {
            let val = visit_expr(ctx, rhs);
            let (addr, is_pointer) = visit_lval(ctx, lhs);
            assert!(!is_pointer);
            ctx.builder.borrow_mut().emit_store(None, val, addr, None);
        }
    }
}

fn visit_lval<'src>(ctx: &Context, lval: &Spanned<LVal<'src>>) -> (ValueRef, bool) {
    match &lval.0 {
        LVal::Var(name) => {
            let var = ctx.var_table.get(name.0).unwrap();
            let is_pointer = match &var.ty {
                VarType::Primitive(_) => false,
                VarType::Array(_, _) => true,
            };
            (var.addr, is_pointer)
        }
        LVal::Array { name, index } => {
            let var = ctx.var_table.get(name.0).unwrap();
            let addr = var.addr;
            let ty_ir = Type::get_i32();
            let mut indices = index
                .iter()
                .map(|idx| visit_expr(ctx, idx))
                .collect::<Vec<_>>();
            let index_len = indices.len();
            let bounds = match &var.ty {
                VarType::Array(_, bounds) => bounds
                    .iter()
                    .map(|bound| {
                        if *bound == 0 {
                            None
                        } else {
                            Some(*bound as usize)
                        }
                    })
                    .collect::<Vec<_>>(),
                _ => unreachable!(),
            };
            while indices.len() < bounds.len() {
                indices.push(
                    ctx.builder
                        .borrow_mut()
                        .insert_literal_value(ConstantInt::new_value(0)),
                );
            }
            let indices_bounds = indices
                .into_iter()
                .zip(bounds.iter().copied())
                .collect::<Vec<_>>();
            let addr =
                ctx.builder
                    .borrow_mut()
                    .emit_offset(None, ty_ir, addr, indices_bounds, None);
            let is_pointer = index_len < bounds.len();
            (addr, is_pointer)
        }
    }
}

fn visit_expr<'src>(ctx: &Context, expr: &Spanned<Expr<'src>>) -> ValueRef {
    match &expr.0 {
        Expr::Lit(lit) => {
            let val = ctx
                .builder
                .borrow_mut()
                .insert_literal_value(ConstantInt::new_value(*lit as i32));
            val
        }
        Expr::LVal(lval) => {
            let (addr, is_pointer) = visit_lval(ctx, lval);
            if is_pointer {
                addr
            } else {
                ctx.builder.borrow_mut().emit_load(None, addr, None)
            }
        }
        Expr::Call { func, args } => {
            let func_name = func.0.to_string();
            let args_val = args
                .iter()
                .map(|arg| visit_expr(ctx, arg))
                .collect::<Vec<_>>();
            ctx.builder
                .borrow_mut()
                .emit_function_call(None, func_name, args_val, None)
        }
        Expr::UnOpExpr { op, rhs } => match op {
            UnOp::Neg => {
                let rhs_val = visit_expr(ctx, rhs);
                let zero_val = ctx
                    .builder
                    .borrow_mut()
                    .insert_literal_value(ConstantInt::new_value(0));
                ctx.builder.borrow_mut().emit_numeric_binary_expr(
                    values::BinaryOp::Sub,
                    None,
                    zero_val,
                    rhs_val,
                    None,
                )
            }
            UnOp::Not => {
                let rhs_val = visit_expr(ctx, rhs);
                let zero_val = ctx
                    .builder
                    .borrow_mut()
                    .insert_literal_value(ConstantInt::new_value(0));
                ctx.builder.borrow_mut().emit_numeric_binary_expr(
                    values::BinaryOp::Eq,
                    None,
                    zero_val,
                    rhs_val,
                    None,
                )
            }
            UnOp::Pos => visit_expr(ctx, rhs),
        },
        Expr::BinOpExpr { op, lhs, rhs } => {
            let lhs_val = visit_expr(ctx, lhs);

            let op = match op {
                BinOp::Add => values::BinaryOp::Add,
                BinOp::Sub => values::BinaryOp::Sub,
                BinOp::Mul => values::BinaryOp::Mul,
                BinOp::Div => values::BinaryOp::Div,
                BinOp::Mod => values::BinaryOp::Rem,
                BinOp::Eq => values::BinaryOp::Eq,
                BinOp::Neq => values::BinaryOp::Ne,
                BinOp::Lt => values::BinaryOp::Lt,
                BinOp::Gt => values::BinaryOp::Gt,
                BinOp::Lte => values::BinaryOp::Le,
                BinOp::Gte => values::BinaryOp::Ge,
                BinOp::And => values::BinaryOp::And,
                BinOp::Or => values::BinaryOp::Or,
            };
            // println!("op: {:?}", op);
            match &op {
                values::BinaryOp::And | values::BinaryOp::Or => {
                    let short_addr = ctx.builder.borrow_mut().emit_alloca(
                        Some("short_val.addr".to_string()),
                        Type::get_i32(),
                        1,
                        None,
                    );
                    ctx.builder
                        .borrow_mut()
                        .emit_store(None, lhs_val, short_addr, None);
                    let rhs_bb = ctx
                        .builder
                        .borrow_mut()
                        .emit_basic_block(Some("short.rhs".to_string()));
                    let end_bb = ctx
                        .builder
                        .borrow_mut()
                        .emit_basic_block(Some("short.end".to_string()));
                    if op == values::BinaryOp::And {
                        ctx.builder
                            .borrow_mut()
                            .fixup_terminator_branch(lhs_val, rhs_bb, end_bb);
                    } else {
                        ctx.builder
                            .borrow_mut()
                            .fixup_terminator_branch(lhs_val, end_bb, rhs_bb);
                    }
                    ctx.builder.borrow_mut().set_insert_point(rhs_bb);
                    let rhs_val = visit_expr(ctx, rhs);
                    ctx.builder
                        .borrow_mut()
                        .emit_store(None, rhs_val, short_addr, None);
                    ctx.builder.borrow_mut().fixup_terminator_jump(end_bb);
                    ctx.builder.borrow_mut().set_insert_point(end_bb);
                    ctx.builder.borrow_mut().emit_load(None, short_addr, None)
                }
                _ => {
                    let rhs_val = visit_expr(ctx, rhs);
                    ctx.builder
                        .borrow_mut()
                        .emit_numeric_binary_expr(op, None, lhs_val, rhs_val, None)
                }
            }
        }
    }
}

pub fn build<'src>(module: &Spanned<Module<'src>>, builder: Rc<RefCell<IRBuilder>>) {
    let mut ctx = Context::new(builder.clone()).add_builtin_funcs();
    for decl in &module.0.decls {
        match decl {
            Decl::FuncDecl(..) => {}
            Decl::VarDecl(var_decl) => {
                ctx = visit_var_decl(&ctx, var_decl, true);
            }
        }
    }
    for decl in &module.0.decls {
        match decl {
            Decl::FuncDecl(func_decl) => {
                visit_func_decl(&ctx, func_decl);
            }
            Decl::VarDecl(..) => {}
        }
    }
}
