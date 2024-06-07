use chumsky::prelude::*;
use std::fmt;

use crate::utils::error::{ParserError, Spanned};

#[derive(Clone, Debug, PartialEq)]
pub enum Token<'src> {
    If,
    Else,
    For,
    While,
    Return,
    Break,
    Continue,
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Semicolon,
    Comma,
    SQuote,
    DQuote,
    Assign,
    Eq,
    Neq,
    Lt,
    Gt,
    Lte,
    Gte,
    Plus,
    Minus,
    Mul,
    Div,
    Mod,
    And,
    Or,
    Not,
    Dot,
    Backslash,
    TyInt,
    TyVoid,
    Int(i64),
    Ident(&'src str),
}

impl<'src> fmt::Display for Token<'src> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            Token::If => write!(f, "if"),
            Token::Else => write!(f, "else"),
            Token::For => write!(f, "for"),
            Token::While => write!(f, "while"),
            Token::Return => write!(f, "return"),
            Token::Break => write!(f, "break"),
            Token::Continue => write!(f, "continue"),
            Token::LParen => write!(f, "("),
            Token::RParen => write!(f, ")"),
            Token::LBrace => write!(f, "{{"),
            Token::RBrace => write!(f, "}}"),
            Token::LBracket => write!(f, "["),
            Token::RBracket => write!(f, "]"),
            Token::Semicolon => write!(f, ";"),
            Token::Comma => write!(f, ","),
            Token::SQuote => write!(f, "'"),
            Token::DQuote => write!(f, "\""),
            Token::Assign => write!(f, "="),
            Token::Eq => write!(f, "=="),
            Token::Neq => write!(f, "!="),
            Token::Lt => write!(f, "<"),
            Token::Gt => write!(f, ">"),
            Token::Lte => write!(f, "<="),
            Token::Gte => write!(f, ">="),
            Token::Plus => write!(f, "+"),
            Token::Minus => write!(f, "-"),
            Token::Mul => write!(f, "*"),
            Token::Div => write!(f, "/"),
            Token::Mod => write!(f, "%"),
            Token::And => write!(f, "&&"),
            Token::Or => write!(f, "||"),
            Token::Not => write!(f, "!"),
            Token::Dot => write!(f, "."),
            Token::Backslash => write!(f, "\\"),
            Token::TyInt => write!(f, "int"),
            Token::TyVoid => write!(f, "void"),
            Token::Int(i) => write!(f, "{}", i),
            Token::Ident(s) => write!(f, "{}", s),
        }
    }
}

pub fn lexer<'src>(
) -> impl Parser<'src, &'src str, Vec<Spanned<Token<'src>>>, ParserError<'src, char>> {
    let op = choice([
        just('=').then_ignore(just('=')).to(Token::Eq),
        just('!').then_ignore(just('=')).to(Token::Neq),
        just('<').then_ignore(just('=')).to(Token::Lte),
        just('>').then_ignore(just('=')).to(Token::Gte),
        just('&').then_ignore(just('&')).to(Token::And),
        just('|').then_ignore(just('|')).to(Token::Or),
    ])
    .or(choice([
        just('(').to(Token::LParen),
        just(')').to(Token::RParen),
        just('{').to(Token::LBrace),
        just('}').to(Token::RBrace),
        just('[').to(Token::LBracket),
        just(']').to(Token::RBracket),
        just(';').to(Token::Semicolon),
        just(',').to(Token::Comma),
        just('\'').to(Token::SQuote),
        just('"').to(Token::DQuote),
        just('=').to(Token::Assign),
        just('<').to(Token::Lt),
        just('>').to(Token::Gt),
        just('+').to(Token::Plus),
        just('-').to(Token::Minus),
        just('*').to(Token::Mul),
        just('/').to(Token::Div),
        just('%').to(Token::Mod),
        just('!').to(Token::Not),
        just('.').to(Token::Dot),
        just('\\').to(Token::Backslash),
        // just('_').to(Token::Underscore),
    ]));
    let keyword = choice([
        text::keyword("if").to(Token::If),
        text::keyword("else").to(Token::Else),
        text::keyword("for").to(Token::For),
        text::keyword("while").to(Token::While),
        text::keyword("return").to(Token::Return),
        text::keyword("break").to(Token::Break),
        text::keyword("continue").to(Token::Continue),
        text::keyword("int").to(Token::TyInt),
        text::keyword("void").to(Token::TyVoid),
    ]);
    let num = text::int::<&'src str, char, ParserError<'src, char>>(10)
        .from_str::<i64>()
        .unwrapped()
        .map(Token::Int);

    let ident = text::ident::<&'src str, char, ParserError<'src, char>>().map(Token::Ident);

    let token = num.or(op).or(keyword).or(ident);

    let single_comment = just::<_, &str, ParserError<'src, char>>("//")
        .ignore_then(none_of("\n").repeated())
        .then_ignore(just("\n").ignored().or(end()))
        .padded();

    let multi_comment = just::<_, &str, ParserError<'src, char>>("/*")
        .ignore_then(any().and_is(just("*/").not()).repeated())
        .then_ignore(just("*/"))
        .padded();

    let comment = single_comment.or(multi_comment);

    token
        .map_with(|tok, e| (tok, e.span()))
        .padded_by(comment.repeated())
        .padded()
        .recover_with(skip_then_retry_until(any().ignored(), end()))
        .repeated()
        .collect::<Vec<_>>()
}
