use super::lexer::Token;
use crate::ast::ast::*;
use crate::utils::error::{ParserError, Span, Spanned};
use chumsky::pratt::*;
use chumsky::{input::MapExtra, input::SpannedInput, prelude::*};

type ParserInput<'tokens, 'src> = SpannedInput<Token<'src>, Span, &'tokens [Spanned<Token<'src>>]>;

macro_rules! prefix_op {
    ($name:ident, $op:ident, $power:expr) => {
        prefix($power, just(Token::$op), |_, rhs, span| {
            (
                Expr::UnOpExpr {
                    op: UnOp::$name,
                    rhs: Box::new(rhs),
                },
                span,
            )
        })
    };
}

macro_rules! infix_op {
    ($name:ident, $op:ident, $assoc:expr) => {
        infix($assoc, just(Token::$op), |l, _, r, span| {
            (
                Expr::BinOpExpr {
                    op: BinOp::$name,
                    lhs: Box::new(l),
                    rhs: Box::new(r),
                },
                span,
            )
        })
    };
}

pub fn parser<'tokens, 'src: 'tokens>() -> impl Parser<
    'tokens,
    ParserInput<'tokens, 'src>,
    Spanned<Module<'src>>,
    ParserError<'tokens, Token<'src>>,
> + Clone {
    let ident = select! {
        Token::Ident(s) = e => (s, e.span())
    };
    let lit = select! {
        Token::Int(x) => x,
    };
    let ty = select! {
        Token::TyInt => PrimitiveType::Int,
        Token::TyVoid => PrimitiveType::Void,
    };

    let mut expr = Recursive::declare();

    let lval = ident
        .clone()
        .then(
            expr.clone()
                .delimited_by(just(Token::LBracket), just(Token::RBracket))
                .repeated()
                .at_least(1)
                .collect::<Vec<_>>()
                .or_not(),
        )
        .map_with(|(name, indices), e| {
            (
                if let Some(indices) = indices {
                    LVal::Array {
                        name,
                        index: indices,
                    }
                } else {
                    LVal::Var(name)
                },
                e.span(),
            )
        });

    let expr_lit = lit.map_with(|lit, e| (Expr::Lit(lit), e.span()));

    let expr_paren = expr
        .clone()
        .delimited_by(just(Token::LParen), just(Token::RParen));

    let expr_call = ident
        .clone()
        .then(
            expr.clone()
                .separated_by(just(Token::Comma))
                .collect::<Vec<_>>()
                .delimited_by(just(Token::LParen), just(Token::RParen)),
        )
        .map_with(|(name, args), e| (Expr::Call { func: name, args }, e.span()));

    let expr_lval = lval
        .clone()
        .map_with(|lval, e| (Expr::LVal(lval), e.span()));

    let term = choice((expr_lit, expr_paren, expr_call, expr_lval));

    let op = term.pratt((
        prefix_op!(Neg, Minus, 6),
        prefix_op!(Pos, Plus, 6),
        prefix_op!(Not, Not, 6),
        infix_op!(Mul, Mul, left(5)),
        infix_op!(Div, Div, left(5)),
        infix_op!(Mod, Mod, left(5)),
        infix_op!(Add, Plus, left(4)),
        infix_op!(Sub, Minus, left(4)),
        infix_op!(Lt, Lt, left(3)),
        infix_op!(Gt, Gt, left(3)),
        infix_op!(Lte, Lte, left(3)),
        infix_op!(Gte, Gte, left(3)),
        infix_op!(Eq, Eq, left(2)),
        infix_op!(Neq, Neq, left(2)),
        infix_op!(And, And, left(1)),
        infix_op!(Or, Or, left(0)),
    ));
    expr.define(op);

    let var_def = ident
        .clone()
        .then(
            lit.clone()
                .delimited_by(just(Token::LBracket), just(Token::RBracket))
                .repeated()
                .at_least(1)
                .collect::<Vec<i64>>()
                .or_not(),
        )
        .then(just(Token::Assign).ignore_then(expr.clone()).or_not())
        .map_with(
            |((name, dims), rhs),
             e: &mut MapExtra<
                '_,
                '_,
                ParserInput<'tokens, 'src>,
                ParserError<'tokens, Token<'src>>,
            >| {
                (
                    VarDef {
                        name,
                        dims,
                        init: rhs,
                    },
                    e.span(),
                )
            },
        );

    let var_decl = ty
        .clone()
        .then(
            var_def
                .clone()
                .separated_by(just(Token::Comma))
                .at_least(1)
                .collect::<Vec<Spanned<VarDef<'src>>>>(),
        )
        .then_ignore(just(Token::Semicolon))
        .map_with(
            |(ty, defs),
             e: &mut MapExtra<
                '_,
                '_,
                ParserInput<'tokens, 'src>,
                ParserError<'tokens, Token<'src>>,
            >| { (VarDecl { ty, defs }, e.span()) },
        );

    let mut stmt = Recursive::declare();
    let block_item = var_decl
        .clone()
        .map(BlockItem::Decl)
        .or(stmt.clone().map(BlockItem::Stmt));
    let block = block_item
        .clone()
        .repeated()
        .collect::<Vec<_>>()
        .delimited_by(just(Token::LBrace), just(Token::RBrace))
        .map_with(|stmts, e| (Block { stmts }, e.span()));
    stmt.define({
        let stmt_expr = expr
            .clone()
            .then_ignore(just(Token::Semicolon))
            .map_with(|expr, e| (Stmt::Expr(expr), e.span()));
        let stmt_block = block
            .clone()
            .map_with(|block, e| (Stmt::Block(block), e.span()));
        let stmt_assign = lval
            .clone()
            .then_ignore(just(Token::Assign))
            .then(expr.clone())
            .then_ignore(just(Token::Semicolon))
            .map_with(|(lhs, rhs), e| (Stmt::Assign { lhs, rhs }, e.span()));
        let stmt_if = just(Token::If)
            .ignore_then(
                expr.clone()
                    .delimited_by(just(Token::LParen), just(Token::RParen)),
            )
            .then(stmt.clone())
            .then(just(Token::Else).ignore_then(stmt.clone()).or_not())
            .map_with(|((cond, then), els), e| {
                (
                    Stmt::If {
                        cond,
                        then: Box::new(then),
                        els: els.map(Box::new),
                    },
                    e.span(),
                )
            });
        let stmt_while = just(Token::While)
            .ignore_then(
                expr.clone()
                    .delimited_by(just(Token::LParen), just(Token::RParen)),
            )
            .then(stmt.clone())
            .map_with(|(cond, body), e| {
                (
                    Stmt::While {
                        cond,
                        body: Box::new(body),
                    },
                    e.span(),
                )
            });
        let stmt_break = just(Token::Break)
            .then_ignore(just(Token::Semicolon))
            .map_with(|_, e| (Stmt::Break, e.span()));
        let stmt_continue = just(Token::Continue)
            .then_ignore(just(Token::Semicolon))
            .map_with(|_, e| (Stmt::Continue, e.span()));
        let stmt_return = just(Token::Return)
            .ignore_then(expr.clone().or_not())
            .then_ignore(just(Token::Semicolon))
            .map_with(|expr, e| (Stmt::Return(expr), e.span()));
        choice((
            stmt_expr,
            stmt_block,
            stmt_assign,
            stmt_if,
            stmt_while,
            stmt_break,
            stmt_continue,
            stmt_return,
        ))
    });

    let func_decl = ty
        .clone()
        .then(ident.clone())
        .then(
            ty.clone()
                .then(ident.clone())
                .then(
                    just(Token::LBracket)
                        .then(just(Token::RBracket))
                        .ignored()
                        .then(
                            just(Token::LBracket)
                                .ignore_then(lit.clone())
                                .then_ignore(just(Token::RBracket))
                                .repeated()
                                .collect::<Vec<i64>>(),
                        )
                        .map(|(_, mut dims)| {
                            let mut ret = vec![0];
                            ret.append(&mut dims);
                            ret
                        })
                        .or_not(),
                )
                .map_with(|((ty, name), dims), e| (FuncParam { ty, name, dims }, e.span()))
                .separated_by(just(Token::Comma))
                .collect::<Vec<Spanned<FuncParam<'src>>>>()
                .delimited_by(just(Token::LParen), just(Token::RParen)),
        )
        .then(block.clone())
        .map_with(|(((ret_ty, func_name), params), body), e| {
            Decl::FuncDecl((
                FuncDecl {
                    name: func_name,
                    params,
                    return_ty: ret_ty,
                    body,
                },
                e.span(),
            ))
        });
    let global_var_decl = var_decl
        .clone()
        .map_with(|decl, e| Decl::VarDecl((decl.0, e.span())));
    let module = choice((func_decl, global_var_decl))
        .repeated()
        .collect::<Vec<_>>()
        .map_with(|decls, e| (Module { decls }, e.span()));
    module.then_ignore(end())
}
