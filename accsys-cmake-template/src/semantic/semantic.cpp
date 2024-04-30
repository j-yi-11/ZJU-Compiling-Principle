#include "ast/ast.h"
#include "semantic/semantic.h"
#include <fmt/core.h>
#include <vector>
#include <string>
#include <cstdlib>
funcSymbolTablePtr GenerateFuncSymTable(NodePtr root)
{
    funcSymbolTablePtr tablePtr = new funcSymbolTable();
    bool debug = false;// jy tested
    if (auto *comp_unit = root->as<CompUnit*>()){
        if(debug)
            fmt::print("comp_unit->all.size(): {}\n", comp_unit->all.size());
        for(size_t i = 0; i < comp_unit->all.size(); i++){
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
                        // argPtr->name = arg->as<FuncFParam*>()->name;
                        argPtr->isArray = true;
                        if(debug)
                            fmt::print("array dim = ");
                        for (auto j : arg->as<FuncFParam*>()->dimensions) {
                            argPtr->dimension.emplace_back(j);
                            if(debug)
                                fmt::print(" {} ", j);
                        }
                    } else {
                        // variable
                        // argPtr->name = arg->as<FuncFParam*>()->name;
                        argPtr->isArray = false;
//                        if(debug)
//                            fmt::print("variable name: {}\n", argPtr->name);
                    }
                    symbol->arglist.emplace_back(argPtr);
                }
                // traverse funcTable to find name
                for(auto i: tablePtr->funcTable) {
                    if(i->name == symbol->name) {
                        fmt::print("re def func\n");
                        exit(1);
                    }
                }
                tablePtr->funcTable.emplace_back(symbol);
            }
        }
        funcSymbolPtr symbol = new funcSymbol();
        // void putint()
        symbol->returnType = funcSymbol::Type::VOID;
        symbol->name = "putint";
        varSymbolTypePtr argPtr = new varSymbolType();
        argPtr->isArray = false;
        symbol->arglist.emplace_back(argPtr);
        tablePtr->funcTable.emplace_back(symbol);
        if(debug) fmt::print("putint added\n");
        // int getint()
        symbol = new funcSymbol();
        symbol->returnType = funcSymbol::Type::INT;
        symbol->name = "getint";
        tablePtr->funcTable.emplace_back(symbol);
        if(debug) fmt::print("getint added\n");
        // void starttime()
        symbol = new funcSymbol();
        symbol->returnType = funcSymbol::Type::VOID;
        symbol->name = "starttime";
        tablePtr->funcTable.emplace_back(symbol);
        if(debug) fmt::print("starttime added\n");
        // void stoptime()
        symbol = new funcSymbol();
        symbol->returnType = funcSymbol::Type::VOID;
        symbol->name = "stoptime";
        tablePtr->funcTable.emplace_back(symbol);
        if(debug) fmt::print("stoptime added\n");
        // int getarray(int[])
        symbol = new funcSymbol();
        symbol->returnType = funcSymbol::Type::INT;
        symbol->name = "getarray";
        argPtr = new varSymbolType();
        argPtr->isArray = true;
        argPtr->dimension.emplace_back(-1);// int[]
        symbol->arglist.emplace_back(argPtr);
        tablePtr->funcTable.emplace_back(symbol);
        if(debug) fmt::print("getarray added\n");
        // void putarray(int,int[])
        symbol = new funcSymbol();
        symbol->returnType = funcSymbol::Type::VOID;
        symbol->name = "putarray";
        argPtr = new varSymbolType();
        argPtr->isArray = false;
        symbol->arglist.emplace_back(argPtr);
        argPtr = new varSymbolType();
        argPtr->isArray = true;
        argPtr->dimension.emplace_back(-1);// int[]
        symbol->arglist.emplace_back(argPtr);
        tablePtr->funcTable.emplace_back(symbol);
        if(debug) fmt::print("putarray added\n");
    }
    return tablePtr;
}

funcSymbol::Type findFuncReturnType(funcSymbolTablePtr TablePtr, std::string funcName) {
    for(auto i : TablePtr->funcTable){
        if(i->name == funcName){
            return i->returnType;
        }
    }
    return funcSymbol::Type::VOID;
}

