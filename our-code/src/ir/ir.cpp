//
// Created by jiangyi on 2024/5/30.
//
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fmt/core.h>
#include "ir/ir.h"
#include "ast/ast.h"
int registerNumber = 1;
int if_scope = 0;
int while_scope = -1;
int short_circuit_scope = 0;
std::vector<std::string> global_var_init;
std::unordered_map<std::string, std::vector<int> > all_arrays;
void re_format(std::string &s){
    if(s[0]=='%'){
        s += ": i32";
        if(s[1]>='a'&&s[1]<='z'||s[1]>='A'&&s[1]<='Z'){
            s += "*";
        }
    }
}

std::string translate_params(std::vector<NodePtr> &arglist){
    std::string result, singleParam;
    bool debug = false;
    if(debug) fmt::print("translate_params\n");
    for(size_t i = 0; i < arglist.size(); i++){
        if(auto* param = arglist[i]->as<FuncFParam*>()){
            singleParam = "#" + param->name + ": i32";
            if(param->isArray){
                singleParam += "*";
            }
            result += singleParam;
            if(i != arglist.size() - 1){
                result += ", ";
            }
        }
    }
    return result;
}

std::string translate_exp(NodePtr node){
    bool debug = false;
    std::string result = "";
    if(debug) fmt::print("translate_exp\n");
    if (auto *int_exp = node->as<intExp*>()){
        result = std::to_string(int_exp->value);
    }
    if(auto* pos_exp = node->as<posExp*>()){
        // a = +b
        // let %4: i32 = load %b: i32*
        // let %5: () = store %4: i32, @a: i32*
        result = translate_exp(pos_exp->lhs);
    }
    if(auto* neg_exp = node->as<negExp*>()){
        // a = -b
        // let %4: i32 = load %b: i32*
        // let %5: i32 = sub 0, %4: i32
        // let %6: () = store %5: i32, @a: i32*
        result = translate_exp(neg_exp->lhs);
        re_format(result);
        result = "let %" + std::to_string(registerNumber++) + ": i32 = sub 0, " + result;
    }
    if(auto *not_exp = node->as<notExp*>()){
        // a = !b
        // let %4: i32 = load %b: i32*
        // let %5: i32 = eq 0, %4: i32
        // let %6: () = store %5: i32, @a: i32*
        result = translate_exp(not_exp->lhs);
        re_format(result);
        result = "let %" + std::to_string(registerNumber++) + ": i32 = eq 0, " + result;
    }
    return result;
}

std::string translate_LVal(NodePtr node){
    bool debug = false;
    std::string result = "";
    if(auto* lval = node->as<LVal*>()){
        if(debug) fmt::print("translate_LVal\n");
        if(lval->isArray == false){
            // not array
            result = lval->name;
        }else{
            // array
            // find name in all_arrays
            if(all_arrays.find(lval->name) != all_arrays.end()){
                // get array size per dimension
                std::vector<int> dimensions = all_arrays[lval->name];
                result = "let %" + std::to_string(registerNumber++);
                // get array index for lval->position
                // todo
            }else{
                fmt::print("Error: array {} not found in all_arrays\n",lval->name);
            }
        }
    }
    return result;

}

void translate_Stmt(NodePtr node){
    bool debug = false;
    if(auto* return_stmt = node->as<ReturnStmt*>()){
        if(return_stmt->result == nullptr){
            // return ;
            if(debug) fmt::print("[translate_Stmt]:return ;\n");
            ircode.emplace_back("jmp label %exit");
        }else{
            // return exp;
            if(debug) fmt::print("[translate_Stmt]:return exp\n");
            std::string return_result = translate_exp(return_stmt->result);
            re_format(return_result);
            //  let %22: () = store 0, %ret_val.addr: i32*
            //  jmp label %exit
            ircode.emplace_back("let %" + std::to_string(registerNumber++) + ": () = store " + return_result + ", %ret_val.addr: i32*");
            ircode.emplace_back("jmp label %exit");
        }
    } else if(auto* assign_stmt = node->as<AssignStmt*>()){
        std::string lval_name = translate_LVal(assign_stmt->LVal);
        std::string exp_result = translate_exp(assign_stmt->Exp);
        re_format(exp_result);
        fmt::print("[translate_Stmt]:assign {}\n",lval_name);
        // let %4: i32* = offset i32, %a: i32*, [2 < 10]
        // let %5: () = store 4, %4: i32*

    }
}

