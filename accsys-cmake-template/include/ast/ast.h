#pragma once
#include <cstdint>
#include <type_traits>
#include <string>
#include <vector>
enum OpType {
#define OpcodeDefine(x, s) x,
#include "common/common.def"
};

enum NodeType {
#define TreeNodeDefine(x) x,
#include "common/common.def"
};

struct Node;
using NodePtr = Node*;
struct TreeExpr;
using ExprPtr = TreeExpr*;
struct TreeType;

struct Node {
    NodeType node_type;
    Node(NodeType type) : node_type(type) {}
    template <typename T> bool is() {
        return node_type == std::remove_pointer_t<T>::this_type;
    }
    template <typename T> T as() {
        if (is<T>())
            return static_cast<T>(this);
        return nullptr;
    }
    template <typename T> T as_unchecked() { return static_cast<T>(this); }
};

/// comp unit
struct CompUnit : public Node {
    constexpr static NodeType this_type = ND_CompUnit;
    // all code
    std::vector<NodePtr> all;
    CompUnit() : Node(this_type) { }
};

/// primitive type
struct FuncType : public Node {
    constexpr static NodeType this_type = ND_PrimitiveType;
    enum Type {
        VOID = 0,
        INT = 1,
    };
    Type type;
    FuncType(Type type) : Node(this_type), type(type) {}
};

/// jy 5.14
struct addExp : public Node {
    constexpr static NodeType this_type = ND_AddExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    addExp() : Node(this_type) {}
};
struct subExp : public Node {
    constexpr static NodeType this_type = ND_SubExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    subExp() : Node(this_type) {}
};
struct mulExp : public Node {
    constexpr static NodeType this_type = ND_MulExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    mulExp() : Node(this_type) {}
};
struct divExp : public Node {
    constexpr static NodeType this_type = ND_DivExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    divExp() : Node(this_type) {}
};
struct modExp : public Node {
    constexpr static NodeType this_type = ND_ModExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    modExp() : Node(this_type) {}
};
struct andExp : public Node {
    constexpr static NodeType this_type = ND_AndExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    andExp() : Node(this_type) {}
};
struct orExp : public Node {
    constexpr static NodeType this_type = ND_OrExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    orExp() : Node(this_type) {}
};
struct lessExp : public Node {
    constexpr static NodeType this_type = ND_LtExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    lessExp() : Node(this_type) {}
};
struct greatExp : public Node {
    constexpr static NodeType this_type = ND_GtExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    greatExp() : Node(this_type) {}
};
struct leqExp : public Node {
    constexpr static NodeType this_type = ND_LeExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    leqExp() : Node(this_type) {}
};
struct geqExp : public Node {
    constexpr static NodeType this_type = ND_GeExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    geqExp() : Node(this_type) {}
};
struct eqExp : public Node {
    constexpr static NodeType this_type = ND_EqExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    eqExp() : Node(this_type) {}
};
struct neqExp : public Node {
    constexpr static NodeType this_type = ND_NeExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    neqExp() : Node(this_type) {}
};
struct notExp : public Node {
    constexpr static NodeType this_type = ND_NotExp;
    NodePtr lhs = nullptr;
    notExp() : Node(this_type) {}
};
struct negExp : public Node {
    constexpr static NodeType this_type = ND_NegExp;
    NodePtr lhs = nullptr;
    negExp() : Node(this_type) {}
};
struct posExp : public Node {
    constexpr static NodeType this_type = ND_PosExp;
    NodePtr lhs = nullptr;
    posExp() : Node(this_type) {}
};
struct intExp : public Node {
    constexpr static NodeType this_type = ND_IntegerConstExp;
    int64_t value = 0;
    intExp() : Node(this_type) {}
    intExp(int64_t value) : Node(this_type), value(value) {}
};
struct funcallExp : public Node {
    constexpr static NodeType this_type = ND_FuncCallExp;
    std::string name = "";
    std::vector<NodePtr> params;
    funcallExp() : Node(this_type) {}
};
struct assignExp : public Node {
    constexpr static NodeType this_type = ND_AssignExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    assignExp() : Node(this_type){}
};
/// Expr
//struct Expr : public Node {
//    constexpr static NodeType this_type = ND_Expr;
//    NodePtr LgExp = nullptr;
//    Expr() : Node(this_type) {}
//};

//struct LgExp : public Node {
//    constexpr static NodeType this_type = ND_LgExpr;
//    NodePtr lhs = nullptr;
//    NodePtr rhs = nullptr;
//    OpType optype;
//    LgExp() : Node(this_type) {}
//};
//
//struct CompExp : public Node {
//    constexpr static NodeType this_type = ND_CompExpr;
//    NodePtr lhs = nullptr;
//    NodePtr rhs = nullptr;
//    OpType optype;
//    CompExp() : Node(this_type) {}
//};

//struct AddExp : public Node {
//    constexpr static NodeType this_type = ND_AddExpr;
//    NodePtr mulExp = nullptr;
//    NodePtr addExp = nullptr;
//    OpType optype;
//    AddExp() : Node(this_type) {}
//};
//
//struct MulExp : public Node {
//    constexpr static NodeType this_type = ND_MulExpr;
//    NodePtr mulExp = nullptr;
//    NodePtr unaryExp = nullptr;
//    OpType optype;
//    MulExp() : Node(this_type) {}
//};