void GenerateVarSymTable(NodePtr root, symbolTablePtr currentTable,
                         funcSymbolTablePtr funcTable, std::string funcName) {
    if(root == nullptr) return;
    bool debug = false;
    if(auto *comp_unit = root->as<CompUnit*>()){
        if(debug) fmt::print("comp_unit->all.size(): {}\n", comp_unit->all.size());
        for(size_t i = 0; i < comp_unit->all.size(); i++){
            auto decl = comp_unit->all[i]->as<Decl*>();
            if(decl!= nullptr) {
//                if(debug)
                GenerateVarSymTable(decl, currentTable, funcTable, funcName);
            }
            auto funcDef = comp_unit->all[i]->as<FuncDef*>();
            if(funcDef != nullptr) {
                std::string funcName = funcDef->name;
                if(debug)
                    fmt::print("[CompUnit]: funcDef->name: {}\n", funcName);
                GenerateVarSymTable(funcDef, currentTable, funcTable, funcName);
            }
        }
    }
    else if(auto *decl = root->as<Decl*>()){
        if(debug){
            fmt::print("encounter Decl: decl->VarDecl->VarDefs.size(): {}\n", decl->VarDecl->as<VarDecl*>()->VarDefs.size());
        }
        // construct varTable of currentTable
        // add varDecl, varDef to varTable
        std::vector<NodePtr> varDefs = decl->VarDecl->as<VarDecl*>()->VarDefs;
        for(size_t j = 0; j < varDefs.size(); j++){
            auto varDef = varDefs[j]->as<VarDef*>();
            if(varDef != nullptr){
                varSymbolTypePtr symbol = new varSymbolType();
                symbol->name = varDef->name;
                if(findVarRedefineByName(currentTable, varDef->name)==true) {
                    fmt::print("var re defined: {}\n",varDef->name);
                    exit(1);
                }
                if(debug) fmt::print("varDef->name: {}\n", varDef->name);
                if(varDef->isArray){
                    symbol->isArray = true;
                    if(debug) fmt::print("array dim = ");
                    for(auto k : varDef->dimensions){
                        symbol->dimension.emplace_back(k);
                        if(debug) fmt::print(" {} ", k);
                    }
                }else{
                    symbol->isArray = false;
                    // deal with initial value(InitVal contianing NodePtr Exp) valid or not
                    if(varDef->initialValue != nullptr) {
//                        if(debug) {
//                            fmt::print("initial value check");
//                        }
//                        bool shouldReturnInt = true;
                        if (checkExp(varDef->initialValue, currentTable, funcTable)) {//, shouldReturnInt
                            // valid pass
                        } else {
                            // invalid error
                            exit(1);
                        }
                    }
                }
//                fmt::print("symbol->name: {}\n", symbol->name);
                currentTable->varTable.insert(std::make_pair(symbol->name, symbol));
            }

        }
        if(debug){
            fmt::print("\nfinished decl:  currentTable->varTable.size()=={}\n", currentTable->varTable.size());
        }
        return;
    }
//    else id(auto *vardef = root->as<VarDef*>()){
//
//    }
    else if(auto *funcdef = root->as<FuncDef*>()){
        if(debug)
            fmt::print("encounter FuncDef: funcdef->name: {}\n", funcdef->name);
        // when encounter block, create a new SymbolTable, make it point to currentTable
        SymbolTable* newTable = new SymbolTable();
        newTable->highLevelTable = currentTable;
        // get funcDef name
        std::string funcName = funcdef->name;
        // todo add param to newTable->varTable
        std::vector<NodePtr> funcArgList = funcdef->argList;
        for(auto i:funcArgList) {
            if(auto *funcfparam = i->as<FuncFParam*>()){
                varSymbolTypePtr argPtr = new varSymbolType();
                argPtr->name = funcfparam->name;
                argPtr->isArray = funcfparam->isArray;
                if(funcfparam->isArray) {
                    for(auto j:funcfparam->dimensions) {
                        argPtr->dimension.emplace_back(j);
                    }
                }
                newTable->varTable.insert(std::make_pair(argPtr->name,argPtr));
            }
        }
        if(debug) fmt::print("[FuncDef]: newTable->varTable.size() = {}\n",newTable->varTable.size());
        GenerateVarSymTable(funcdef->block, newTable, funcTable,funcName);
    }
    else if(auto *block = root->as<Block*>()){
        if(debug){
            fmt::print("encounter Block: block->BlockItems.size(): {}\n", block->BlockItems.size());
        }
        for(size_t i = 0; i < block->BlockItems.size(); i++){
            auto decl = block->BlockItems[i]->as<BlockItem *>()->Decl;
            if(decl!= nullptr) {
                if(debug) fmt::print("[Block]: Decl in Block\n");
                GenerateVarSymTable(decl, currentTable, funcTable, funcName);
            }
            else {
                if(debug) fmt::print("[Block]: Stmt in Block\n");
                GenerateVarSymTable(block->BlockItems[i]->as<BlockItem *>()->Stmt, currentTable, funcTable, funcName);
            }
        }
    }
    else if(auto *returnstmt = root->as<ReturnStmt*>()){
        if(debug){
            fmt::print("encounter ReturnStmt\n");
        }
        // find the return type of function according to funcName from funcTable
        funcSymbol::Type returnType = findFuncReturnType(funcTable, funcName);
        if(returnType == funcSymbol::Type::VOID) {
            // return type is void
            fmt::print("[ReturnStmt]: return void\n");
            if (returnstmt->result != nullptr) {
                fmt::print("return type is void, but return statement has expression\n");
                exit(1);
            }
        }else{
            // return type is int
            fmt::print("[ReturnStmt]: return int\n");
            if (returnstmt->result == nullptr) {
                fmt::print("return type is int, but return statement has no expression\n");
                exit(1);
            }
//            bool shouldReturnInt = true;// return int
            if(checkExp(returnstmt->result, currentTable, funcTable)==false){//, shouldReturnInt
                fmt::print("return type is int, but return statement has invalid expression\n");
                exit(1);
            }
        }
    }
    else if(auto *whilestmt = root->as<WhileStmt*>()){
        if(debug){
            fmt::print("encounter WhileStmt\n");
        }
//        bool shouldReturnInt = true;// while(int)
        if(checkExp(whilestmt->condition, currentTable, funcTable)==false){//, shouldReturnInt
            fmt::print("while statement has invalid condition expression\n");
            exit(1);
        }else{
            GenerateVarSymTable(whilestmt->then, currentTable, funcTable, funcName);
        }
    }
    else if(auto *blockstmt = root->as<BlockStmt*>()){
        if(debug){
            fmt::print("encounter BlockStmt\n");
        }
        // new SymbolTable
        SymbolTable* newTable = new SymbolTable();
        newTable->highLevelTable = currentTable;
        GenerateVarSymTable(blockstmt->Block, newTable, funcTable, funcName);
    }
    else if(auto *expstmt = root->as<ExpStmt*>()){
        if(debug){
            fmt::print("encounter ExpStmt\n");
        }
        // only in this part can use void f(): f();
        // expstmt->Exp->LgExp->
        bool callVoidFunc = false;
//        auto *unaryexp;
        if(auto *exp = expstmt->Exp->as<Expr*>()){
            if(auto *lgexp = exp->LgExp->as<LgExp*>()){
                if(lgexp->rhs== nullptr && lgexp->lhs != nullptr){
                    if(auto *lglgexp = lgexp->lhs->as<LgExp*>()) {
                        if(lglgexp->rhs == nullptr && lglgexp->lhs != nullptr) {
                            if(auto *compexp = lglgexp->lhs->as<CompExp*>()){
                                if(compexp->rhs == nullptr && compexp->lhs != nullptr) {
                                    if(auto *compcompexp = compexp->lhs->as<CompExp*>()){
                                        if(compcompexp->rhs== nullptr && compcompexp->lhs != nullptr){
                                            if(auto *addexp = compcompexp->lhs->as<AddExp*>()){
                                                if(addexp->addExp == nullptr && addexp->mulExp != nullptr){
                                                    if(auto *mulexp = addexp->mulExp->as<MulExp*>()){
                                                        if(mulexp->mulExp == nullptr && mulexp->unaryExp != nullptr){
                                                            if(auto *unaryexp = mulexp->unaryExp->as<UnaryExpr*>()){
                                                                if(unaryexp->unaryExp == nullptr && unaryexp->primaryExp == nullptr && unaryexp->name != ""){
                                                                    fmt::print("void function call within a line\n");
                                                                    callVoidFunc = true;
                                                                    checkFuncCall(unaryexp,currentTable,funcTable,false);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
//        if(callVoidFunc){
//            checkFuncCall(expstmt->Exp,currentTable,funcTable,false);
//        }
        if(!callVoidFunc && checkExp(expstmt->Exp, currentTable, funcTable)==false){
            fmt::print("expression statement has invalid expression\n");
            exit(1);
        }
    }
    else if(auto *assignstmt = root->as<AssignStmt*>()){
        if(debug){
            fmt::print("encounter AssignStmt\n");
        }
//        bool shouldReturnInt = true;// a = (int)
        if(checkExp(assignstmt->Exp, currentTable, funcTable)==false){//, shouldReturnInt
            fmt::print("assign statement has invalid expression\n");
            exit(1);
        }
//        shouldReturnInt = false;// lval can not be fun call
        if(checkExp(assignstmt->LVal, currentTable, funcTable)==false){//, shouldReturnInt
            fmt::print("assign statement has invalid lvalue\n");
            exit(1);
        }
    }
    else if(auto *ifstmt = root->as<IfStmt*>()){
        if(debug){
            fmt::print("encounter IfStmt\n");
        }
//        bool shouldReturnInt = true;// if(int)
        if(checkExp(ifstmt->condition, currentTable, funcTable)==false){//, shouldReturnInt
            fmt::print("if statement has invalid condition expression\n");
            exit(1);
        }else{
            GenerateVarSymTable(ifstmt->then, currentTable, funcTable, funcName);
            GenerateVarSymTable(ifstmt->els, currentTable,funcTable, funcName);
        }
    } else{
        return;
    }
}

bool checkExp(NodePtr exp, symbolTablePtr currentTable,
              funcSymbolTablePtr funcTablePtr) {//, bool shouldReturnInt
    if(exp == nullptr)  return true;
    // TODO : implement this function
    // fmt::print("encounter checkExp\n");
    bool debug = false;
    if(auto* expr = exp->as<Expr*>()){
        if(debug)
            fmt::print("[checkExp]: expr\n");
        return checkExp(expr->LgExp, currentTable, funcTablePtr);//, shouldReturnInt
    }
    if(auto *lgexp = exp->as<LgExp*>()){
        if(debug)
            fmt::print("[checkExp]: lgexp\n");
        return checkExp(lgexp->lhs, currentTable, funcTablePtr) && //, shouldReturnInt
                checkExp(lgexp->rhs, currentTable, funcTablePtr);//, shouldReturnInt
    }
    if(auto *compexp = exp->as<CompExp*>()){
        if(debug)
            fmt::print("[checkExp]: compexp\n");
        return checkExp(compexp->lhs, currentTable, funcTablePtr) && //, shouldReturnInt
                checkExp(compexp->rhs, currentTable, funcTablePtr);//, shouldReturnInt
    }
    if(auto *addexp = exp->as<AddExp*>()){
        if(debug)
            fmt::print("[checkExp]: addexp\n");
        return checkExp(addexp->addExp, currentTable, funcTablePtr) && //, shouldReturnInt
                checkExp(addexp->mulExp, currentTable, funcTablePtr);//, shouldReturnInt
    }
    if(auto *mulexp = exp->as<MulExp*>()){
        if(debug)
            fmt::print("[checkExp]: mulexp\n");
        return checkExp(mulexp->mulExp, currentTable, funcTablePtr) && //, shouldReturnInt
                checkExp(mulexp->unaryExp, currentTable, funcTablePtr);//, shouldReturnInt
    }
    if(auto *unaryexp = exp->as<UnaryExpr*>()){
        // todo check fun call
        if(debug)
            fmt::print("[checkExp]: unaryexp ");
        bool result = true;
        if(unaryexp->name != "") {
            fmt::print("name is {} , checkFunCall\n", unaryexp->name);
            bool shouldReturnInt = true;
            // todo:find return type of function given by name==>judge shouldReturnInt
            if(unaryexp->name == "putint") shouldReturnInt = false;
            result = result && checkFuncCall(unaryexp, currentTable, funcTablePtr, shouldReturnInt);
        }
        if(unaryexp->unaryExp != nullptr) {
            if(debug) fmt::print("unaryExp\n");
            result = result && checkExp(unaryexp->unaryExp, currentTable, funcTablePtr);//,shouldReturnInt
        }
        if(unaryexp->primaryExp != nullptr) {
            if(debug) fmt::print("primaryExp\n");
            result = result && checkExp(unaryexp->primaryExp, currentTable, funcTablePtr);//, shouldReturnInt
        }
        return result;
    }
    if(auto *primaryexp = exp->as<PrimaryExpr*>()){
        // todo check lval
        if(debug) fmt::print("[checkExp]: primaryexp ");
        bool result = true;
        if(primaryexp->Exp != nullptr) {
            if(debug) fmt::print("Exp\n");
            result = result && checkExp(primaryexp->Exp, currentTable, funcTablePtr);//, shouldReturnInt
        }
        if(primaryexp->Number != nullptr) {
            if(debug) fmt::print("Number\n");
            result = result && checkExp(primaryexp->Number, currentTable, funcTablePtr);//, shouldReturnInt
        }
        if(primaryexp->LVal != nullptr) {
            if(debug) fmt::print("LVal\n");
            result = result && checkLval(primaryexp->LVal->as<LVal*>(), currentTable, funcTablePtr);
        }
        return  result;

    }
    if(auto *integerliteral = exp->as<IntegerLiteral*>()){
        if(debug)
            fmt::print("[checkExp]: integerliteral->value: {}\n", integerliteral->value);
        return true;
    }
    if(auto *lval = exp->as<LVal*>()){
        if(debug)
            fmt::print("[checkExp]: lval->name: {}\n", lval->name);
        return checkLval(lval, currentTable, funcTablePtr);
    }
    if(auto *initialvalue = exp->as<InitVal*>()){
        if(debug)
            fmt::print("[checkExp]: InitVal\n");
        return checkExp(initialvalue->Exp,currentTable,funcTablePtr);
    }
    else{
        fmt::print("[checkExp]: not match type\n");
        return false;
    }
}

varSymbolTypePtr findVarByName(symbolTablePtr currentTable, std::string name) {
//    std::map<std::string, varSymbolTypePtr> varTable
    bool debug = true;
    if(debug) fmt::print("[findVarByName]: ");
    while(currentTable != nullptr){
        for(auto &element : currentTable->varTable){
            if(element.first == name) {
                if(debug) fmt::print("var {} found\n",name);
                return element.second;
            }
        }
        currentTable = currentTable->highLevelTable;
    }
    if(debug) fmt::print("var {} not found\n", name);
    exit(1);
}

bool findVarRedefineByName(symbolTablePtr currentTable, std::string name) {
    bool debug = false;
    if(debug) fmt::print("[findVarRedefineByName]: ");
    // redefine means define in same block
    for(auto &element : currentTable->varTable){
        if(element.first == name) {
            if(debug) fmt::print("var {} redefine found\n",name);
            return true;
        }
    }
    return false;
}

bool checkLval(NodePtr exp, symbolTablePtr currentTable, funcSymbolTablePtr funcTablePtr){
    if(exp == nullptr) {
        fmt::print("lvalue is nullptr\n");
        return true;//todo
    }
    bool debug = false;
    if(debug) fmt::print("check lvalue\n");
    bool isArray = false;
    std::vector<int> position;
    std::string name;
//    if(auto *primaryexp = exp->as<PrimaryExpr *>()){
//        // get isarray, position
//        auto *lval = primaryexp->LVal->as<LVal*>();
//        isArray = lval->isArray;
//        position = lval->position;
//    }
    if(auto *lval = exp->as<LVal*>()) {
        if (debug) fmt::print("lval->name: {}\n", lval->name);
        // get info of lval
        name = lval->name;
        isArray = lval->isArray;
        position = lval->position;
        // find name in currentTable
        varSymbolTypePtr symbol = findVarByName(currentTable, name);
        // if found
        if (symbol != nullptr) {
            if (debug) fmt::print("symbol->name: {}\n", symbol->name);
            if (debug) fmt::print("symbol->isArray: {}\n", symbol->isArray);
            if (debug) fmt::print("symbol->dimension.size(): {}\n", symbol->dimension.size());
            if (debug) fmt::print("position.size(): {}\n", position.size());
            if (symbol->isArray) {
                if (position.size() != symbol->dimension.size()) {
                    fmt::print("lvalue array dimension not match\n");
                    return false;
                }
                for (size_t i = 0; i < position.size(); i++) {
                    if (position[i] >= symbol->dimension[i] && i != 0) {
                        fmt::print("lvalue array index out of range\n");
                        fmt::print("position[{}]: {}\n", i, position[i]);
                        fmt::print("dimension[{}]: {}\n", i, symbol->dimension[i]);
                        exit(1);
                        return false;
                    }
                }
            } else {
                if (position.size() != 0) {
                    fmt::print("lvalue variable has no dimension\n");
                    return false;
                }
            }
        }
        return true;
    } else {
        fmt::print("lvalue is not LVal\n");
        return false;
    }
}

funcSymbolPtr findFuncByName(funcSymbolTablePtr funcTablePtr, std::string name) {
    for(auto i : funcTablePtr->funcTable){
        if(i->name == name){
            return i;
        }
    }
    fmt::print("[ERROR]: function {} not found\n", name);
    exit(1);
}

bool checkFuncCall(NodePtr exp, symbolTablePtr currentTable,
                   funcSymbolTablePtr funcTablePtr, bool shouldReturnInt){
    if(exp == nullptr) {
        fmt::print("[checkFuncCall]: exp is nullptr\n");
        return false; //todo
    }
    bool debug = true;
    if(debug)
        fmt::print("[checkFuncCall]: begin\n");
    std::string name = "";
    std::vector<NodePtr> params;
    if(auto *unaryexp = exp->as<UnaryExpr *>()){
        // get name, params
        name = unaryexp->name;
        params = unaryexp->params;
    }
    if(debug) {
//        fmt::print("[checkFuncCall]: func name: {}\n", name);
//        fmt::print("[checkFuncCall]: func params.size(): {}\n", params.size());
        // for all params, print their name, type, dimension
    }
    if(name == ""){
        fmt::print("[ERROR]: function call name is empty\n");
        return false;// todo
    }
    // find func by name
    funcSymbolPtr symbol = findFuncByName(funcTablePtr, name);
    // print all thing in symbol->arglist
//    if(debug) {
//        fmt::print("[checkFuncCall]: symbol->arglist.size(): {}\n", symbol->arglist.size());
//        for(auto i : symbol->arglist){
//            if(i->isArray){
//                fmt::print("[checkFuncCall]: arg name: {} is array with ", i->name);
//                fmt::print("dimension size: {}\n", i->dimension.size());
//                for(auto j : i->dimension){
//                    fmt::print("[{}]", j);
//                }
//                fmt::print("\n");
//            }else{
//                fmt::print("[checkFuncCall]: arg name: {} is variable\n", i->name);
//            }
//        }
//    }
    // compare return type
    if(shouldReturnInt && symbol->returnType != funcSymbol::Type::INT) {
        fmt::print("[ERROR]: return type mismatch\n");
        exit(1);
    }
    // compare parameters
    if(params.size() != symbol->arglist.size()){
        fmt::print("[ERROR]: parameter number not match\n");
        return false;
    }
    for(size_t i = 0; i < params.size(); i++){
        if(debug) fmt::print("[checkFuncCall]: i: {}\n", i);
        // symbol is formal parameter
        // params[i] is actual parameter
        // todo check param[i] is int exp
        // todo check params[i] is variable call
//        if(auto* temp = params[i]->as<Expr*>()->LgExp->as<LgExp*>()->lhs->as<LgExp*>()->lhs->as<CompExp*>()->lhs->as<CompExp*>()->lhs->as<AddExp*>()->mulExp->as<MulExp*>()->unaryExp->as<UnaryExpr*>()->primaryExp->as<PrimaryExpr*>()->LVal->as<LVal*>()){
//            fmt::print("get actual parameter name: {}\n",temp->name);
//            varSymbolTypePtr tmp = findVarByName(currentTable, temp->name);
//            // print info of tmp
//            if(tmp->isArray){
//                fmt::print("{} is array with ",tmp->name);
//                //print dimension
//                for(auto j:tmp->dimension){
//                    fmt::print("[{}]",j);
//                }
//                fmt::print("\n");
//            }else{
//                fmt::print("{} is variable\n",tmp->name);
//            }
//        }
        if(symbol->arglist[i]->isArray == false){// && params[i]->as<LVal*>() == nullptr
            // valid: not array, all int, check params[i] is int exp
            if(checkExp(params[i], currentTable, funcTablePtr) == false){// shouldReturnInt
                fmt::print("[ERROR]: function call parameter type not match int\n");
                return false;
            }
        }else if(symbol->arglist[i]->isArray == true) {// && params[i]->as<LVal*>() != nullptr
            // valid: all array, compare dimension
            // auto *lval = params[i]->as<LVal*>();
//            if(lval->position.size() != symbol->arglist[i]->dimension.size()){
//                fmt::print("[ERROR]: function call parameter array dimension size not match\n");
//                return false;
//            }
//            for(size_t j = 0; j < lval->position.size(); j++){
//                if(lval->position[j] != symbol->arglist[i]->dimension[j] && j != 0){
//                    fmt::print("[ERROR]: function call parameter array {}th dimension not match\n", j+1);
//                    return false;
//                }
//            }
            bool isArrayCall = false;
            if(auto *exp = params[i]->as<Expr*>()){
                if(auto *lgexp = exp->LgExp->as<LgExp*>()){
                    if(lgexp->rhs== nullptr && lgexp->lhs != nullptr){
                        if(auto *lglgexp = lgexp->lhs->as<LgExp*>()) {
                            if(lglgexp->rhs == nullptr && lglgexp->lhs != nullptr) {
                                if(auto *compexp = lglgexp->lhs->as<CompExp*>()){
                                    if(compexp->rhs == nullptr && compexp->lhs != nullptr) {
                                        if(auto *compcompexp = compexp->lhs->as<CompExp*>()){
                                            if(compcompexp->rhs== nullptr && compcompexp->lhs != nullptr){
                                                if(auto *addexp = compcompexp->lhs->as<AddExp*>()){
                                                    if(addexp->addExp == nullptr && addexp->mulExp != nullptr){
                                                        if(auto *mulexp = addexp->mulExp->as<MulExp*>()){
                                                            if(mulexp->mulExp == nullptr && mulexp->unaryExp != nullptr){
                                                                if(auto *unaryexp = mulexp->unaryExp->as<UnaryExpr*>()){
                                                                    if(unaryexp->unaryExp == nullptr && unaryexp->primaryExp != nullptr && unaryexp->name == ""){
                                                                        if(auto* primaryexp = unaryexp->primaryExp->as<PrimaryExpr*>()){
                                                                            if(primaryexp->LVal != nullptr){
                                                                                isArrayCall = true;
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if(isArrayCall){
                fmt::print("array call\n");
                auto *temp = params[i]->as<Expr*>()->LgExp->as<LgExp*>()->lhs->as<LgExp*>()->lhs->as<CompExp*>()->lhs->as<CompExp*>()->lhs->as<AddExp*>()->mulExp->as<MulExp*>()->unaryExp->as<UnaryExpr*>()->primaryExp->as<PrimaryExpr*>()->LVal->as<LVal*>();
                fmt::print("get actual parameter name: {}\n",temp->name);
                varSymbolTypePtr tmp = findVarByName(currentTable, temp->name);
                if(tmp->isArray){
                    if(tmp->dimension.size() - temp->position.size() != symbol->arglist[i]->dimension.size()){
                        fmt::print("tmp->dimension.size(): {}\n",tmp->dimension.size());
                        fmt::print("temp->position.size(): {}\n",temp->position.size());
                        fmt::print("symbol->arglist[i]->dimension.size(): {}\n",symbol->arglist[i]->dimension.size());
                        fmt::print("function call parameter array dimension size not match\n");
                        return false;
                    }
                    for(size_t j = 0; j < temp->position.size(); j++){
                        if(temp->position[j] != symbol->arglist[i]->dimension[j] && j!=0){
                            fmt::print("[ERROR]: function call parameter array {}th dimension not match\n", j+1);
                            return false;
                        }
                    }
                }else{
                    fmt::print("[ERROR]: function call parameter is not array\n");
                    exit(1);
                    return false;
                }
            }else{
                fmt::print("[ERROR]: function call parameter is not array\n");
                exit(1);
                return false;
            }
        }else{
//            fmt::print("[ERROR]: function call parameter type not match\n");
//            return false;
        }
    }
    fmt::print("[checkFuncCall]: func Call valid\n");
    return true;
}