void translate_BlockItem(NodePtr node){
    bool debug = false;
    if(auto* block_item = node->as<BlockItem*>()){
        if(block_item->Decl != nullptr){
            auto* temp = block_item->Decl->as<Decl*>();
            auto* var_decl = temp->VarDecl->as<VarDecl*>();
            if(debug) fmt::print("[translate_BlockItem]:varDecl\n");
            translate_VarDecl(var_decl,false);
        }else if(block_item->Stmt != nullptr){
            if(debug) fmt::print("[translate_BlockItem]:Stmt\n");
            translate_Stmt(block_item->Stmt);
        }
    }
}

void translate_Block(NodePtr node){
    bool debug = false;
    if(auto* block = node->as<Block*>()){
        if(debug) fmt::print("[translate_Block]:block->BlockItem.size() = {}\n",block->BlockItems.size());
        for(auto* blockitem : block->BlockItems){
            translate_BlockItem(blockitem);
        }
    }
}

void translate_VarDef(NodePtr node, bool isGlobalVar){
    bool debug = false;
    if(auto* var_def = node->as<VarDef*>()){
        if(isGlobalVar){
            if(debug) fmt::print("[translate_VarDef]: global var {}\n",var_def->name);
            if(var_def->dimensions.size() == 0) {
                // not array
                ircode.emplace_back("@" + var_def->name + " region i32, 1\n");
                if(var_def->initialValue != nullptr) {
                    std::string value = translate_exp(var_def->initialValue);
                    re_format(value);
                    std::string init = "let %" + std::to_string(registerNumber++) + ":() = store " + value + ",@" +
                                       var_def->name + ": i32*";
                    global_var_init.emplace_back(init);
                }
            } else{
                // array
                // TODO function parameter array
                int arraySize = 1;
                for(auto dim : var_def->dimensions){
                    arraySize *= dim;
                }
                all_arrays[var_def->name] = var_def->dimensions;
                ircode.emplace_back("@" + var_def->name + " region i32, " + std::to_string(arraySize) + "\n");
            }
        }else{
            if(debug) fmt::print("[translate_VarDef]: local var {}\n",var_def->name);
            if(var_def->dimensions.size() == 0){
                // not array
                ircode.emplace_back("let %" + var_def->name + ": i32* = alloca i32, 1\n");
                if(var_def->initialValue != nullptr){
                    std::string value = translate_exp(var_def->initialValue);
                    re_format(value);
                    std::string init = "let %" + std::to_string(registerNumber++) + ":() = store " + value + ",%" + var_def->name + ": i32*";
                    ircode.emplace_back(init);
                }
            }else{
                // array
                int arraySize = 1;
                for(auto dim : var_def->dimensions){
                    arraySize *= dim;
                }
                all_arrays[var_def->name] = var_def->dimensions;
                ircode.emplace_back("let %" + var_def->name + ": i32* = alloca i32, " + std::to_string(arraySize) + "\n");
            }
        }
    }
}

void translate_VarDecl(NodePtr node, bool isGlobalVar){
    bool debug = true;
    if(debug) {
        fmt::print("translate_VarDecl ");
        if(isGlobalVar) fmt::print("global\n");
        else fmt::print("local\n");
    }
    if(auto *var_decl = node->as<VarDecl*>()){
        if(debug) fmt::print("[translate_VarDecl]:VarDecl->VarDefs.size = {}\n",var_decl->VarDefs.size());
        for(auto* vardef : var_decl->VarDefs){
            translate_VarDef(vardef,isGlobalVar);
        }
    }else{
        fmt::print("node is not a VarDecl\n");
    }
}

