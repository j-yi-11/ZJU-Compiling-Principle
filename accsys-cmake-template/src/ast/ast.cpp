#include "ast/ast.h"

#include <fmt/core.h>
#include <cassert>

static std::string op_str(OpType op_type) {
    switch (op_type) {
#define OpcodeDefine(x, s)     \
        case x: return s;
#include "common/common.def"
    default:
        return "<unknown>";
    }
}
void print_vector_data(std::vector<NodePtr> &vec, std::string &prefix, std::string &ident) {
    for (int i = 0; i < vec.size(); i++) {
        bool is_last = i == vec.size() - 1;
        print_expr(vec[i], prefix + (is_last ? "\n└─ " : "\n├─ "), ident + " ");
    }
}

void print_vector_data(std::vector<int> &vec, std::string &prefix, std::string &ident) {
    for (int i = 0; i < vec.size(); i++) {
        bool is_last = i == vec.size() - 1;
        print_expr(vec[i], prefix + (is_last ? "\n└─ " : "\n├─ "), ident + " ");
    }
}

void print_expr(NodePtr exp, std::string prefix, std::string ident) {
    bool debug = false;
    if(debug)
        fmt::print("print_expr called\n");
    if(exp==nullptr){
        return;
    }
    if(debug){
        fmt::print("Address of exp: {0}\n", (void*) exp);
        fmt::print("length of prefix: {0}\n", prefix.length());
        fmt::print("length of ident: {0}\n", ident.length());
    }
    fmt::print(prefix);
    // print the expression type according to ast.h defines
    // comp unit
    if (auto *comp_unit = exp->as<CompUnit *>()) {
        fmt::print("CompUnit\n");
        // all ==> print if not null
//        for (auto &node : comp_unit->all) {
//            print_expr(node, ident + "└─ ", ident + "   ");
//        }
        print_vector_data(comp_unit->all, prefix, ident);
        fmt::print(ident);
        return;
    }
    // exp
    if (auto *expr = exp->as<Expr *>()) {
        //fmt::print("Expr\n");
        //print_expr(expr->LgExp, ident + "└─ ", ident + "   ");
        print_expr(expr->LgExp, prefix, ident);
        fmt::print(ident);
        return;
    }
    // lg_exp
    if (auto *lg_exp = exp->as<LgExp *>()) {
        fmt::print("LgExp \"{}\"\n", op_str(lg_exp->optype));
        print_expr(lg_exp->lhs, ident + "├─ ", ident + "   ");
        print_expr(lg_exp->rhs, ident + "└─ ", ident + "   ");
        fmt::print(ident);
        return;
    }
    // comp_exp
    if (auto *comp_exp = exp->as<CompExp *>()) {
        fmt::print("CompExp \"{}\"\n", op_str(comp_exp->optype));
        print_expr(comp_exp->lhs, ident + "├─ ", ident);
        print_expr(comp_exp->rhs, ident + "└─ ", ident);
        fmt::print(ident);
        return;
    }
    // add_exp
    if (auto *add_exp = exp->as<AddExp *>()) {
        fmt::print("AddExp \"{}\"\n", op_str(add_exp->optype));
        print_expr(add_exp->addExp, ident + "├─ ", ident);
        print_expr(add_exp->mulExp, ident + "└─ ", ident);
        fmt::print(ident);
        return;
    }
    // mul_exp
    if (auto *mul_exp = exp->as<MulExp *>()) {
        fmt::print("MulExp \"{}\"\n", op_str(mul_exp->optype));
        print_expr(mul_exp->mulExp, ident + "├─ ", ident);
        print_expr(mul_exp->unaryExp, ident + "└─ ", ident);
        fmt::print(ident);
        return;
    }
    // primary_exp
    if (auto *primary_exp = exp->as<PrimaryExpr *>()) {
        fmt::print("PrimaryExpr\n");
        // exp lval number ==> select the not null one to print
        if (primary_exp->Exp != nullptr) {
            print_expr(primary_exp->Exp, ident + "└─ ", ident);
        } else if (primary_exp->LVal != nullptr) {
            print_expr(primary_exp->LVal, ident + "└─ ", ident);
        } else if (primary_exp->Number != nullptr) {
            print_expr(primary_exp->Number, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // unary_exp
    if (auto *unary_exp = exp->as<UnaryExpr *>()) {
        fmt::print("UnaryExpr \"{}\"\n", op_str(unary_exp->opType));
        // primary_exp unary_exp params ==> select those not null to print
        if (unary_exp->primaryExp != nullptr) {
            print_expr(unary_exp->primaryExp, ident + "└─ ", ident);
        } else if (unary_exp->unaryExp != nullptr) {
            print_expr(unary_exp->unaryExp, ident + "└─ ", ident);
        } else {
            // params and name(for func call)
            for (auto &param : unary_exp->params) {
                print_expr(param, ident + "└─ ", ident);
            }
            fmt::print("{}\n", unary_exp->name);
        }
        fmt::print(ident);
        return;
    }
    // integer literal
    if (auto *lit = exp->as<IntegerLiteral *>()) {
        fmt::print("Int {}\n", lit->value);
        fmt::print(ident);
        return;
    }
    // lval  for left value
    if (auto *lval = exp->as<LVal *>()) {
        fmt::print("LVal \"{}\"\n", lval->name);
        // array
        if (lval->isArray) {
            // print array out
            for(auto &i : lval->position){
                fmt::print("{} ", i);
            }
        }
        fmt::print(ident);
        return;
    }
    // if statement
    if (auto *stmt = exp->as<IfStmt *>()) {
        fmt::print("IfStmt\n");
        // condition then else ==> print those not null
        print_expr(stmt->condition, ident + "├─ ", ident);
        print_expr(stmt->then, ident + "├─ ", ident);
        if (stmt->els != nullptr) {
            print_expr(stmt->els, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // while statement
    if (auto *stmt = exp->as<WhileStmt *>()) {
        fmt::print("WhileStmt\n");
        // condition then ==> print those not null
        print_expr(stmt->condition, ident + "├─ ", ident);
        print_expr(stmt->then, ident + "└─ ", ident);
        fmt::print(ident);
        return;
    }
    // break statement
    if (auto *stmt = exp->as<BreakStmt *>()) {
        fmt::print("BreakStmt\n");
        fmt::print(ident);
        return;
    }
    // continue statement
    if (auto *stmt = exp->as<ContinueStmt *>()) {
        fmt::print("ContinueStmt\n");
        fmt::print(ident);
        return;
    }
    // return statement
    if (auto *stmt = exp->as<ReturnStmt *>()) {
        fmt::print("ReturnStmt\n");
        // result ==> print if not null
        if (stmt->result != nullptr) {
            print_expr(stmt->result, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // block statement
    if (auto *stmt = exp->as<BlockStmt *>()) {
        fmt::print("BlockStmt\n");
        // block ==> print if not null
        if (stmt->Block != nullptr) {
            print_expr(stmt->Block, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // exp statement
    if (auto *stmt = exp->as<ExpStmt *>()) {
        fmt::print("ExpStmt\n");
        // exp ==> print if not null
        if (stmt->Exp != nullptr) {
            print_expr(stmt->Exp, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // assign statement
    if (auto *stmt = exp->as<AssignStmt *>()) {
        fmt::print("AssignStmt\n");
        // exp lval ==> print if not null
        if (stmt->Exp != nullptr) {
            print_expr(stmt->Exp, ident + "├─ ", ident);
        }
        if (stmt->LVal != nullptr) {
            print_expr(stmt->LVal, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // initial value
    if (auto *stmt = exp->as<InitVal *>()) {
        fmt::print("InitialValue\n");
        // exp ==> print if not null
        if (stmt->Exp != nullptr) {
            print_expr(stmt->Exp, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // decl
    if (auto *stmt = exp->as<Decl *>()) {
        fmt::print("Decl\n");
        // vardecl ==> print if not null
        if (stmt->VarDecl != nullptr) {
            print_expr(stmt->VarDecl, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // vardecl
    if (auto *stmt = exp->as<VarDecl *>()) {
        fmt::print("VarDecl\n");
        // vardefs ==> print if not null
        for (auto &vardef : stmt->VarDefs) {
            print_expr(vardef, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // vardef
    if (auto *stmt = exp->as<VarDef *>()) {
        fmt::print("VarDef");
        // name initialValue dimension ==> print if not null
        fmt::print(" {} ", stmt->name);
        if (stmt->initialValue != nullptr) {
            print_expr(stmt->initialValue, ident + "└─ ", ident);
        }
        for (auto &dim : stmt->dimensions) {
            fmt::print("{}\n", dim);
        }
        fmt::print(ident);
        return;
    }
    // func def
    if (auto *stmt = exp->as<FuncDef *>()) {
        fmt::print("FuncDef ");
        // return type
        if (stmt->ReturnType == FuncDef::Type::INT) {
            fmt::print("int ");
        } else {
            fmt::print("void ");
        }
        // name arglist block  ==> print if not null
        fmt::print("{}", stmt->name);
        if (stmt->argList.size() > 0) {
            fmt::print(" size {}\n", stmt->argList.size());
            for (auto &arg : stmt->argList) {
                print_expr(arg, ident + "└─ ", ident + "   ");
            }
        } else {
            fmt::print(" void ");
        }
        if (stmt->block != nullptr) {
            print_expr(stmt->block, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // Func FParam
    if (auto *stmt = exp->as<FuncFParam *>()) {
        fmt::print("FuncFParam ");
        // name isArray dimension ==> print if not null
        fmt::print("{} ", stmt->name);
        if (stmt->isArray) {
            fmt::print("dimension ");
            for (auto &dim : stmt->dimensions) {
                fmt::print("{} ", dim);
            }
        }
        fmt::print(ident);
        return;
    }
    //block
    if (auto *stmt = exp->as<Block *>()) {
        fmt::print("Block ");
        // blockitems ==> print if not null
        for (auto &blockitem : stmt->BlockItems) {
            print_expr(blockitem, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // block item
    if (auto *stmt = exp->as<BlockItem *>()) {
        fmt::print("BlockItem ");
        // decl stmt ==> print if not null
        if (stmt->Decl != nullptr) {
            print_expr(stmt->Decl, ident + "└─ ", ident);
        }
        if (stmt->Stmt != nullptr) {
            print_expr(stmt->Stmt, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    return;
}