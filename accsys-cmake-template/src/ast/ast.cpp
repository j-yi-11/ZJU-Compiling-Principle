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

void print_expr(NodePtr exp, std::string prefix, std::string ident) {
    assert(exp != nullptr);
    fmt::print(prefix);
    // print the expression type according to ast.h defines
    // exp
    if (auto *expr = exp->as<Expr *>()) {
        fmt::print("Exp\n");
        print_expr(expr->LgExp, ident + "└─ ", ident + "   ");
    }
    // lg_exp
    if (auto *lg_exp = exp->as<LgExp *>()) {
        fmt::print("LgExp \"{}\"\n", op_str(lg_exp->optype));
        print_expr(lg_exp->lhs, ident + "├─ ", ident + "│  ");
        print_expr(lg_exp->rhs, ident + "└─ ", ident + "   ");
    }
    // comp_exp
    if (auto *comp_exp = exp->as<CompExp *>()) {
        fmt::print("CompExp \"{}\"\n", op_str(comp_exp->optype));
        print_expr(comp_exp->lhs, ident + "├─ ", ident + "│  ");
        print_expr(comp_exp->rhs, ident + "└─ ", ident + "   ");
    }
    // add_exp
    if (auto *add_exp = exp->as<AddExp *>()) {
        fmt::print("AddExp \"{}\"\n", op_str(add_exp->optype));
        print_expr(add_exp->addExp, ident + "├─ ", ident + "│  ");
        print_expr(add_exp->mulExp, ident + "└─ ", ident + "   ");
    }
    // mul_exp
    if (auto *mul_exp = exp->as<MulExp *>()) {
        fmt::print("MulExp \"{}\"\n", op_str(mul_exp->optype));
        print_expr(mul_exp->mulExp, ident + "├─ ", ident + "│  ");
        print_expr(mul_exp->unaryExp, ident + "└─ ", ident + "   ");
    }
    // primary_exp
    if (auto *primary_exp = exp->as<PrimaryExpr *>()) {
        fmt::print("PrimaryExpr\n");
        // exp lval number ==> select the not null one to print
        if (primary_exp->Exp != nullptr) {
            print_expr(primary_exp->Exp, ident + "└─ ", ident + "   ");
        } else if (primary_exp->LVal != nullptr) {
            print_expr(primary_exp->LVal, ident + "└─ ", ident + "   ");
        } else if (primary_exp->Number != nullptr) {
            print_expr(primary_exp->Number, ident + "└─ ", ident + "   ");
        }
    }
    // unary_exp
    if (auto *unary_exp = exp->as<UnaryExpr *>()) {
        fmt::print("UnaryExpr \"{}\"\n", op_str(unary_exp->opType));
        // primary_exp unary_exp params ==> select those not null to print
        if (unary_exp->primaryExp != nullptr) {
            print_expr(unary_exp->primaryExp, ident + "└─ ", ident + "   ");
        } else if (unary_exp->unaryExp != nullptr) {
            print_expr(unary_exp->unaryExp, ident + "└─ ", ident + "   ");
        } else {
            // params and name(for func call)
            for (auto &param : unary_exp->params) {
                print_expr(param, ident + "└─ ", ident + "   ");
            }
            fmt::print("{}\n", unary_exp->name);
        }
    }
    // integer literal
    if (auto *lit = exp->as<IntegerLiteral *>()) {
        fmt::print("Int {}\n", lit->value);
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
    }
    // if statement
    if (auto *stmt = exp->as<IfStmt *>()) {
        fmt::print("IfStmt\n");
        // condition then else ==> print those not null
        print_expr(stmt->condition, ident + "├─ ", ident + "│  ");
        print_expr(stmt->then, ident + "├─ ", ident + "│  ");
        if (stmt->els != nullptr) {
            print_expr(stmt->els, ident + "└─ ", ident + "   ");
        }
    }
    // while statement
    if (auto *stmt = exp->as<WhileStmt *>()) {
        fmt::print("WhileStmt\n");
        // condition then ==> print those not null
        print_expr(stmt->condition, ident + "├─ ", ident + "│  ");
        print_expr(stmt->then, ident + "└─ ", ident + "   ");
    }
    // break statement
    if (auto *stmt = exp->as<BreakStmt *>()) {
        fmt::print("BreakStmt\n");
    }
    // continue statement
    if (auto *stmt = exp->as<ContinueStmt *>()) {
        fmt::print("ContinueStmt\n");
    }
    // return statement
    if (auto *stmt = exp->as<ReturnStmt *>()) {
        fmt::print("ReturnStmt\n");
        // result ==> print if not null
        if (stmt->result != nullptr) {
            print_expr(stmt->result, ident + "└─ ", ident + "   ");
        }
    }
    // block statement
    if (auto *stmt = exp->as<BlockStmt *>()) {
        fmt::print("BlockStmt\n");
        // block ==> print if not null
        if (stmt->Block != nullptr) {
            print_expr(stmt->Block, ident + "└─ ", ident + "   ");
        }
    }
    // exp statement
    if (auto *stmt = exp->as<ExpStmt *>()) {
        fmt::print("ExpStmt\n");
        // exp ==> print if not null
        if (stmt->Exp != nullptr) {
            print_expr(stmt->Exp, ident + "└─ ", ident + "   ");
        }
    }
    // assign statement
    if (auto *stmt = exp->as<AssignStmt *>()) {
        fmt::print("AssignStmt\n");
        // exp lval ==> print if not null
        if (stmt->Exp != nullptr) {
            print_expr(stmt->Exp, ident + "├─ ", ident + "│  ");
        }
        if (stmt->LVal != nullptr) {
            print_expr(stmt->LVal, ident + "└─ ", ident + "   ");
        }
    }
    // initial value
    if (auto *stmt = exp->as<InitVal *>()) {
        fmt::print("InitialValue\n");
        // exp ==> print if not null
        if (stmt->Exp != nullptr) {
            print_expr(stmt->Exp, ident + "└─ ", ident + "   ");
        }
    }
    // decl
    if (auto *stmt = exp->as<Decl *>()) {
        fmt::print("Decl\n");
        // vardecl ==> print if not null
        if (stmt->VarDecl != nullptr) {
            print_expr(stmt->VarDecl, ident + "└─ ", ident + "   ");
        }
    }
    // vardecl
    if (auto *stmt = exp->as<VarDecl *>()) {
        fmt::print("VarDecl\n");
        // vardefs ==> print if not null
        for (auto &vardef : stmt->VarDefs) {
            print_expr(vardef, ident + "└─ ", ident + "   ");
        }
    }
    // var def
    if (auto *stmt = exp->as<VarDef *>()) {
        fmt::print("VarDef\n");
        // name initialValue dimension ==> print if not null
        fmt::print("{}\n", stmt->name);
        if (stmt->initialValue != nullptr) {
            print_expr(stmt->initialValue, ident + "└─ ", ident + "   ");
        }
        for (auto &dim : stmt->dimensions) {
            fmt::print("{}\n", dim);
        }
    }
    // func def
    if (auto *stmt = exp->as<FuncDef *>()) {
        fmt::print("FuncDef\n");
        // name arglist block returntype ==> print if not null
        fmt::print("{}\n", stmt->name);
        for (auto &arg : stmt->argList) {
            print_expr(arg, ident + "└─ ", ident + "   ");
        }
        if (stmt->block != nullptr) {
            print_expr(stmt->block, ident + "└─ ", ident + "   ");
        }
        // return type
        if (stmt->ReturnType->type == FuncType::Type::INT) {
            fmt::print("int\n");
        } else {
            fmt::print("void\n");
        }
    }
    // Func FParam
    if (auto *stmt = exp->as<FuncFParam *>()) {
        fmt::print("FuncFParam\n");
        // name isArray dimension ==> print if not null
        fmt::print("{}\n", stmt->name);
        if (stmt->isArray) {
            fmt::print("array\n");
            for (auto &dim : stmt->dimensions) {
                fmt::print("{}\n", dim);
            }
        }
    }
    //block
    if (auto *stmt = exp->as<Block *>()) {
        fmt::print("Block\n");
        // blockitems ==> print if not null
        for (auto &blockitem : stmt->BlockItems) {
            print_expr(blockitem, ident + "└─ ", ident + "   ");
        }
    }
    // block item
    if (auto *stmt = exp->as<BlockItem *>()) {
        fmt::print("BlockItem\n");
        // decl stmt ==> print if not null
        if (stmt->Decl != nullptr) {
            print_expr(stmt->Decl, ident + "└─ ", ident + "   ");
        }
        if (stmt->Stmt != nullptr) {
            print_expr(stmt->Stmt, ident + "└─ ", ident + "   ");
        }
    }
//    if (auto *bin_op = exp->as<TreeBinaryExpr *>()) {
//        fmt::print("BinOp \"{}\"\n", op_str(bin_op->op));
//        print_expr(bin_op->lhs, ident + "├─ ", ident + "│  ");
//        print_expr(bin_op->rhs, ident + "└─ ", ident + "   ");
//    }
//    if (auto *un_op = exp->as<TreeUnaryExpr *>()) {
//        fmt::print("UnOp \"{}\"\n", op_str(un_op->op));
//        print_expr(un_op->operand, ident + "└─ ", ident + "   ");
//    }

}