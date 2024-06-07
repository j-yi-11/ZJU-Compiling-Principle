use crate::ast::ast::*;
use crate::utils::error::{Error, Span, Spanned};

pub fn transform<'src>(module: &mut Spanned<Module<'src>>) {
    let mut main_func = None;
    let mut globals_init: Vec<(Spanned<&'src str>, &Spanned<Expr<'src>>)> = Vec::new();
    for decl in &mut module.0.decls {
        match decl {
            Decl::FuncDecl(func_decl) => {
                if func_decl.0.name.0 == "main" {
                    main_func = Some(func_decl);
                }
            }
            Decl::VarDecl(var_decl) => {
                for (def, span) in &var_decl.0.defs {
                    if let Some(init) = &def.init {
                        globals_init.push((def.name, init));
                    }
                }
            }
        }
    }
    let main_func = main_func.expect("No main function found");
    let body = &mut main_func.0.body.0;
    let default_span = Span::new(0, 0);
    let init_stmts = globals_init
        .iter()
        .map(|(name, init)| {
            BlockItem::Stmt((
                Stmt::Assign {
                    lhs: (LVal::Var(name.clone()), default_span.clone()),
                    rhs: (*init).clone(),
                },
                default_span.clone(),
            ))
        })
        .collect::<Vec<_>>();
	let new_body = Block {
		stmts: init_stmts.into_iter().chain(body.stmts.clone()).collect(),
	};
	main_func.0.body = (new_body, main_func.0.body.1.clone());
}
