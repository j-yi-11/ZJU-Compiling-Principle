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

/// Expr
struct Expr : public Node {
    constexpr static NodeType this_type = ND_Expr;
    NodePtr LgExp = nullptr;
    Expr() : Node(this_type) {}
};

struct LgExp : public Node {
    constexpr static NodeType this_type = ND_LgExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    OpType optype;
    LgExp() : Node(this_type) {}
};

struct CompExp : public Node {
    constexpr static NodeType this_type = ND_CompExp;
    NodePtr lhs = nullptr;
    NodePtr rhs = nullptr;
    OpType optype;
    CompExp() : Node(this_type) {}
};

struct AddExp : public Node {
    constexpr static NodeType this_type = ND_AddExp;
    NodePtr mulExp = nullptr;
    NodePtr addExp = nullptr;
    OpType optype;
    AddExp() : Node(this_type) {}
};

struct MulExp : public Node {
    constexpr static NodeType this_type = ND_MulExp;
    NodePtr mulExp = nullptr;
    NodePtr unaryExp = nullptr;
    OpType optype;
    MulExp() : Node(this_type) {}
};

struct PrimaryExpr : public Node {
    constexpr static NodeType this_type = ND_PrimaryExpr;
    NodePtr Exp = nullptr;
    NodePtr LVal = nullptr;
    NodePtr Number = nullptr;
    PrimaryExpr() : Node(this_type) {}
};

struct TreeBinaryExpr : public Node {
    constexpr static NodeType this_type = ND_BinaryExpr;
    OpType op;
    NodePtr lhs, rhs;
    TreeBinaryExpr(OpType op, NodePtr lhs, NodePtr rhs)
        : Node(this_type), op(op), lhs(lhs), rhs(rhs) {
    }
};

struct UnaryExpr : public Node {
    constexpr static NodeType this_type = ND_UnaryExpr;
    OpType optype;
    NodePtr primaryExp = nullptr;
    NodePtr unaryExp = nullptr;
    std::vector<NodePtr> params;
    std::string name = "";
    UnaryExpr() : Node(this_type) { }
};

struct IntegerLiteral : public Node {
    constexpr static NodeType this_type = ND_IntegerLiteral;
    int64_t value = 0;
    IntegerLiteral() : Node(this_type) {}
    IntegerLiteral(int64_t value) : Node(this_type), value(value) {}
};

//// add func/var/array call
//struct TreeArrayCall : public Node {
//    constexpr static NodeType this_type = ND_ArrayCall;
//    std::string name = "";
//    // type is int
//    std::vector<int> positions;// array
//    TreeArrayCall(std::string &name, std::vector<int> &positions)
//        : Node(this_type), name(name), positions(positions) {
//    }
//};

//struct TreeVarCall : public Node {
//    constexpr static NodeType this_type = ND_VarCall;
//    std::string name = "";
//    // type is int
//    TreeVarCall(std::string &name)
//        : Node(this_type), name(name) {
//    }
//};

//struct TreeFunCall : public Node {
//    constexpr static NodeType this_type = ND_FunCall;
//    std::string name = "";
//    std::vector<Node *> argList;
//    int funReturnType = 0;// 0 is void, 1 is int
//    TreeFunCall(std::string &name, std::vector<Node *> &argList, int funReturnType)
//        : Node(this_type), name(name), argList(argList), funReturnType(funReturnType) {
//    }
//};
/// lval
struct LVal : public Node {
    constexpr static NodeType this_type = ND_LVal;
    std::string name = "";
    bool isArray = false;
    std::vector<int> position; // array
    LVal() : Node(this_type) {}
}
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
    VarDef() : Node(this_type){ }//std::string &name, int initialValue
        //, name(name), initialValue(initialValue)
};

// struct ArrayDef : public Node {
//     constexpr static NodeType this_type = ND_ArrayDef;
//     std::string name = "";
//     // type is int
//     std::vector<int> dimensions;// array
//     ArrayDef(std::string &name, std::vector<int> &dimensions) :
//         Node(this_type), name(name), dimensions(dimensions){ }
// };
struct FuncDef : public Node {
    constexpr static NodeType this_type = ND_FuncDef;
    std::string name = "";
    enum funcReturnType{
        VOID = 0, INT = 1
    };
    funcReturnType ReturnType = funcReturnType::VOID;
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
struct InitVal : public Node {
    constexpr static NodeType this_type = ND_InitVal;
    NodePtr Exp = nullptr;
    InitVal() : Node(this_type) {}
};

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
void print_expr(NodePtr exp, std::string prefix = "", std::string ident = "");