void translate_FuncDef(NodePtr node){
    bool debug = false;
    std::string func_def_code;
    if(auto *func_def = node->as<FuncDef*>()){
        if(debug) fmt::print("translate_FuncDef\n");
        // head
        func_def_code = "fn @" + func_def->name + "(";
        std::string func_params_code = translate_params(func_def->argList);
        func_def_code += func_params_code + ") -> ";
        if(func_def->ReturnType == FuncDef::Type::INT){
            func_def_code += "i32";
        }else{
            func_def_code += "()";
        }
        if(debug) fmt::print("[translate_FuncDef]: head finish\n");
        // body
        func_def_code += "{";
        ircode.emplace_back(func_def_code);
        ircode.emplace_back("%entry:");
        // func param init
        for(auto* param : func_def->argList){
            //  let %a.addr: i32* = alloca i32, 1
            //  let %3: () = store #a: i32, %a.addr: i32*
            if(auto* fparam = param->as<FuncFParam*>()){
                ircode.emplace_back("let %" + fparam->name + ".addr: i32* = alloca i32, 1");
                ircode.emplace_back("let %" + std::to_string(registerNumber++) + ": () = store #" + fparam->name + ": i32, %" + fparam->name + ".addr: i32*");
            }
        }
        if(debug) fmt::print("[translate_FuncDef]: func param init finish\n");
        // return value init
        if(func_def->ReturnType == FuncDef::Type::INT){
            ircode.emplace_back("let %ret_val.addr: i32* = alloca i32, 1");
            if(debug) fmt::print("[translate_FuncDef]: return value init finish\n");
        }
        // add global init to main()
        if(func_def->name == "main"){
            for(auto init : global_var_init){
                ircode.emplace_back(init);
            }
            if(debug) fmt::print("[translate_FuncDef]: global var init finish\n");
        }
        // block
        translate_Block(func_def->block);
        if(debug) fmt::print("[translate_FuncDef]: block finish\n");
        // %exit:
        // let %25: i32 = load %ret_val.addr: i32*
        // ret %25: i32
        ircode.emplace_back("%exit:");
        if(func_def->ReturnType == FuncDef::Type::INT){
            ircode.emplace_back("let %" + std::to_string(registerNumber++) + ": i32 = load %ret_val.addr: i32*");
            ircode.emplace_back("ret %" + std::to_string(registerNumber - 1) + ": i32");
        }else{
            ircode.emplace_back("ret ()");
        }
        ircode.emplace_back("}");
    }
}

void translate_CompUnit(NodePtr node){
    bool debug = false;
    if(debug) fmt::print("translate_CompUnit\n");
    if(auto* decl = node->as<Decl*>()){
        if(debug) fmt::print("CompUnit->Decl\n");
        if(auto* var_decl = decl->VarDecl->as<VarDecl*>()){
            translate_VarDecl(var_decl,true);
        }
    }else if(auto* func_def = node->as<FuncDef*>()) {
        if(debug) fmt::print("CompUnit->FuncDef\n");
        translate_FuncDef(func_def);
    }
}

void generate_ircode(NodePtr root){
    ircode.emplace_back("fn @putint(#x: i32) -> ();\n");
    ircode.emplace_back("fn @putch(#x: i32) -> ();\n");
    ircode.emplace_back("fn @putarray(#n: i32, #arr: i32*) -> ();\n");
    ircode.emplace_back("fn @getint() -> i32;\n");
    ircode.emplace_back("fn @getch() -> i32;\n");
    ircode.emplace_back("fn @getarray(#n: i32, #arr: i32*) -> ();\n");
    bool debug = false;
    if(debug) fmt::print("generate_ircode\n");
    if(auto* comp_unit = root->as<CompUnit*>()){
        for(size_t i = 0; i < comp_unit->all.size(); i++){
            translate_CompUnit(comp_unit->all[i]);
        }
    }else{
        fmt::print("Error: root is not a CompUnit\n");
    }
}