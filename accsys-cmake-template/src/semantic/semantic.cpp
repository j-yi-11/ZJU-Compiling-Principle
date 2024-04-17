#include "ast/ast.h"
#include "semantic/semantic.h"
#include <fmt/core.h>
#include <vector>
#include <string>
funcSymbolTablePtr GenerateFuncSymTable(NodePtr root)
{
    funcSymbolTablePtr tablePtr = new funcSymbolTable();
    bool debug = false;// jy tested
    if (auto *comp_unit = root->as<CompUnit*>()){
        if(debug)
            fmt::print("comp_unit->all.size(): {}\n", comp_unit->all.size());
        for(int i = 0; i < comp_unit->all.size(); i++){
            //fmt::print("i: {}\n", i);
            auto tmp = comp_unit->all[i]->as<FuncDef*>();
            if(tmp != nullptr){
                funcSymbolPtr symbol = new funcSymbol();
                // return type
                if(tmp->ReturnType == FuncDef::Type::INT){
                    symbol->returnType = funcSymbol::Type::INT;
                    if(debug)
                        fmt::print("return type: INT\n");
                }else{
                    symbol->returnType = funcSymbol::Type::VOID;
                    if(debug)
                        fmt::print("return type: VOID\n");
                }
                // name
                symbol->name = tmp->name;
                if(debug){
                    fmt::print("name: {}\n", symbol->name);
                    fmt::print("arglist.size(): {}\n", tmp->argList.size());
                }
                // arglist
                for (auto arg : tmp->argList) {
                    varSymbolTypePtr argPtr = new varSymbolType();
                    if (arg->as<FuncFParam*>()->isArray) {
                        // array
                        argPtr->name = arg->as<FuncFParam*>()->name;
                        argPtr->isArray = true;
                        if(debug)
                            fmt::print("array name: {} dim = ", argPtr->name);
                        for (auto j : arg->as<FuncFParam*>()->dimensions) {
                            argPtr->dimension.emplace_back(j);
                            if(debug)
                                fmt::print(" {} ", j);
                        }
                    } else {
                        // variable
                        argPtr->name = arg->as<FuncFParam*>()->name;
                        argPtr->isArray = false;
                        if(debug)
                            fmt::print("variable name: {}\n", argPtr->name);
                    }
                    symbol->arglist.emplace_back(argPtr);
                }
            }
        }
    }
    return tablePtr;
}