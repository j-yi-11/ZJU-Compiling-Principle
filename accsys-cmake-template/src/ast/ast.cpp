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


void printAST(NodePtr exp, std::string prefix, std::string ident) {
    bool debug = false;
    if(debug) fmt::print("printAST called\n");
    if(exp==nullptr) return;
    // print the expression type according to ast.h defines
    // comp unit
    if (auto *comp_unit = exp->as<CompUnit *>()) {
        fmt::print(prefix);
        fmt::print("CompUnit\n");
        for (size_t i = 0; i < comp_unit->all.size(); i++) {
            printAST(comp_unit->all[i], " ", "");
        }
        return;
    }
//    if (auto *expr = exp->as<Expr *>()) {
////        fmt::print(prefix);
//        printAST(expr->LgExp, prefix, ident);
////        fmt::print(ident);
//        return;
//    }
    // lg_exp
//    if (auto *lg_exp = exp->as<LgExp *>()) {
//        fmt::print(prefix);
//        fmt::print("LgExp \"{}\"\n", op_str(lg_exp->optype));
//        if(lg_exp->rhs == nullptr){
//            printAST(lg_exp->lhs, prefix, ident);
//        }else{
//            printAST(lg_exp->lhs, prefix+" ", ident);
//            printAST(lg_exp->rhs, prefix+" ", ident);
//        }
//        return;
//    }
    // comp_exp
//    if (auto *comp_exp = exp->as<CompExp *>()) {
//        fmt::print(prefix);
//        fmt::print("CompExp \"{}\"\n", op_str(comp_exp->optype));
//        if(comp_exp->rhs == nullptr){
//            printAST(comp_exp->lhs, prefix, ident);
//        }else{
//            printAST(comp_exp->lhs, prefix+" ", ident);
//            printAST(comp_exp->rhs, prefix+" ", ident);
//        }
//        return;
//    }
    // add_exp
//    if (auto *add_exp = exp->as<AddExp *>()) {
//        fmt::print(prefix);
//        fmt::print("AddExp \"{}\"\n", op_str(add_exp->optype));
//        printAST(add_exp->addExp, prefix+" ", ident);
//        printAST(add_exp->mulExp, prefix+" ", ident);
////        fmt::print(ident);
//        return;
//    }
    // mul_exp
//    if (auto *mul_exp = exp->as<MulExp *>()) {
//        fmt::print(prefix);
//        fmt::print("MulExp \"{}\"\n", op_str(mul_exp->optype));
//        printAST(mul_exp->mulExp, prefix+" ", ident);
//        printAST(mul_exp->unaryExp, prefix+" ", ident);
////        fmt::print(ident);
//        return;
//    }
    // primary_exp
//    if (auto *primary_exp = exp->as<PrimaryExpr *>()) {
//        fmt::print(prefix);
//        fmt::print("PrimaryExpr\n");
//        if (primary_exp->Exp != nullptr) {
//            printAST(primary_exp->Exp, prefix+" ", ident);
//        } else if (primary_exp->LVal != nullptr) {
//            printAST(primary_exp->LVal, prefix+" ", ident);
//        } else if (primary_exp->Number != nullptr) {
//            printAST(primary_exp->Number, prefix+" ", ident);
//        }
////        fmt::print(ident);
//        return;
//    }
//    if (auto *unary_exp = exp->as<UnaryExpr *>()) {
//        fmt::print(prefix);
//        fmt::print("UnaryExpr \"{}\"\n", op_str(unary_exp->opType));
//        if (unary_exp->primaryExp != nullptr) {
//            printAST(unary_exp->primaryExp, prefix+" ", ident);
//        } else if (unary_exp->unaryExp != nullptr) {
//            printAST(unary_exp->unaryExp, prefix+" ", ident);
//        } else {
//            fmt::print(prefix+" "+"{}\n", unary_exp->name);
//            for (auto &param : unary_exp->params) {
//                printAST(param, prefix+" ", ident);
//            }
//        }
////        fmt::print(ident);
//        return;
//    }
    // integer literal
//    if (auto *lit = exp->as<IntegerLiteral *>()) {
//        fmt::print(prefix);
//        fmt::print("Int {}\n", lit->value);
////        fmt::print(ident);
//        return;
//    }
    if (auto *add_exp = exp->as<addExp*>()){
        fmt::print(prefix);
        fmt::print("exp +\n");
        printAST(add_exp->lhs,prefix,ident);
        printAST(add_exp->rhs,prefix,ident);
    }
    if (auto *sub_exp = exp->as<subExp*>()){
        fmt::print(prefix);
        fmt::print("exp -\n");
        printAST(sub_exp->lhs,prefix,ident);
        printAST(sub_exp->rhs,prefix,ident);
    }
    if (auto *mul_exp = exp->as<mulExp*>()){
        fmt::print(prefix);
        fmt::print("exp *\n");
        printAST(mul_exp->lhs,prefix,ident);
        printAST(mul_exp->rhs,prefix,ident);
    }
    if (auto *div_exp = exp->as<divExp*>()){
        fmt::print(prefix);
        fmt::print("exp /\n");
        printAST(div_exp->lhs,prefix,ident);
        printAST(div_exp->rhs,prefix,ident);
    }
    if (auto *mod_exp = exp->as<modExp*>()){
        fmt::print(prefix);
        fmt::print("exp %\n");
        printAST(mod_exp->lhs,prefix,ident);
        printAST(mod_exp->rhs,prefix,ident);
    }
    if (auto *and_exp = exp->as<andExp*>()){
        fmt::print(prefix);
        fmt::print("exp &&\n");
        printAST(and_exp->lhs,prefix,ident);
        printAST(and_exp->rhs,prefix,ident);
    }
    if (auto *or_exp = exp->as<orExp*>()){
        fmt::print(prefix);
        fmt::print("exp ||\n");
        printAST(or_exp->lhs,prefix,ident);
        printAST(or_exp->rhs,prefix,ident);
    }
    if (auto *less_exp = exp->as<lessExp*>()){
        fmt::print(prefix);
        fmt::print("exp <\n");
        printAST(less_exp->lhs,prefix,ident);
        printAST(less_exp->rhs,prefix,ident);
    }
    if (auto *great_exp = exp->as<greatExp*>()){
        fmt::print(prefix);
        fmt::print("exp >\n");
        printAST(great_exp->lhs,prefix,ident);
        printAST(great_exp->rhs,prefix,ident);
    }
    if (auto *leq_exp = exp->as<leqExp*>()){
        fmt::print(prefix);
        fmt::print("exp <=\n");
        printAST(leq_exp->lhs,prefix,ident);
        printAST(leq_exp->rhs,prefix,ident);
    }
    if (auto *geq_exp = exp->as<geqExp*>()){
        fmt::print(prefix);
        fmt::print("exp >=\n");
        printAST(geq_exp->lhs,prefix,ident);
        printAST(geq_exp->rhs,prefix,ident);
    }
    if (auto *eq_exp = exp->as<eqExp*>()){
        fmt::print(prefix);
        fmt::print("exp ==\n");
        printAST(eq_exp->lhs,prefix,ident);
        printAST(eq_exp->rhs,prefix,ident);
    }
    if (auto *neq_exp = exp->as<neqExp*>()){
        fmt::print(prefix);
        fmt::print("exp !=\n");
        printAST(neq_exp->lhs,prefix,ident);
        printAST(neq_exp->rhs,prefix,ident);
    }
    if (auto *not_exp = exp->as<notExp*>()){
        fmt::print(prefix);
        fmt::print("exp !\n");
        printAST(not_exp->lhs,prefix,ident);
    }
    if (auto *neg_exp = exp->as<negExp*>()){
        fmt::print(prefix);
        fmt::print("exp -(neg)\n");
        printAST(neg_exp->lhs,prefix,ident);
    }
    if (auto *pos_exp = exp->as<posExp*>()){
        fmt::print(prefix);
        fmt::print("exp +(pos)\n");
        printAST(pos_exp->lhs,prefix,ident);
    }
    if (auto *int_exp = exp->as<intExp*>()){
        fmt::print(prefix);
        fmt::print("exp int const with value {}\n",int_exp->value);
    }
    if(auto *funcall_exp = exp->as<funcallExp*>()){
        fmt::print(prefix);
        fmt::print("exp function call with name {}\n",funcall_exp->name);
        for(auto &para:funcall_exp->params){
            printAST(para,prefix+" ",ident);
        }
    }
    if (auto *lval = exp->as<LVal *>()) {
        fmt::print(prefix);
        fmt::print("LVal: {}", lval->name);
        // array
        if (lval->isArray) {
            // print array out
            fmt::print(" with dimension ");
            for(auto &i : lval->position){
                fmt::print("[{}]", i);
            }
        }
        fmt::print("\n");
        return;
    }
    if (auto *ifstmt = exp->as<IfStmt *>()) {
        fmt::print(prefix);
        fmt::print("IfStmt\n");
        printAST(ifstmt->condition, prefix+" ", ident);
        printAST(ifstmt->then, prefix+" ", ident);
        printAST(ifstmt->els, prefix+" ", ident);
        return;
    }
    // while statement
    if (auto *whilestmt = exp->as<WhileStmt *>()) {
        fmt::print(prefix);
        fmt::print("WhileStmt\n");
        printAST(whilestmt->condition, prefix+" ", ident);
        printAST(whilestmt->then, prefix+" ", ident);
        return;
    }
//    if (auto *breakstmt = exp->as<BreakStmt *>()) {
//        fmt::print(prefix);
//        fmt::print("BreakStmt\n");
//        return;
//    }
//    if (auto *continuestmt = exp->as<ContinueStmt *>()) {
//        fmt::print(prefix);
//        fmt::print("ContinueStmt\n");
//        return;
//    }
    if (auto *returnstmt = exp->as<ReturnStmt *>()) {
        fmt::print(prefix);
        fmt::print("ReturnStmt\n");
        printAST(returnstmt->result, prefix+" ", ident);
        return;
    }
    if (auto *blockstmt = exp->as<BlockStmt *>()) {
        fmt::print(prefix);
        printAST(blockstmt->Block, prefix, ident);
        return;
    }
    // exp statement
    if (auto *stmt = exp->as<ExpStmt *>()) {
//        fmt::print(prefix);
//        fmt::print("ExpStmt\n");
        printAST(stmt->Exp, prefix, ident);
        return;
    }
    // assign statement
    if (auto *assignstmt = exp->as<AssignStmt *>()) {
        fmt::print(prefix);
        fmt::print("AssignStmt\n");
        printAST(assignstmt->LVal, prefix+" ", ident);
        printAST(assignstmt->Exp, prefix+" ", ident);
        return;
    }
//    if (auto *initialvalue = exp->as<InitVal *>()) {
//        fmt::print(prefix);
//        fmt::print("InitialValue:\n");
//        printAST(initialvalue->Exp, prefix+" ", ident);
//        return;
//    }
    if (auto *decl = exp->as<Decl *>()) {
//        fmt::print(prefix);
        printAST(decl->VarDecl, prefix, ident);
        return;
    }
    if (auto *vardecl = exp->as<VarDecl *>()) {
        fmt::print(prefix);
        fmt::print("VarDecl:\n");
        for (auto &vardef : vardecl->VarDefs) {
            printAST(vardef, prefix+" ", ident);
        }
        return;
    }
    if (auto *vardef = exp->as<VarDef *>()) {
        fmt::print(prefix);
//        fmt::print("VarDef");
        fmt::print("{}\n", vardef->name);
        printAST(vardef->initialValue, prefix+" ", ident);
        for (auto &dim : vardef->dimensions) {
            fmt::print("[{}]", dim);
        }
        return;
    }
    if (auto *funcdef = exp->as<FuncDef *>()) {
        fmt::print(prefix);
        fmt::print("FuncDef: ");
        // return type
        if (funcdef->ReturnType == FuncDef::Type::INT) {
            fmt::print("int ");
        } else {
            fmt::print("void ");
        }
        fmt::print("{}\n", funcdef->name);
        fmt::print(prefix+" "+"FuncFParam: ");
        if (funcdef->argList.size() > 0) {
            for (auto &arg : funcdef->argList) {
                // FuncFParam
                if(auto *funcfparam = arg->as<FuncFParam*>()){
                    fmt::print(" {} 'int'", funcfparam->name);
                    if (funcfparam->isArray) {
                        for (auto &dim : funcfparam->dimensions) {
                            fmt::print("[{}]", dim);
                        }
                    }
                }
            }
        } else {
            fmt::print("void");
        }
        fmt::print("\n");
        printAST(funcdef->block, prefix+" ", ident);
        return;
    }
    if (auto *funcfparam = exp->as<FuncFParam *>()) {
        fmt::print(prefix);
//        fmt::print("FuncFParam ");
        fmt::print(" {} int", funcfparam->name);
        if (funcfparam->isArray) {
//            fmt::print("dimension ");
            for (auto &dim : funcfparam->dimensions) {
                fmt::print("[{}]", dim);
            }
        }
        return;
    }
    if (auto *block = exp->as<Block *>()) {
        fmt::print(prefix);
        fmt::print("Block\n");
        for (auto &blockitem : block->BlockItems) {
            printAST(blockitem, prefix, ident);
        }
        return;
    }
    if (auto *blockitem = exp->as<BlockItem *>()) {
//        fmt::print(prefix);
        printAST(blockitem->Decl, prefix+" ", ident);
        printAST(blockitem->Stmt, prefix+" ", ident);
        return;
    }
    return;
}