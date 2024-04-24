#include "ast/ast.h"
#include <cstdlib>
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
    for (size_t i = 0; i < vec.size(); i++) {
        bool is_last = i == vec.size() - 1;
        printAST(vec[i], prefix + (is_last ? "\n└─ " : "\n├─ "), ident + " ");
    }
}

void printAST(NodePtr exp, std::string prefix, std::string ident) {
    bool debug = false;
    if(debug)
        fmt::print("printAST called\n");
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
        //print_vector_data(comp_unit->all, prefix, ident);
        for (size_t i = 0; i < comp_unit->all.size(); i++) {
            bool is_last = i == comp_unit->all.size() - 1;
            printAST(comp_unit->all[i], prefix + (is_last ? "\n└─ " : "\n├─ "), ident + " ");
        }
        fmt::print(ident);
        return;
    }
    // exp
    if (auto *expr = exp->as<Expr *>()) {
        //fmt::print("Expr\n");
        //printAST(expr->LgExp, ident + "└─ ", ident + "   ");
        printAST(expr->LgExp, prefix, ident);
        fmt::print(ident);
        return;
    }
    // lg_exp
    if (auto *lg_exp = exp->as<LgExp *>()) {
        fmt::print("LgExp \"{}\"\n", op_str(lg_exp->optype));
        printAST(lg_exp->lhs, ident + "├─ ", ident + "   ");
        printAST(lg_exp->rhs, ident + "└─ ", ident + "   ");
        fmt::print(ident);
        return;
    }
    // comp_exp
    if (auto *comp_exp = exp->as<CompExp *>()) {
        fmt::print("CompExp \"{}\"\n", op_str(comp_exp->optype));
        printAST(comp_exp->lhs, ident + "├─ ", ident);
        printAST(comp_exp->rhs, ident + "└─ ", ident);
        fmt::print(ident);
        return;
    }
    // add_exp
    if (auto *add_exp = exp->as<AddExp *>()) {
        fmt::print("AddExp \"{}\"\n", op_str(add_exp->optype));
        printAST(add_exp->addExp, ident + "├─ ", ident);
        printAST(add_exp->mulExp, ident + "└─ ", ident);
        fmt::print(ident);
        return;
    }
    // mul_exp
    if (auto *mul_exp = exp->as<MulExp *>()) {
        fmt::print("MulExp \"{}\"\n", op_str(mul_exp->optype));
        printAST(mul_exp->mulExp, ident + "├─ ", ident);
        printAST(mul_exp->unaryExp, ident + "└─ ", ident);
        fmt::print(ident);
        return;
    }
    // primary_exp
    if (auto *primary_exp = exp->as<PrimaryExpr *>()) {
        fmt::print("PrimaryExpr\n");
        // exp lval number ==> select the not null one to print
        if (primary_exp->Exp != nullptr) {
            printAST(primary_exp->Exp, ident + "└─ ", ident);
        } else if (primary_exp->LVal != nullptr) {
            printAST(primary_exp->LVal, ident + "└─ ", ident);
        } else if (primary_exp->Number != nullptr) {
            printAST(primary_exp->Number, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // unary_exp
    if (auto *unary_exp = exp->as<UnaryExpr *>()) {
        fmt::print("UnaryExpr \"{}\"\n", op_str(unary_exp->opType));
        // primary_exp unary_exp params ==> select those not null to print
        if (unary_exp->primaryExp != nullptr) {
            printAST(unary_exp->primaryExp, ident + "└─ ", ident);
        } else if (unary_exp->unaryExp != nullptr) {
            printAST(unary_exp->unaryExp, ident + "└─ ", ident);
        } else {
            // params and name(for func call)
            for (auto &param : unary_exp->params) {
                printAST(param, ident + "└─ ", ident);
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
        printAST(stmt->condition, ident + "├─ ", ident);
        printAST(stmt->then, ident + "├─ ", ident);
        if (stmt->els != nullptr) {
            printAST(stmt->els, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // while statement
    if (auto *stmt = exp->as<WhileStmt *>()) {
        fmt::print("WhileStmt\n");
        // condition then ==> print those not null
        printAST(stmt->condition, ident + "├─ ", ident);
        printAST(stmt->then, ident + "└─ ", ident);
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
            printAST(stmt->result, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // block statement
    if (auto *stmt = exp->as<BlockStmt *>()) {
        fmt::print("BlockStmt\n");
        // block ==> print if not null
        if (stmt->Block != nullptr) {
            printAST(stmt->Block, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // exp statement
    if (auto *stmt = exp->as<ExpStmt *>()) {
        fmt::print("ExpStmt\n");
        // exp ==> print if not null
        if (stmt->Exp != nullptr) {
            printAST(stmt->Exp, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // assign statement
    if (auto *stmt = exp->as<AssignStmt *>()) {
        fmt::print("AssignStmt\n");
        // exp lval ==> print if not null
        if (stmt->Exp != nullptr) {
            printAST(stmt->Exp, ident + "├─ ", ident);
        }
        if (stmt->LVal != nullptr) {
            printAST(stmt->LVal, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // initial value
    if (auto *stmt = exp->as<InitVal *>()) {
        fmt::print("InitialValue\n");
        // exp ==> print if not null
        if (stmt->Exp != nullptr) {
            printAST(stmt->Exp, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // decl
    if (auto *stmt = exp->as<Decl *>()) {
        fmt::print("Decl\n");
        // vardecl ==> print if not null
        if (stmt->VarDecl != nullptr) {
            printAST(stmt->VarDecl, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    if (auto *vardecl = exp->as<VarDecl *>()) {
        fmt::print("VarDecl\n");
        // vardefs ==> print if not null
        for (auto &vardef : vardecl->VarDefs) {
            printAST(vardef, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    // vardef
    if (auto *vardef = exp->as<VarDef *>()) {
        fmt::print("VarDef");
        // name initialValue dimension ==> print if not null
        fmt::print(" {} ", vardef->name);
        if (vardef->initialValue != nullptr) {
            printAST(vardef->initialValue, ident + "└─ ", ident);
        }
        for (auto &dim : vardef->dimensions) {
            fmt::print(" {} ", dim);
        }
        fmt::print(ident);
        return;
    }
    if (auto *funcdef = exp->as<FuncDef *>()) {
        fmt::print("FuncDef ");
        // return type
        if (funcdef->ReturnType == FuncDef::Type::INT) {
            fmt::print("int ");
        } else {
            fmt::print("void ");
        }
        // name arglist block  ==> print if not null
        fmt::print("{}", funcdef->name);
        if (funcdef->argList.size() > 0) {
            fmt::print(" size {}\n", funcdef->argList.size());
            for (auto &arg : funcdef->argList) {
                printAST(arg, ident + "└─ ", ident + "   ");
            }
        } else {
            fmt::print(" void ");
        }
        if (funcdef->block != nullptr) {
            printAST(funcdef->block, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    if (auto *funcfparam = exp->as<FuncFParam *>()) {
        fmt::print("FuncFParam ");
        fmt::print("name is {} ", funcfparam->name);
        if (funcfparam->isArray) {
            fmt::print("dimension ");
            for (auto &dim : funcfparam->dimensions) {
                fmt::print("{} ", dim);
            }
        }
        fmt::print(ident);
        return;
    }
    if (auto *block = exp->as<Block *>()) {
        fmt::print("Block ");
        for (auto &blockitem : block->BlockItems) {
            printAST(blockitem, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    if (auto *blockitem = exp->as<BlockItem *>()) {
        fmt::print("BlockItem ");
        if (blockitem->Decl != nullptr) {
            printAST(blockitem->Decl, ident + "└─ ", ident);
        }
        if (blockitem->Stmt != nullptr) {
            printAST(blockitem->Stmt, ident + "└─ ", ident);
        }
        fmt::print(ident);
        return;
    }
    return;
}