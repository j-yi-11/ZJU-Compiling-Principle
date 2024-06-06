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
            for(auto *i : lval->position){
                printAST(i,prefix,ident);
            }
        }
        fmt::print("\n");
        return;
    }
    if (auto *ifstmt = exp->as<IfStmt *>()) {
        fmt::print(prefix);
        fmt::print("IfStmt\n");
        fmt::print("condition:\n");
        printAST(ifstmt->condition, prefix+" ", ident);
        fmt::print("then:\n");
        printAST(ifstmt->then, prefix+" ", ident);

        if(ifstmt->matched==true) {
            fmt::print("els matched:\n");
            printAST(ifstmt->els, prefix + " ", ident);
        }
        return;
    }
    // while statement
    if (auto *whilestmt = exp->as<WhileStmt *>()) {
        fmt::print(prefix);
        fmt::print("WhileStmt\n");
        fmt::print(" condition:");
        printAST(whilestmt->condition, prefix+" ", ident);
        fmt::print(" body(then):");
        printAST(whilestmt->then, prefix+" ", ident);
        return;
    }
    if (auto *returnstmt = exp->as<ReturnStmt *>()) {
        fmt::print(prefix);
        fmt::print("ReturnStmt\n");
        printAST(returnstmt->result, prefix+" ", ident);
        return;
    }
    if (auto *blockstmt = exp->as<BlockStmt *>()) {
//        fmt::print(prefix);
        fmt::print("BlockStmt\n");
        printAST(blockstmt->Block, prefix, ident);
        return;
    }
    // exp statement
    if (auto *stmt = exp->as<ExpStmt *>()) {
        fmt::print("ExpStmt\n");
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
    if (auto *decl = exp->as<Decl *>()) {
        fmt::print("Decl\n");
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
//        fmt::print("Block\n");
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