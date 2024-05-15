use chumsky::prelude::{extra, Rich, SimpleSpan};

pub type Span = SimpleSpan<usize>;
pub type ParserError<'a, T> = extra::Err<Rich<'a, T, Span>>;
pub type Spanned<T> = (T, Span);
pub type Error = Rich<'static, String, Span, &'static str>;