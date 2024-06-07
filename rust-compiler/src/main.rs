use ariadne::{sources, Color, Label, Report, ReportKind};
use chumsky::prelude::*;
use std::rc::Rc;
use std::cell::RefCell;
// use std::{borrow::Borrow, collections::HashMap, hash::Hash};
mod ast;
mod frontend;
mod midend;
mod utils;

// use ast::*;
use frontend::lexer::lexer;
use frontend::parser::parser;
use midend::type_checker;
use midend::init_globals;
use midend::tree2ssa;
use accipit::ir::builders::IRBuilder;

use utils::error::Span;

// use std::ffi::{CString, OsStr};
// use std::mem::MaybeUninit;
// use std::path::{Path, PathBuf};
// use std::ptr::null_mut;

fn main() -> Result<(), i64> {
    let filename = std::env::args().nth(1).unwrap();
    let ir_filename = std::env::args().nth(2);
    let src = std::fs::read_to_string(&filename).unwrap();

    println!("Source:\n{:#?}\n", src);
    let (tokens, errs) = lexer().parse(src.as_str()).into_output_errors();
    println!("Tokens:\n{:?}\n", tokens);
    let mut errs = errs
        .clone()
        .into_iter()
        .map(|e| e.map_token(|c| c.to_string()))
        .collect::<Vec<_>>();

    let ast = if let Some(tokens) = &tokens {
        let (ast, parse_errs) = parser()
            .parse(
                tokens
                    .as_slice()
                    .spanned(Span::new(src.len(), src.len()))
                    .into(),
            )
            .into_output_errors();

        errs = errs
            .into_iter()
            .chain(
                parse_errs
                    .into_iter()
                    .map(|e| e.map_token(|tok| tok.to_string())),
            )
            .collect::<Vec<_>>();
        ast
    } else {
        None
    };

    let mut ast_checked = ast.map(|ast| {
        if let Err(typecheck_errs) = type_checker::check(&ast) {
            errs.push(typecheck_errs)
        }
        ast
    });

    ast_checked.as_mut().map(|ast| init_globals::transform(ast));

    let builder = Rc::new(RefCell::new(IRBuilder::new()));

    ast_checked.map(|ast| {
        tree2ssa::build(&ast, builder.clone());
    });

    println!("IR:\n{}\n", builder.borrow().module.to_string());

    if let Some(ir_filename) = ir_filename {
        std::fs::write(ir_filename, builder.borrow().module.to_string()).unwrap();
    }

    
    if !errs.is_empty() {
        errs.clone().into_iter().for_each(|e| {
            Report::build(ReportKind::Error, filename.clone(), e.span().start)
                .with_message(e.to_string())
                .with_label(
                    Label::new((filename.clone(), e.span().into_range()))
                        .with_message(e.to_string())
                        .with_color(Color::Red),
                )
                .finish()
                .print(sources([(filename.clone(), src.clone())]))
                .unwrap()
        });
        return Err(1);
    }
    Ok(())
}