//struct PrimaryExpr : public Node {
//    constexpr static NodeType this_type = ND_PrimaryExpr;
//    NodePtr Exp = nullptr;
//    NodePtr LVal = nullptr;
//    NodePtr Number = nullptr;
//    PrimaryExpr() : Node(this_type) {}
//};

//struct UnaryExpr : public Node {
//    constexpr static NodeType this_type = ND_UnaryExpr;
//    OpType opType;
//    NodePtr primaryExp = nullptr;
//    NodePtr unaryExp = nullptr;
//    bool isFunCall = false;
//    std::vector<NodePtr> params;
//    std::string name = "";
//    UnaryExpr() : Node(this_type) { }
//};

//struct IntegerLiteral : public Node {
//    constexpr static NodeType this_type = ND_IntegerLiteral;
//    int64_t value = 0;
//    IntegerLiteral() : Node(this_type) {}
//    IntegerLiteral(int64_t value) : Node(this_type), value(value) {}
//};


/// lval
struct LVal : public Node {
    constexpr static NodeType this_type = ND_LVal;
    std::string name = "";
    bool isArray = false;
    std::vector<int> position; // array
    LVal() : Node(this_type) {}
};
/// statement
struct IfStmt : public Node {
    constexpr static NodeType this_type = ND_IfStmt;
    bool matched = false;
    NodePtr condition = nullptr;
    NodePtr then = nullptr;
    NodePtr els = nullptr;
    IfStmt(): Node(this_type) {/* ... */}
};

struct WhileStmt : public Node {
    constexpr static NodeType this_type = ND_WhileStmt;
    bool matched = false;
    NodePtr condition = nullptr;
    NodePtr then = nullptr;
    WhileStmt(): Node(this_type) { }
};

struct BreakStmt : public Node {
    constexpr static NodeType this_type = ND_BreakStmt;
    bool matched = false;
    BreakStmt(): Node(this_type){ }
};

struct ContinueStmt : public Node {
    constexpr static NodeType this_type = ND_ContinueStmt;
    bool matched = false;
    ContinueStmt(): Node(this_type){ }
};

struct ReturnStmt : public Node {
    constexpr static NodeType this_type = ND_ReturnStmt;
    bool matched = false;
    NodePtr result = nullptr;
    ReturnStmt(): Node(this_type){ }
};

struct BlockStmt : public Node {
    constexpr static NodeType this_type = ND_BlockStmt;
    bool matched = false;
    NodePtr Block = nullptr;
    BlockStmt(): Node(this_type){ }
};

struct ExpStmt : public Node {
    constexpr static NodeType this_type = ND_ExpStmt;
    bool matched = false;
    NodePtr Exp = nullptr;
    ExpStmt(): Node(this_type){ }
};

struct AssignStmt : public Node {
    constexpr static NodeType this_type = ND_AssignStmt;
    bool matched = false;
    NodePtr Exp = nullptr;
    NodePtr LVal = nullptr;
    AssignStmt(): Node(this_type){ }
};
/// definition
struct VarDef : public Node {
    constexpr static NodeType this_type = ND_VarDef;
    std::string name = "";
    // type is int
    NodePtr initialValue = nullptr;//var single
    bool isArray = false;
    std::vector<int> dimensions;// array
    VarDef() : Node(this_type){ }
};

struct FuncDef : public Node {
    constexpr static NodeType this_type = ND_FuncDef;
    std::string name = "";
    enum Type {
        VOID = 0,
        INT = 1,
    } ReturnType;
    std::vector<NodePtr> argList;// var , array
    NodePtr block = nullptr;
    FuncDef() : Node(this_type) { }
};
struct FuncFParam : public Node {
    constexpr static NodeType this_type = ND_FuncFParam;
    std::string name = "";
    bool isArray = false;
    std::vector<int> dimensions;// array
    FuncFParam() : Node(this_type) { }
};

/// declaration
struct Decl : public Node {
    constexpr static NodeType this_type = ND_Decl;
    NodePtr VarDecl = nullptr;
    Decl() : Node(this_type) {}
};
struct VarDecl : public Node {
    constexpr static NodeType this_type = ND_VarDecl;
    std::vector<NodePtr> VarDefs;
    VarDecl() : Node(this_type) {}
};

/// initial value
//struct InitVal : public Node {
//    constexpr static NodeType this_type = ND_InitVal;
//    NodePtr Exp = nullptr;
//    InitVal() : Node(this_type) {}
//};

/// block
struct Block : public Node {
    constexpr static NodeType this_type = ND_Block;
    std::vector<NodePtr> BlockItems;
    Block() : Node(this_type) {}
};
struct BlockItem : public Node {
    constexpr static NodeType this_type = ND_BlockItem;
    NodePtr Decl=nullptr;
    NodePtr Stmt=nullptr;
    BlockItem() : Node(this_type) {}
};
/// A possible helper function dipatch based on the type of `NodePtr`
void printAST(NodePtr exp, std::string prefix = "", std::string ident = "");

/// A possible helper function to print the AST
void print_vector_data(std::vector<NodePtr> &vec, std::string &prefix, std::string &ident);

void print_vector_data(std::vector<int> &vec, std::string &prefix, std::string &ident);