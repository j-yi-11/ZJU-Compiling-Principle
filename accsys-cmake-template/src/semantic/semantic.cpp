#include "ast/ast.h"
#include "semantic/semantic.h"
funcSymbolTablePtr GenerateFuncSymTable(NodePtr root)
{
    funcSymbolTablePtr tablePtr = new funcSymbolTable();
    if (auto *comp_unit = root->as<CompUnit*>()){
        for(int i = 0; i < comp_unit->all.size(); i++){
            auto tmp = comp_unit->all[i]->as<FuncDef*>();
            if(tmp != nullptr){
                funcSymbolPtr symbol = new funcSymbol();
                // return type
                if(tmp->ReturnType == FuncDef::Type::INT){
                    symbol->returnType = funcSymbol::Type::INT;
                }else{
                    symbol->returnType = funcSymbol::Type::VOID;
                }
                // name
                symbol->name = tmp->name;
                // arglist
                for (auto arg : tmp->argList) {
                    varSymbolTypePtr argPtr = new varSymbolType();
                    if (arg->as<FuncFParam*>()->isArray) {
                        // array
                        argPtr->isArray = true;
                        for (auto j : arg->as<FuncFParam*>()->dimensions) {
                            argPtr->dimension.emplace_back(j);
                        }
                        
                        //for(int j = 0; j < )
                    } else {
                        // variable
                        argPtr->isArray = false;
                    }
                    symbol->arglist.emplace_back(argPtr);
                }
            }
        }
    }
    return tablePtr;
}