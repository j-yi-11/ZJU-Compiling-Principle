use crate::utils::error::Spanned;
use std::fmt;

#[derive(Debug)]
pub struct Module<'src> {
    pub decls: Vec<Decl<'src>>,
}

#[derive(Debug)]
pub enum Decl<'src> {
    FuncDecl(Spanned<FuncDecl<'src>>),
    VarDecl(Spanned<VarDecl<'src>>),
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum PrimitiveType {
    Int,
    Void,
}

impl fmt::Display for PrimitiveType {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        use PrimitiveType::*;
        match self {
            Int => write!(f, "int"),
            Void => write!(f, "void"),
        }
    }
}

#[derive(Debug, Clone)]
pub enum VarType {
    Primitive(PrimitiveType),
    Array(PrimitiveType, Vec<i64>),
}

impl PartialEq for VarType {
    fn eq(&self, other: &Self) -> bool {
        use VarType::*;
        match (self, other) {
            (Primitive(a), Primitive(b)) => a == b,
            (Array(a, adims), Array(b, bdims)) => {
                a == b
                    && if adims[0] == 0 || bdims[0] == 0 {
                        &adims[1..] == &bdims[1..]
                    } else {
                        adims == bdims
                    }
            }
            _ => false,
        }
    }
}

impl fmt::Display for VarType {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            VarType::Primitive(ty) => write!(f, "{}", ty),
            VarType::Array(ty, dims) => {
                write!(f, "{}", ty)?;
                if dims[0] == 0 {
                    write!(f, "[]")?;
                } else {
                    write!(f, "[{}]", dims[0])?;
                }
                for dim in &dims[1..] {
                    write!(f, "[{}]", dim)?;
                }
                Ok(())
            }
        }
    }
}

#[derive(Debug, Clone)]
pub struct FuncType {
    pub params: Vec<VarType>,
    pub return_ty: PrimitiveType,
}

#[derive(Debug)]
pub struct FuncParam<'src> {
    pub ty: PrimitiveType,
    pub name: Spanned<&'src str>,
    pub dims: Option<Vec<i64>>,
}

#[derive(Debug)]
pub struct FuncDecl<'src> {
    pub name: Spanned<&'src str>,
    pub params: Vec<Spanned<FuncParam<'src>>>,
    pub return_ty: PrimitiveType,
    pub body: Spanned<Block<'src>>,
}

#[derive(Debug, Clone)]
pub struct VarDef<'src> {
    pub name: Spanned<&'src str>,
    pub dims: Option<Vec<i64>>,
    pub init: Option<Spanned<Expr<'src>>>,
}

#[derive(Debug, Clone)]
pub struct VarDecl<'src> {
    pub ty: PrimitiveType,
    pub defs: Vec<Spanned<VarDef<'src>>>,
}

#[derive(Debug, Clone)]
pub enum BlockItem<'src> {
    Stmt(Spanned<Stmt<'src>>),
    Decl(Spanned<VarDecl<'src>>),
}

#[derive(Debug, Clone)]
pub struct Block<'src> {
    pub stmts: Vec<BlockItem<'src>>,
}

#[derive(Debug, Clone)]
pub enum LVal<'src> {
    Var(Spanned<&'src str>),
    Array {
        name: Spanned<&'src str>,
        index: Vec<Spanned<Expr<'src>>>,
    },
}

#[derive(Debug, Clone)]
pub enum Stmt<'src> {
    Expr(Spanned<Expr<'src>>),
    Block(Spanned<Block<'src>>),
    If {
        cond: Spanned<Expr<'src>>,
        then: Box<Spanned<Stmt<'src>>>,
        els: Option<Box<Spanned<Stmt<'src>>>>,
    },
    While {
        cond: Spanned<Expr<'src>>,
        body: Box<Spanned<Stmt<'src>>>, // only {}
    },
    Return(Option<Spanned<Expr<'src>>>),
    Break,
    Continue,
    Assign {
        lhs: Spanned<LVal<'src>>,
        rhs: Spanned<Expr<'src>>,
    },
}

#[derive(Debug, Clone)]
pub enum Expr<'src> {
    Lit(i64),
    LVal(Spanned<LVal<'src>>),
    Call {
        func: Spanned<&'src str>,
        args: Vec<Spanned<Expr<'src>>>,
    },
    UnOpExpr {
        op: UnOp,
        rhs: Box<Spanned<Expr<'src>>>,
    },
    BinOpExpr {
        lhs: Box<Spanned<Expr<'src>>>,
        op: BinOp,
        rhs: Box<Spanned<Expr<'src>>>,
    },
}

#[derive(Debug, Clone)]
pub enum BinOp {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Eq,
    Neq,
    Lt,
    Gt,
    Lte,
    Gte,
    And,
    Or,
}

impl fmt::Display for BinOp {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        use BinOp::*;
        match self {
            Add => write!(f, "+"),
            Sub => write!(f, "-"),
            Mul => write!(f, "*"),
            Div => write!(f, "/"),
            Mod => write!(f, "%"),
            Eq => write!(f, "=="),
            Neq => write!(f, "!="),
            Lt => write!(f, "<"),
            Gt => write!(f, ">"),
            Lte => write!(f, "<="),
            Gte => write!(f, ">="),
            And => write!(f, "&&"),
            Or => write!(f, "||"),
        }
    }
}

#[derive(Debug, Clone)]
pub enum UnOp {
    Neg,
    Pos,
    Not,
}

impl fmt::Display for UnOp {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        use UnOp::*;
        match self {
            Neg => write!(f, "-"),
            Pos => write!(f, "+"),
            Not => write!(f, "!"),
        }
    }
}
