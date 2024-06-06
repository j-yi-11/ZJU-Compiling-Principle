#include "ir/ir.h"
#include "ir/type.h"
#include <string>
#include <vector>
NodePtr parser_root = nullptr;

void Module::genGlobalList(NodePtr root)
{
    fmt::print("genGlobalList()\n");
    parser_root = root;
    bool debug = false;
    if(root == nullptr) return;
    if(auto *comp_unit = root->as<CompUnit*>()) {
        if(debug) 
            fmt::print("comp_unit->all.size() = {}\n",comp_unit->all.size());
        for(int i = 0; i < comp_unit->all.size(); i++) {
            auto decl = comp_unit->all[i]->as<Decl*>();
            if (decl == nullptr) 
                continue;
            std::vector<NodePtr> varDefs = decl->VarDecl->as<VarDecl*>()->VarDefs;
            if(debug) 
                fmt::print("varDefs.size() = {}\n",varDefs.size());
            for(int j = 0; j < varDefs.size(); j++) {
                auto varDef = varDefs[j]->as<VarDef*>();
                if(varDef != nullptr) {
                    // 数组
                    if(varDef->isArray) {
                        // 数组维数乘积
                        if(debug) fmt::print("array\n");
                        int dim_product = 1;
                        for (auto k : varDef->dimensions) {
                            dim_product *= k;
                        }
                        if(debug) fmt::print("array : dim_product = {}\n",dim_product);
                        auto global_var = GlobalVariable::Create(Type::getIntegerTy(), dim_product, false, varDef->name, this);
                        for (auto k : varDef->dimensions) {
                            global_var->setBounds(k);    // set bounds of global array variable
                        }
                        if(debug) fmt::print("array pass create\n");
                    } else {    // 整型
                        GlobalVariable::Create(Type::getIntegerTy(), 1, false, varDef->name, this);
                        if(debug) fmt::print("int pass create\n");
                    }
                }
            }
            
        }
        
    }
}

void Function::globalValInitial()
{
    bool debug = true;
    if(auto *comp_unit = parser_root->as<CompUnit*>()) {
        if(debug)
            fmt::print("comp_unit->all.size() = {}\n",comp_unit->all.size());
        for(int i = 0; i < comp_unit->all.size(); i++) {
            auto decl = comp_unit->all[i]->as<Decl*>();
            if (decl == nullptr)
                continue;
            std::vector<NodePtr> varDefs = decl->VarDecl->as<VarDecl*>()->VarDefs;
            if(debug)
                fmt::print("varDefs.size() = {}\n",varDefs.size());
            for(int j = 0; j < varDefs.size(); j++) {
                auto varDef = varDefs[j]->as<VarDef*>();
                if(varDef != nullptr) {
                    // 数组
                    if(varDef->isArray) {

                    } else {    // 整型
                        // GlobalVariable::Create(Type::getIntegerTy(), 1, false, varDef->name, this);
                        // if(debug) fmt::print("int pass create\n");
                        // added
                        if (varDef->initialValue != nullptr) {
                            // assign statement
                            fmt::print("assign initial value for the global variable\n");
                            Value *rv = PostOrderTraversal(varDef->initialValue, block_exec_ordered[0], true);
                            Value *lv = findVariableAddr(varDef, nullptr);
                            StoreInst *store = StoreInst::Create(rv, lv);
                            block_exec_ordered[0]->instructions.push_back(store);

                        }
                    }
                }
            }

        }

    }
}

void Module::declExLinkFunction()
{
    bool debug = false;
    if(debug) fmt::print("declExLinkFunction()\n");
    // putint
    Type *Result_putint = Type::getUnitTy();
    std::vector<Type *> Params_putint;
    Params_putint.push_back(Type::getIntegerTy());
    FunctionType *FTy_putint = new FunctionType(Result_putint, Params_putint);
    auto f_putint = Function::Create(FTy_putint, true, "putint", this);
    f_putint->getArg(0)->setName("x");
    // putch
    Type *Result_putch = Type::getUnitTy();
    std::vector<Type *> Params_putch;
    Params_putch.push_back(Type::getIntegerTy());
    FunctionType *FTy_putch = new FunctionType(Result_putch, Params_putch);
    auto f_putch = Function::Create(FTy_putch, true, "putch", this);
    f_putch->getArg(0)->setName("x");
    // putarray
    Type *Result_putarray = Type::getUnitTy();
    std::vector<Type *> Params_putarray;
    Params_putarray.push_back(Type::getIntegerTy());
    Params_putarray.push_back(new PointerType(Type::getIntegerTy()));
    FunctionType *FTy_putarray = new FunctionType(Result_putarray, Params_putarray);
    auto f_putarray = Function::Create(FTy_putarray, true, "putarray", this);
    f_putarray->getArg(0)->setName("n");
    f_putarray->getArg(1)->setName("arr");
    f_putarray->getArg(1)->dimensions.push_back(-1);
    // getint
    Type *Result_getint = Type::getIntegerTy();
    std::vector<Type *> Params_getint;  // empty
    FunctionType *FTy_getint = new FunctionType(Result_getint, Params_getint);
    Function::Create(FTy_getint, true, "getint", this);
    // auto f_getint = Function::Create(FTy_getint, true, "getint", this);
    // getch
    Type *Result_getch = Type::getIntegerTy();
    std::vector<Type *> Params_getch;  // empty
    FunctionType *FTy_getch = new FunctionType(Result_getch, Params_getch);
    Function::Create(FTy_getch, true, "getch", this);
    // auto f_getch = Function::Create(FTy_getch, true, "getch", this);
    // getarray
    Type *Result_getarray = Type::getUnitTy();
    std::vector<Type *> Params_getarray;
    Params_getarray.push_back(Type::getIntegerTy());
    Params_getarray.push_back(new PointerType(Type::getIntegerTy()));
    FunctionType *FTy_getarray = new FunctionType(Result_getarray, Params_getarray);
    auto f_getarray = Function::Create(FTy_getarray, true, "getarray", this);
    f_getarray->getArg(0)->setName("n");
    f_getarray->getArg(1)->setName("arr");
    f_putarray->getArg(1)->dimensions.push_back(-1);
}

void Module::genFuncList(NodePtr root)
{
    bool debug = true;
    fmt::print("genFuncList()\n");
    // Step1: 外部库函数的声明
    declExLinkFunction();
    
    // Step2: 内部函数的声明
    if(root == nullptr) return;
    if(auto *comp_unit = root->as<CompUnit*>()) {
        for(int i = 0; i < comp_unit->all.size(); i++){
            auto funcDef = comp_unit->all[i]->as<FuncDef*>();
            if (funcDef == nullptr)
                continue;
            // ----------------------------------------------------
            if(debug) fmt::print("[genFuncList]: Create local function name {}\n",funcDef->name);
            Type *Result = funcDef->ReturnType == FuncDef::Type::INT ? Type::getIntegerTy() : Type::getUnitTy();
            std::vector<Type *> Params;
            for (auto arg : funcDef->argList) {
                // 遍历每一个参数
                if (arg->as<FuncFParam*>()->isArray) {
                    // array  -> Type应该是PointerType
                    Type *param = new PointerType(Type::getIntegerTy());
                    Params.push_back(param);
                } else {
                    // integer
                    Params.push_back(Type::getIntegerTy());
                }
            }
            FunctionType *FTy = new FunctionType(Result, Params);
            auto f = Function::Create(FTy, false, funcDef->name, this);
            // 传递参数的dimension
            for (int i = 0; i < f->getNumParams(); i++) {
                if (auto arg = funcDef->argList[i]->as<FuncFParam*>()) {
                    f->getArg(i)->setName(arg->name);
                    if (arg->isArray) {
                        for (auto t : arg->dimensions) {
                            // fmt::print("dimensions push back\n");
                            f->getArg(i)->dimensions.push_back(t);
                        }
                    }
                }
            }
            // ----------------------------------------------------
            // setName
            int index = 0;
            for (auto arg : funcDef->argList) {
                // 遍历每一个参数
                f->getArg(index)->setName(arg->as<FuncFParam*>()->name);
                index++;
            }
            // fmt::print("function create successfully\n");
            // 先生成Exit block和After Return block
            // f->generateExitBlock();
            // fmt::print("0\n");
            // f->generateReturnBlock();
            // fmt::print("0.0\n");
            // 生成temp block
            f->generateTempBlock(funcDef->block->as<Block*>(), nullptr);
            if(debug) fmt::print("[genFuncList]: f->generateTempBlock\n");
            f->AnalysisBlockOrder();
            if(debug) fmt::print("[genFuncList]: f->AnalysisBlockOrder\n");
            f->TempBlockConnect();
            if(debug) fmt::print("[genFuncList]: f->TempBlockConnect\n");
            f->CreateBlocks();
            if(debug) fmt::print("[genFuncList]: f->CreateBlocks\n");
            f->BasicBlockConnect();
            if(debug) fmt::print("[genFuncList]: f->BasicBlockConnect\n");
            f->generateReturnBlock();
            if(debug) fmt::print("[genFuncList]: f->generateReturnBlock\n");
        }
    }

}



// ok
TempBlock *Function::generateEntryBlock()
{
    fmt::print("generateEntryBlock() \n");
    TempBlock *entry_block = new TempBlock(TempBlock::TempBlockType::ENTRY, nullptr, cur_depth);
    // if(entry_block==nullptr){
    //     fmt::print("[ERROR]:generateEntryBlock: entry_block==nullptr\n");
    //     //assert false;
    // }
    // fmt::print("block_exec_ordered.size = {}\n",block_exec_ordered.size());
    block_exec_ordered.push_back(entry_block);
    // fmt::print("block_exec_ordered.size = {}\n",block_exec_ordered.size());
    globalValInitial();
    // 分配参数的空间
    for (int i = 0; i < (int)getNumParams(); i++) {
        Argument *arg = getArg(i);
        if (arg->getType() == Type::getIntegerTy()) {
            auto alloca_param = AllocaInst::Create(Type::getIntegerTy(), 1);
            // if(alloca_param == nullptr){
            //     fmt::print("[ERROR]:generateEntryBlock: alloca_param==nullptr\n");
            // }
            alloca_param->setName((std::string)arg->getName()+".addr");
            (entry_block->instructions).push_back(alloca_param);
            getArg(i)->pointer = alloca_param;
            // fmt::print("entry_block->instructions.size() = {}\n",(entry_block->instructions).size());
            // store
            auto store_inst = StoreInst::Create(arg, alloca_param);
            entry_block->instructions.push_back(store_inst);
        }
    }
    // 分配返回值的空间
    if (this->getFunctionType()->getReturnType() == Type::getIntegerTy()) {
        auto alloca_return_var = AllocaInst::Create(Type::getIntegerTy(), 1);
        // if(alloca_return_var == nullptr){
        //     fmt::print("[ERROR]:generateEntryBlock: alloca_return_var==nullptr\n");
        // }
        alloca_return_var->setName("ret_val.addr");
        entry_block->instructions.push_back(alloca_return_var);
        // fmt::print("entry_block->instructions.size() = {}\n",(entry_block->instructions).size());
    }

    // exit
    generateExitBlock();
    // generateReturnBlock();

    return entry_block;
}
// ok
Value *Function::createLocalVariable(VarDef *varDef, TempBlock *root)
{
    fmt::print("createLocalVariable()\n");
    int dim_product = 1;
    if (varDef->isArray) {
        for (auto k : varDef->dimensions) {
            dim_product *= k;
        }
    }
    AllocaInst *alloca_instr;
    alloca_instr = AllocaInst::Create(Type::getIntegerTy(), dim_product);
    alloca_instr->setName(varDef->name);
    root->instructions.push_back(alloca_instr);
    if (varDef->isArray) {
        for (auto k : varDef->dimensions){
            alloca_instr->setBounds(k);
        }
    }
    return alloca_instr;
}
// ok
void Function::generateTempBlock(Block *rootBlock, TempBlock *parent)
{
    bool debug = false;
    if(debug) fmt::print("generateTempBlock: \n");
    // generate Entry block
    if (parent == nullptr) {
        parent = generateEntryBlock();

    }
    // return;
    if(debug) fmt::print("[generateTempBlock]: rootBlock->BlockItems.size() = {}\n",rootBlock->BlockItems.size());
    for (auto blockItem : rootBlock->BlockItems) {
        if(debug) fmt::print("[generateTempBlock]: blockItem:\n");
        auto statement = blockItem->as<BlockItem*>()->Stmt;
        auto declare = blockItem->as<BlockItem*>()->Decl;
        // decl语句
        if (statement == nullptr) {
            fmt::print("find out a declare statement\n");
            if (auto varDeclStmt = declare->as<Decl*>()->VarDecl->as<VarDecl*>()) {
                std::vector<NodePtr> varDefs = varDeclStmt->VarDefs;
                for (auto t : varDefs) {
                    fmt::print("createLocalVariable for {}\n", t->as<VarDef*>()->name);
                    auto alloca_instr = createLocalVariable(t->as<VarDef*>(), parent); // alloca
                    // added
                    if (t->as<VarDef*>()->initialValue != nullptr) {
                        // assign statement
                        fmt::print("assign initial value for the local variable\n");
                        Value *rv = PostOrderTraversal(t->as<VarDef*>()->initialValue, parent, true);
                        Value *lv = alloca_instr;
                        StoreInst *store = StoreInst::Create(rv, lv);
                        parent->instructions.push_back(store);

                    }
                }
            }
            continue;
        }   
            
        // assign语句
        if (auto assignStmt = statement->as<AssignStmt*>()) {
            Value * lv = nullptr;
            Value * rv = nullptr;
            fmt::print("find out an assign: \n");
            
            rv = PostOrderTraversal(assignStmt->Exp, parent, true);
            fmt::print("rVal generate successfully\n");
            lv = findVariableAddr(assignStmt->LVal->as<LVal*>(), parent);
            fmt::print("lVal generate successfully\n");
            
            StoreInst *store = StoreInst::Create(rv, lv);
            parent->instructions.push_back(store);

            fmt::print("create assignment success\n");
            
            continue;
        }
        
        // while语句
        if (auto whileStmt = statement->as<WhileStmt*>()) {
            if(debug) fmt::print("[generateTempBlock]: find out a while: \n");
            BlockStmt *then_block_stmt = whileStmt->then->as<BlockStmt*>();
            Block *then_block = then_block_stmt->Block->as<Block*>();
            
            // while_cond
            TempBlock *while_cond_block = new TempBlock(
                    TempBlock::TempBlockType::WHILE_COND, parent, cur_depth, 
                    if_index, else_index, while_index);
            block_exec_ordered.push_back(while_cond_block);
            while_index++;
            
            // condition
            Value *conditionVal = PostOrderTraversal(whileStmt->condition, while_cond_block, true);
            while_cond_block->condition = conditionVal;

            // while_body
            TempBlock *while_body_block = new TempBlock(
                    TempBlock::TempBlockType::WHILE_BODY, parent, cur_depth, 
                    if_index, else_index, while_cond_block->while_index);
            block_exec_ordered.push_back(while_body_block);

            cur_depth++;
            generateTempBlock(then_block, while_body_block);
            cur_depth--;

            // while_end
            TempBlock *while_end_block = new TempBlock(
                    TempBlock::TempBlockType::WHILE_END, parent, cur_depth, 
                    if_index, else_index, while_cond_block->while_index);
            block_exec_ordered.push_back(while_end_block);

            // 需要修改parent
            parent = while_end_block;
            if(debug) fmt::print("[generateTempBlock]: while : parent changed\n");
        }

        // expStmt语句
        if (auto expStmt = statement->as<ExpStmt*>()) {
            if(debug) fmt::print("[generateTempBlock]: find out expStmt\n");
            PostOrderTraversal(expStmt->Exp, parent, true);
        }
        
        // return语句
        if (auto returnStmt = statement->as<ReturnStmt*>()) {
            if(debug) fmt::print("[generateTempBlock]: returnStmt\n");
            Value *lv = nullptr;
            Value *rv = nullptr;
            if (returnStmt->result == nullptr) {
                // rv = ConstantUnit::Create();
                // JumpInst *jump_inst = JumpInst::Create(&back());
            } else {
                // block_exec_ordered的first是
                for (Instruction *t : block_exec_ordered[0]->instructions) {
                    if (AllocaInst::classof(t) && t->getName() == "ret_val.addr") {
                        lv = t;
                        break;
                    }
                }

                rv = PostOrderTraversal(returnStmt->result, parent, true);


                StoreInst *store_inst = StoreInst::Create(rv, lv);
                parent->instructions.push_back(store_inst);
            }

            // jump to exit
            JumpInst *jump_inst = JumpInst::Create(&back());
            parent->instructions.push_back(jump_inst);
            parent->next_block = std::nullopt;  // 没有后继的temp block了
        }
        
        // if语句
        if (auto ifStmt = statement->as<IfStmt*>()) {
            if(debug) fmt::print("[generateTempBlock]: find out ifStmt: \n");
            // if(ifStmt->then==nullptr){
            //     fmt::print("ifStmt->then==nullptr\n");
            // }
            // if(ifStmt->then->as<BlockStmt*>()==nullptr){
            //     fmt::print("ifStmt->then->as<BlockStmt*>()==nullptr\n");
            // }
            BlockStmt *then_block_stmt = ifStmt->then->as<BlockStmt*>();
            if (then_block_stmt == nullptr) {
                then_block_stmt = new BlockStmt();
                then_block_stmt->Block = new Block();
                auto blockItem = new BlockItem();
                blockItem->Stmt = ifStmt->then;
                then_block_stmt->Block->as<Block*>()->BlockItems.emplace_back(blockItem);
            }
            // if(then_block_stmt->Block==nullptr){
            //     fmt::print("then_block_stmt->Block==nullptr\n");
            // }
            // if(then_block_stmt->Block->as<Block*>()==nullptr){
            //     fmt::print("then_block_stmt->Block->as<Block*>()==nullptr\n");
            // }
            Block *then_block = then_block_stmt->Block->as<Block*>();
            fmt::print("0\n");
            // condition
            Value *conditionVal = PostOrderTraversal(ifStmt->condition, parent, true);
            if (conditionVal == nullptr) 
                assert("conditionVal is empty\n");
            if(parent != nullptr) 
                parent->condition = conditionVal;
            else{
                if(debug) fmt::print("[generateTempBlock]: parent is nullptr\n");
            }
            // fmt::print("1\n");
            TempBlock *if_then_block = new TempBlock(
                    TempBlock::TempBlockType::IF_THEN, parent, cur_depth, 
                    if_index, else_index, while_index);
            block_exec_ordered.push_back(if_then_block);
            // fmt::print("2\n");
            if_index++;

            cur_depth++;
            generateTempBlock(then_block, if_then_block);
            cur_depth--;
            // fmt::print("3\n");
            // matched if
            if(ifStmt->matched) {
                // fmt::print("4\n");
                // if_else
                TempBlock *if_else_block = nullptr;
                if_else_block = new TempBlock( 
                    TempBlock::TempBlockType::IF_ELSE, parent, cur_depth, 
                    if_then_block->if_index, if_then_block->else_index, while_index);
                block_exec_ordered.push_back(if_else_block);
                // fmt::print("5\n");
                Block *els_block = nullptr;
                // 三种可能性 IfStmt / BlockStmt / WhileStmt
                if (BlockStmt* els_block_stmt = ifStmt->els->as<BlockStmt*>()) {
                    if(debug) fmt::print("[generateTempBlock]: last else \n");
                    els_block = els_block_stmt->Block->as<Block*>();

                } else {
                    // 不是一个block，那就创建一个
                    if(debug) fmt::print("[generateTempBlock]: create els_block\n");
                    els_block = new Block();
                    BlockItem *temp_blockItem = new BlockItem();
                    temp_blockItem->as<BlockItem*>()->Stmt = ifStmt->els;
                    els_block->as<Block*>()->BlockItems.push_back(temp_blockItem);
                }
                cur_depth++;
                generateTempBlock(els_block, if_else_block);
                cur_depth--;

                else_index++;
            }
            // fmt::print("6\n");
            
            TempBlock *if_end_block = new TempBlock(
                    TempBlock::TempBlockType::IF_END, parent, cur_depth, 
                    if_then_block->if_index, if_then_block->else_index, while_index);
            block_exec_ordered.push_back(if_end_block);
            // fmt::print("7\n");
            // 需要修改parent
            parent = if_end_block;
            if(debug) fmt::print("[generateTempBlock]: parent changed\n");
        }
    }
}
// ok
void Function::generateReturnBlock()
{
    auto return_block = BasicBlock::Create(this, &back());
    // setName
    return_block->setName("after_return");
    // jump to exit
    JumpInst::Create(&back(), return_block);
    return;
}
// ok
void Function::generateExitBlock()
{
    fmt::print("generateExitBlock()\n");
    auto exit_block = BasicBlock::Create(this, nullptr);
    // if(exit_block == nullptr){
    //     fmt::print("exit_block null\n");
    //     return;
    // }
    exit_block->setName("exit");
    LoadInst *load_inst = nullptr;
    // fmt::print("2\n");
    // exec_ordered_exec[0]->instructions
    // fmt::print("block_exec_ordered[0]->instructions.size = {}\n",block_exec_ordered[0]->instructions.size());
    for (auto t : block_exec_ordered[0]->instructions) {
        if (t!=nullptr && AllocaInst::classof(t) && t->getName() == "ret_val.addr") {
            load_inst = LoadInst::Create(t, exit_block);
            break;
        }
    }
    // fmt::print("3\n");
    if (load_inst == nullptr)
        RetInst::Create(ConstantUnit::Create(), exit_block);
    else
        RetInst::Create(load_inst, exit_block);
    // fmt::print("4\n");
    return;
}
// ok
void Function::AnalysisBlockOrder()
{
    fmt::print("AnalysisBlockOrder()\n");
    bool debug = true;
    if (debug) fmt::print("block_exec_ordered.size(): {}\n", block_exec_ordered.size());
    for (int i = 0; i < (int)block_exec_ordered.size(); i++)
    {
        if (i == block_exec_ordered.size()-1) {
            block_exec_ordered[i]->is_last = true;
            if(debug) fmt::print("[AnalysisBlockOrder]: last block\n");
            continue;
        }
        if (!((block_exec_ordered[i]->next_block).has_value())) {
            // 该block已经提前返回了
            if(debug) fmt::print("[AnalysisBlockOrder]: block return in advance\n");
            continue;
        }
        switch(block_exec_ordered[i]->type) {
            case TempBlock::TempBlockType::ENTRY:
                if (block_exec_ordered[i+1]->type == TempBlock::TempBlockType::IF_THEN) {
                    block_exec_ordered[i]->branch = TempBlock::BranchType::BR;
                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::IF_THEN;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index;
                } else if (block_exec_ordered[i+1]->type == TempBlock::TempBlockType::WHILE_COND) {
                    block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::WHILE_COND;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->while_index;
                }
                break;
            case TempBlock::TempBlockType::IF_THEN:
                if(debug) fmt::print("[AnalysisBlockOrder]: if_then\n");
                if (block_exec_ordered[i+1]->depth > block_exec_ordered[i]->depth) {
                    // 内部又分裂出了新的block
                    if(debug) fmt::print("[AnalysisBlockOrder]: split new block\n");
                    if (block_exec_ordered[i+1]->type == TempBlock::TempBlockType::WHILE_COND) {
                        // while_cond
                        if(debug) fmt::print("[AnalysisBlockOrder]: if then jump to while cond\n");
                        fmt::print("-------------------------------------------------------------------\n");
                        block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                        block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type;
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->while_index;
                    } else {
                        // if then
                        block_exec_ordered[i]->branch = TempBlock::BranchType::BR;
                        block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type;
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index;
                    }
                    
                } else {
                    // 内部没有分裂出新的block，跳转到end
                    block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::IF_END;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i]->if_index;
                }
                break;
            case TempBlock::TempBlockType::IF_ELSE:
                if (block_exec_ordered[i+1]->depth > block_exec_ordered[i]->depth) {
                    if(debug) fmt::print("[AnalysisBlockOrder]: split new block\n");
                    if (block_exec_ordered[i+1]->type == TempBlock::TempBlockType::WHILE_COND) {
                        // while_cond
                        fmt::print("if then jump to while cond\n");
                        fmt::print("-------------------------------------------------------------------\n");
                        block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                        block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type;
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->while_index;
                    } else {
                        // 内部又分裂出了新的block
                        block_exec_ordered[i]->branch = TempBlock::BranchType::BR;
                        block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type;
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index; // 不是加1
                    }
                    
                    
                } else {
                    // 内部没有分裂出新的block，跳转到end
                    block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::IF_END;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i]->if_index;
                }
                break;
            case TempBlock::TempBlockType::IF_END:
                // 1. 跳转到当前层数下的下一个block
                if (block_exec_ordered[i]->depth == block_exec_ordered[i+1]->depth) {
                    if (block_exec_ordered[i+1]->type == TempBlock::TempBlockType::WHILE_COND) {
                        block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                        block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type; // while_cond
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->while_index;
                    } else {
                        block_exec_ordered[i]->branch = TempBlock::BranchType::BR;
                        block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type; // 一定是If_then
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index;
                    }
                    
                    
                } else {
                    // 这就不一定了，上一级如果是if，那就跳if_end，上一级如果是while，那就跳while_cond index -> parent的while index
                    if (block_exec_ordered[i]->parent->type == TempBlock::TempBlockType::WHILE_BODY) {
                        block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                        block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::WHILE_COND;
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i]->parent->while_index;
                    } else {
                        // 要跳转到上一级的if_end
                        block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                        block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::IF_END;
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index;
                    }
                }
                break;
//
            case TempBlock::TempBlockType::WHILE_COND:
                block_exec_ordered[i]->branch = TempBlock::BranchType::BR;
                block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::WHILE_BODY;
                block_exec_ordered[i]->next_block_index = block_exec_ordered[i]->while_index;
                break;
            case TempBlock::TempBlockType::WHILE_BODY:
                if (block_exec_ordered[i+1]->depth > block_exec_ordered[i]->depth) {
                    // 内部又分裂出了新的block
                    fmt::print("split new block\n");
                    if (block_exec_ordered[i+1]->type == TempBlock::TempBlockType::WHILE_COND) {
                        // while_cond
                        fmt::print("if then jump to while cond\n");
                        fmt::print("-------------------------------------------------------------------\n");
                        block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                        block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type;
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->while_index;
                    } else {
                        // if then
                        block_exec_ordered[i]->branch = TempBlock::BranchType::BR;
                        block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type;
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index;
                    }
                } else {
                    block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::WHILE_COND;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i]->while_index;
                }
                break;
            case TempBlock::TempBlockType::WHILE_END:
                if(debug) fmt::print("[AnalysisBlockOrder]: while_end\n");
                // 1. 跳转到当前层数下的下一个block
                if (block_exec_ordered[i]->depth == block_exec_ordered[i+1]->depth) {
                    if (block_exec_ordered[i+1]->type == TempBlock::TempBlockType::WHILE_COND) {
                        block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                        block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type; // while_cond
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->while_index;
                    } else {
                        block_exec_ordered[i]->branch = TempBlock::BranchType::BR;
                        block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type; // 一定是If_then
                        block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index;
                    }
                    
                    
                } else {
                    // 要沿着parent链找，找到第一个上层节点
                    // 如果是if_then -> if_end
                    // 如果是if_else -> if_end
                    // 如果是if_end -> (可能吗？) -> 忽略 -> 继续向上找 -> if_end->parent是if_then的parent
                    // gttttttttttttttdrrr/
                    TempBlock *temp_block = nullptr;
                    temp_block = block_exec_ordered[i]->parent;
                    fmt::print("[AnalysisBlockOrder]: while_end jump to the parent\n");
                    while(temp_block != nullptr) {
                        if(debug) fmt::print("[AnalysisBlockOrder]: while (temp_block != nullptr) loop\n");
                        if (temp_block->depth < block_exec_ordered[i]->depth && 
                            temp_block->type != TempBlock::TempBlockType::IF_END && 
                            temp_block->type != TempBlock::TempBlockType::WHILE_END) {
                            if(debug) fmt::print("[AnalysisBlockOrder]: enter if\n");
                            switch(temp_block->type) {
                                case TempBlock::TempBlockType::ENTRY:
                                    block_exec_ordered[i]->next_block = std::nullopt;
                                    block_exec_ordered[i]->is_last = true;
                                    break;
                                case TempBlock::TempBlockType::IF_THEN:
                                case TempBlock::TempBlockType::IF_ELSE:
                                    block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::IF_END;
                                    block_exec_ordered[i]->next_block_index = temp_block->if_index;
                                    break;
                                case TempBlock::TempBlockType::WHILE_BODY:
                                    block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::WHILE_COND;
                                    block_exec_ordered[i]->next_block_index = temp_block->while_index;
                                    break;
                                default: 
                                    fmt::print("[AnalysisBlockOrder]: ERROR: END block can't find out the next block\n");
                                    break;
                            }
                        }
                        
                        if(debug) fmt::print("[AnalysisBlockOrder]: loop end\n");
                        temp_block = temp_block->parent;
                    }
                    

                    // // 这就不一定了，上一级如果是if，那就跳if_end，上一级如果是while，那就跳while_cond index -> parent的while index
                    // if (block_exec_ordered[i]->parent->type == TempBlock::TempBlockType::WHILE_BODY) {
                    //     block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                    //     block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::WHILE_COND;
                    //     block_exec_ordered[i]->next_block_index = block_exec_ordered[i]->parent->while_index;
                    // } else {
                    //     // 要跳转到上一级的if_end
                    //     block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                    //     block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::IF_END;
                    //     block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index;
                    // }
                }
                break;
            default: break;
        }
        // switch-case end
    }
}
// ok
void Function::TempBlockConnect()
{
    fmt::print("TempBlockConnect()\n");
    fmt::print("size: {}\n", block_exec_ordered.size());
    for (auto t : block_exec_ordered) {
        if(t==nullptr){
            fmt::print("[TempBlockConnect]: t is nullptr\n");
        }
        t->print();
    }
    for (auto t : block_exec_ordered) {
        if (t->is_last) {
            t->next_block = std::nullopt;
        }
        for (auto next : block_exec_ordered) {
            if(t==nullptr){
                fmt::print("[TempBlockConnect]: next is nullptr\n");
            }
            if (next->type == t->next_block_type) {
                switch (t->next_block_type) {
                    case TempBlock::TempBlockType::IF_THEN:
                    case TempBlock::TempBlockType::IF_ELSE:
                    case TempBlock::TempBlockType::IF_END:
                        if (next->if_index == t->next_block_index) {
                            t->next_block = next;
                            fmt::print("Next: ");
                            next->print();
                        }
                            
                        break;
                    case TempBlock::TempBlockType::WHILE_COND:
                    case TempBlock::TempBlockType::WHILE_BODY:
                    case TempBlock::TempBlockType::WHILE_END:
                        if (next->while_index == t->next_block_index) {
                            t->next_block = next;
                            fmt::print("Next: ");
                            next->print();
                        }
                            
                        break;
                    default:
                        break;
                }
            }
        }
    }
    fmt::print("***************************************************************\n");

    return;
}
// ok
void Function::CreateBlocks()
{
    bool debug = true;
    if (debug) {
        // output the block_exec_ordered
        fmt::print("[CreateBlocks]: size = {}\n", block_exec_ordered.size());
        for (int i = 0; i < (int)block_exec_ordered.size(); i++)
        {
            block_exec_ordered[i]->print();
        }
    }
    int else_index = 0;
    if (block_exec_ordered.size() == 0) return;


    for (auto t : block_exec_ordered) {
        if(t==nullptr){
            if(debug) fmt::print("[CreateBlocks]: t is nullptr\n");
        }
        if (t->type == TempBlock::TempBlockType::ENTRY) {
            if(debug) fmt::print("[CreateBlocks]: entry\n");
            BasicBlock *entry_block = BasicBlock::Create(this, &back());
            fmt::print("1\n");
            if(entry_block==nullptr){
                if(debug) fmt::print("[CreateBlocks]: entry_block is nullptr\n");
            }
            entry_block->setName("entry");
            fmt::print("2\n");
            t->basic_block = entry_block;
            BasicBlock::iterator it = entry_block->begin();
            fmt::print("3\n");
            for (int i = (int)t->instructions.size() - 1; i >= 0; i--) {
                it = t->instructions[i]->insertInto(entry_block, it);
            }
            fmt::print("4\n");
        }

        if (t->type == TempBlock::TempBlockType::IF_THEN) {
            if(debug) fmt::print("[CreateBlocks]: if_then\n");
            // if_then
            BasicBlock *block0 = BasicBlock::Create(this, &back());
            block0->setName(t->if_index==0?"if_then":"if_then."+std::to_string(t->if_index));
            t->basic_block = block0;

            BasicBlock::iterator it = block0->begin();
            for (int i = (int)t->instructions.size() - 1; i >= 0; i--) {
                it = t->instructions[i]->insertInto(block0, it);
            }

            // find out if_end
            for (auto t_ : block_exec_ordered) {
                if (t_->if_index == t->if_index && t_->type == TempBlock::TempBlockType::IF_END) {
                    if(debug) fmt::print("[CreateBlocks]: if_end\n");
                    BasicBlock *block1 = BasicBlock::Create(this, &back());
                    block1->setName(t->if_index==0?"if_end":"if_end."+std::to_string(t->if_index));
                    t_->basic_block = block1;

                    BasicBlock::iterator it = block1->begin();
                    for (int i = (int)t_->instructions.size() - 1; i >= 0; i--) {
                        it = t_->instructions[i]->insertInto(block1, it);
                    }
                }
            }
            // find out if_else
            for (auto t_ : block_exec_ordered) {
                if (t_->if_index == t->if_index && t_->type == TempBlock::TempBlockType::IF_ELSE) {
                    if(debug) fmt::print("[CreateBlocks]: if else\n");
                    BasicBlock *block2 = BasicBlock::Create(this, &back());
                    block2->setName(else_index==0?"if_else":"if_else."+std::to_string(else_index));
                    t_->basic_block = block2;
                    else_index++;

                    BasicBlock::iterator it = block2->begin();
                    for (int i = (int)t_->instructions.size() - 1; i >= 0; i--) {
                        it = t_->instructions[i]->insertInto(block2, it);
                    }
                }
            }
        }
    
        if (t->type == TempBlock::TempBlockType::WHILE_COND) {
            if(debug) fmt::print("[CreateBlocks]: while_cond\n");
            BasicBlock *block0 = BasicBlock::Create(this, &back());
            block0->setName(t->while_index==0?"while_cond":"while_cond."+std::to_string(t->while_index));
            t->basic_block = block0;

            BasicBlock::iterator it = block0->begin();
            for (int i = (int)t->instructions.size() - 1; i >= 0; i--) {
                it = t->instructions[i]->insertInto(block0, it);
            }
            // find out while body
            for (auto t_ : block_exec_ordered) {

                if (t_->while_index == t->while_index && t_->type == TempBlock::TempBlockType::WHILE_BODY) {
                    if(debug) fmt::print("[CreateBlocks]: while body\n");
                    BasicBlock *block1 = BasicBlock::Create(this, &back());
                    block1->setName(t->while_index==0?"while_body":"while_body."+std::to_string(t->while_index));
                    t_->basic_block = block1;

                    BasicBlock::iterator it = block1->begin();
                    for (int i = (int)t_->instructions.size() - 1; i >= 0; i--) {
                        it = t_->instructions[i]->insertInto(block1, it);
                    }
                }
            }
            // find out while end
            for (auto t_ : block_exec_ordered) {
                if (t_->while_index == t->while_index && t_->type == TempBlock::TempBlockType::WHILE_END) {
                    if(debug) fmt::print("[CreateBlocks]: while_end\n");
                    BasicBlock *block1 = BasicBlock::Create(this, &back());
                    block1->setName(t->while_index==0?"while_end":"while_end."+std::to_string(t->while_index));
                    t_->basic_block = block1;

                    BasicBlock::iterator it = block1->begin();
                    for (int i = (int)t_->instructions.size() - 1; i >= 0; i--) {
                        it = t_->instructions[i]->insertInto(block1, it);
                    }
                }
            }
        }
    }

}
// ok
void Function::BasicBlockConnect()
{
    bool debug = false;
    fmt::print("BasicBlockConnect()\n");

    // int index = 0;
    for (auto t : block_exec_ordered) {  
        if (!((t->next_block).has_value())) {
            // JumpInst::Create(&back(), t->basic_block);
            continue;
        }      
        if (t->branch == TempBlock::BranchType::JMP) {
            if(debug) fmt::print("[BasicBlockConnect]: which block?\n");
            t->print();
            if(debug) fmt::print("[BasicBlockConnect]: next block is?\n");
            t->next_block.value()->print();
            if(debug) fmt::print("[BasicBlockConnect]: jmp\n");
            if(t->basic_block == nullptr) fmt::print("basic_block is empty\n");
            if((t->next_block).value() == nullptr) fmt::print("next_block is empty\n");
            JumpInst::Create((t->next_block).value()->basic_block, t->basic_block);
            if(debug) fmt::print("[BasicBlockConnect]: create jump finished\n");
        }
        if (t->branch == TempBlock::BranchType::BR) {
            if(debug) fmt::print("[BasicBlockConnect]: br\n");
            BasicBlock *next_block1 = (t->next_block).value()->basic_block;
            BasicBlock *next_block2 = nullptr;
            if ((t->next_block).value()->type == TempBlock::TempBlockType::IF_THEN) {
                // IF_ELSE or IF_END
                bool found = false;
                for (auto t_ : block_exec_ordered) {
                    if (t_->type == TempBlock::TempBlockType::IF_ELSE && t_->if_index == (t->next_block).value()->if_index) {
                        next_block2 = t_->basic_block;
                        found = true;
                        break;
                    }
                }
                // not found -> IF_END
                if (!found) {
                    for (auto t_ : block_exec_ordered) {
                        if (t_->type == TempBlock::TempBlockType::IF_END && t_->if_index == (t->next_block).value()->if_index) {
                            next_block2 = t_->basic_block;
                            found = true;
                            break;
                        }
                    }
                }
                if (!found) {
                    if(debug) fmt::print("[BasicBlockConnect]: Can not found next block\n");
                }
            }

            
            else if ((t->next_block).value()->type == TempBlock::TempBlockType::WHILE_BODY) {
                // While_end
                bool found = false;
                for (auto t_ : block_exec_ordered) {
                    if (t_->type == TempBlock::TempBlockType::WHILE_END && t_->while_index == (t->next_block).value()->while_index) {
                        next_block2 = t_->basic_block;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    if(debug) fmt::print("[BasicBlockConnect]: Can not found next block\n");
                }
            }
            
            if (next_block1 == nullptr) fmt::print("next_block1 empty\n");
            if (next_block2 == nullptr) fmt::print("next_block2 empty\n");
            if (t->condition == nullptr) fmt::print("condition empty\n");

            BranchInst::Create(next_block1, next_block2, t->condition, t->basic_block);
            if(debug) fmt::print("[BasicBlockConnect]: br_end\n");
        }
        
    }
    fmt::print("[BasicBlockConnect]: out\n");
}

Value* Function::PostOrderTraversal(NodePtr root, TempBlock *parent, bool isVal) {
    fmt::print("PostOrderTraversal()\n");
    bool debug = true;
    if(root == nullptr){
        fmt::print("[PostOrderTraversal]: root is nullptr\n");
        return nullptr;
    }
    // leaf
    if (root->as<intExp*>() != nullptr || root->as<LVal*>() != nullptr) {
        // constant
        if (auto constant = root->as<intExp*>()) {
            if(debug) fmt::print("[PostOrderTraversal]: constant\n");
            return ConstantInt::Create((std::uint32_t)constant->value);
        }
        // variable
        if (auto var = root->as<LVal*>()) {
            if(debug) fmt::print("[PostOrderTraversal]: LVal\n");
            if (isVal) {
                return findVariable(root, parent);
            } else {
                return findVariableAddr(root, parent);
            }
        }
    }
    // x = f(1);

    if (auto t = root->as<addExp*>()) {
        fmt::print("addExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);

        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Add, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<subExp*>()) {
        if(debug) fmt::print("[PostOrderTraversal]: subExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst = nullptr;
        inst = BinaryInst::Create(Instruction::BinaryOps::Sub, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<mulExp*>()) {
        fmt::print("mulExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Mul, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<divExp*>()) {
        fmt::print("divExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Div, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<modExp*>()) {
        fmt::print("modExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Mod, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<andExp*>()) {
        fmt::print("andExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::And, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<orExp*>()) {
        fmt::print("orExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Or, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<lessExp*>()) {
        fmt::print("lessExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Lt, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<greatExp*>()) {
        fmt::print("greatExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Gt, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<leqExp*>()) {
        fmt::print("leqExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Le, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<geqExp*>()) {
        fmt::print("geqExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Ge, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<eqExp*>()) {
        fmt::print("eq\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Eq, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<neqExp*>()) {
        fmt::print("neqExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = PostOrderTraversal(t->rhs, parent, true);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Ne, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (root->as<assignExp*>()) { // auto t = root->as<assignExp*>()
        if(debug) fmt::print("[PostOrderTraversal]: assignExp is not implemented. \n");
        // 还剩assignExp没有处理
    }


    // 单目运算符
    else if (auto t = root->as<negExp*>()){
        fmt::print("negExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = ConstantInt::Create((std::uint32_t)0);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Sub, val2, val1, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<notExp*>()){
        fmt::print("notExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent, true);
        auto val2 = ConstantInt::Create((std::uint32_t)0);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Eq, val2, val1, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<posExp*>()){
        fmt::print("posExp\n");
        return PostOrderTraversal(t->lhs, parent, true);
    } else if (auto t = root->as<funcallExp*>()){
        fmt::print("funcallExp name = {}\n", t->name);
        CallInst* inst = nullptr;
        Function *f = getParent()->getFunction(t->name);
        std::vector<Value *> Args;
        int index = 0;
        for (auto param : t->params) {
            if (f->getArg(index)->getType() == Type::getIntegerTy()) {
                auto arg = PostOrderTraversal(param, parent, true);
                Args.push_back(arg);
            } else {
                auto arg = PostOrderTraversal(param, parent, false);
                Args.push_back(arg);
            }
        }
        inst = CallInst::Create(f, Args);
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<assignExp*>()) {
        fmt::print("assignExp is not implemented. \n");
        // 还剩assignExp没有处理
    }
}

Value *Function::findVariableAddr(NodePtr variable, TempBlock* root)
{
    fmt::print("findVariable()\n");
    bool debug = true;
    bool found = false;

    if (auto varDef = variable->as<VarDef*>()) {
        fmt::print("findVariableAddr: varDef\n");
        fmt::print("find in the global variable list\n");
        for (auto &t : getParent()->getGlobalList()) {
            if (t.getName() == varDef->name) {
                if (varDef->isArray==false) {
                    fmt::print("integer\n");
                    return &t;
                }
            }
        }
    }



    auto var = variable->as<LVal*>();
//    if (root == nullptr) {
//        assert("findVariable(): root is empty\n");
//    }

    // 在tempBlock中找匹配variable的变量
    TempBlock *temp_block = root;
    while (temp_block != nullptr) {
        // if(debug) fmt::print("[findVariable]: temp_block->instructions.size = {}\n",temp_block->instructions.size());
        for (auto t : temp_block->instructions) {
            // if(debug) fmt::print("[findVariable]: loop\n");
            if (AllocaInst::classof(t) && t->getName() == var->name) {
                if(debug) fmt::print("[findVariable]: find out a local variable {}\n", var->name);
                // 数组变量
                if (var->isArray) {
                    if(debug) fmt::print("[findVariable]: array name = {}\n",var->name);
                    OffsetInst *offset = nullptr;
                    std::vector<std::optional<std::size_t> > Bounds = ((AllocaInst*)t)->getBounds();
                    if(debug) fmt::print("[findVariable]: Bounds.size = {}\n",Bounds.size());
                    std::vector<Value *> Indices;
                    if(debug) fmt::print("[findVariable]: var->position.size = {}\n",var->position.size());
                    for (auto exp : var->position) {
                        Indices.push_back(PostOrderTraversal(exp, root, true));
                    }
                    // Indices的维数不足时需要补0
                    if (Indices.size() < Bounds.size()) {
                        fmt::print("补！\n");
                        for (int j = Indices.size(); j < Bounds.size(); j++) {
                            Indices.push_back(ConstantInt::Create(0));
                        }
                    }
                    if(debug) fmt::print("[findVariable]: start to create offset\n");
                    offset = OffsetInst::Create(Type::getIntegerTy(), t, Indices, Bounds);
                    root->instructions.push_back(offset);
                    // LoadInst *load_inst = LoadInst::Create(offset);
                    // root->instructions.push_back(load_inst);
                    found = true;
                    return offset;
                    break;
                } else { // 整型
                    if(debug) fmt::print("[findVariable]: single integer var name = {}\n",var->name);
                    // LoadInst *load_inst = LoadInst::Create(t);
                    // root->instructions.push_back(load_inst);
                    found = true;
                    return t;
                    break;
                }


            }

        }
        // 说明不在这一层
        temp_block = temp_block->parent;
    }
    // 找参数列表

    if (!found) {
        if(debug) fmt::print("[findVariable]: find the variable in the argument list\n");
        for (int i = 0; i < getNumParams(); i++) {
            if (var->name == getArg(i)->getName()) {
                if(debug) fmt::print("[findVariable]: find out {} in the arg\n", var->name);
                if (getArg(i)->getType() == Type::getIntegerTy()) {
                    // if (!var->isArray) {
                    fmt::print("[findVariable]: find out an integer variable in the arg\n");
                    fmt::print("i={}\n", i);
                    if (getArg(i)->pointer == nullptr) {
                        fmt::print("empty\n");
                    }
                    // LoadInst *load_inst = LoadInst::Create(getArg(i)->pointer);
                    // fmt::print("out1\n");
                    // root->instructions.push_back(load_inst);
                    fmt::print("out\n");
                    return getArg(i)->pointer;
                } else {
                    fmt::print("[findVariable]: find out an array variable in the arg\n");
                    OffsetInst *offset;
                    // std::vector<std::size_t> Bounds = ((AllocaInst*)&t)->getBounds();
                    std::vector<std::optional<std::size_t>> Bounds;
                    // Bound 可以是空
                    for (auto dimension : getArg(i)->dimensions) {
                        fmt::print("{}\n", "di: " + std::to_string(dimension));
                        if (dimension == -1) {
                            fmt::print("empty dimension\n");
                            Bounds.push_back(std::nullopt);
                        } else {
                            Bounds.push_back(dimension);
                        }
                    }
                    std::vector<Value *> Indices;
                    for (auto exp : var->position) {
                        Indices.push_back(PostOrderTraversal(exp, root, true));
                    }
                    if (Indices.size() == Bounds.size()) {
                        fmt::print("eq\n");
                    } else {
                        fmt::print("{}\n", std::to_string(Indices.size()));
                        fmt::print("{}\n", std::to_string(Bounds.size()));
                        fmt::print("neq\n");
                    }
                    // Indices的维数不足时需要补0
                    if (Indices.size() < Bounds.size()) {
                        fmt::print("补！\n");
                        for (int j = Indices.size(); j < Bounds.size(); j++) {
                            Indices.push_back(ConstantInt::Create(0));
                        }
                    }
                    fmt::print("start to create offset\n");

                    fmt::print("create offset and load successfully\n");

                    offset = OffsetInst::Create(Type::getIntegerTy(), getArg(i), Indices, Bounds);
                    root->instructions.push_back(offset);
                    // LoadInst *load_inst = LoadInst::Create(offset);
                    // root->instructions.push_back(load_inst);
                    return offset;

                }



                found = true;


                break;
            }
        }
    }
    // 全局变量中找
    if (!found) {
        fmt::print("find in the global variable list\n");
        for (auto &t : getParent()->getGlobalList()) {
            if (t.getName() == var->name) {
                if (var->isArray) {
                    fmt::print("isArray\n");
                    OffsetInst *offset;
                    std::vector<std::optional<std::size_t> > Bounds = t.getBounds();
                    std::vector<Value *> Indices;
                    for (auto exp : var->position) {
                        Indices.push_back(PostOrderTraversal(exp, root, true));
                    }
                    fmt::print("start to create offset\n");

                    offset = OffsetInst::Create(Type::getIntegerTy(), &t, Indices, Bounds);
                    root->instructions.push_back(offset);
                    // LoadInst *load_inst = LoadInst::Create(offset);
                    // root->instructions.push_back(load_inst);
                    return offset;
                } else {
                    fmt::print("integer\n");
                    // LoadInst *load_inst = LoadInst::Create(&t);
                    // root->instructions.push_back(load_inst);
                    return &t;
                }
            }
        }
    }
    if (!found) {
        assert("can't find the variable anywhere\n");
    }
}

Value * Function::findVariable(NodePtr variable, TempBlock* root)
{
    fmt::print("findVariable()\n");    
    bool debug = true;
    bool found = false;
    auto var = variable->as<LVal*>();
    if (root == nullptr) {
        assert("findVariable(): root is empty\n");
    }
    
    // 在tempBlock中找匹配variable的变量
    TempBlock *temp_block = root;
    while (temp_block != nullptr) {
        // if(debug) fmt::print("[findVariable]: temp_block->instructions.size = {}\n",temp_block->instructions.size());
        for (auto t : temp_block->instructions) {
            // if(debug) fmt::print("[findVariable]: loop\n");
            if (AllocaInst::classof(t) && t->getName() == var->name) {
                if(debug) fmt::print("[findVariable]: find out a local variable {}\n", var->name);
                // 数组变量
                if (var->isArray) {
                    if(debug) fmt::print("[findVariable]: array name = {}\n",var->name);
                    OffsetInst *offset = nullptr;
                    std::vector<std::optional<std::size_t> > Bounds = ((AllocaInst*)t)->getBounds();
                    if(debug) fmt::print("[findVariable]: Bounds.size = {}\n",Bounds.size());
                    std::vector<Value *> Indices;
                    if(debug) fmt::print("[findVariable]: var->position.size = {}\n",var->position.size());
                    for (auto exp : var->position) {
                        Indices.push_back(PostOrderTraversal(exp, root, true));
                    }
                    // Indices的维数不足时需要补0
                    if (Indices.size() < Bounds.size()) {
                        fmt::print("补！\n");
                        for (int j = Indices.size(); j < Bounds.size(); j++) {
                            Indices.push_back(ConstantInt::Create(0));
                        }
                    }
                    if(debug) fmt::print("[findVariable]: start to create offset\n");
                    offset = OffsetInst::Create(Type::getIntegerTy(), t, Indices, Bounds);
                    root->instructions.push_back(offset);
                    LoadInst *load_inst = LoadInst::Create(offset);
                    root->instructions.push_back(load_inst);
                    found = true;
                    return load_inst;
                    break;
                } else { // 整型
                    if(debug) fmt::print("[findVariable]: single integer var name = {}\n",var->name);
                    LoadInst *load_inst = LoadInst::Create(t);
                    root->instructions.push_back(load_inst);
                    found = true;
                    return load_inst;
                    break;
                }
                
                
            }
            
        }
        // 说明不在这一层
        temp_block = temp_block->parent;
    }
    // 找参数列表
    
    if (!found) {
        if(debug) fmt::print("[findVariable]: find the variable in the argument list\n");
        for (int i = 0; i < getNumParams(); i++) {
            if (var->name == getArg(i)->getName()) {
                if(debug) fmt::print("[findVariable]: find out {} in the arg\n", var->name);
                if (getArg(i)->getType() == Type::getIntegerTy()) {
                // if (!var->isArray) {
                    fmt::print("[findVariable]: find out an integer variable in the arg\n");
                    fmt::print("i={}\n", i);
                    if (getArg(i)->pointer == nullptr) {
                        fmt::print("empty\n");
                    }
                    LoadInst *load_inst = LoadInst::Create(getArg(i)->pointer);
                    // fmt::print("out1\n");
                    root->instructions.push_back(load_inst);
                    fmt::print("out\n");
                    return load_inst;
                } else {
                    fmt::print("[findVariable]: find out an array variable in the arg\n");
                    OffsetInst *offset;
                    // std::vector<std::size_t> Bounds = ((AllocaInst*)&t)->getBounds();
                    std::vector<std::optional<std::size_t>> Bounds;
                    // Bound 可以是空
                    for (auto dimension : getArg(i)->dimensions) {
                        if(debug) fmt::print("[findVariable]:{}\n", "di: " + std::to_string(dimension));
                        if (dimension == -1) {
                            if(debug) fmt::print("[findVariable]:empty dimension\n");
                            Bounds.push_back(std::nullopt);
                        } else {
                            Bounds.push_back(dimension);
                        }
                    }
                    std::vector<Value *> Indices;
                    for (auto exp : var->position) {
                        Indices.push_back(PostOrderTraversal(exp, root, true));
                    }
                    if (Indices.size() == Bounds.size()) {
                        fmt::print("eq\n"); 
                    } else {
                        fmt::print("{}\n", std::to_string(Indices.size()));
                        fmt::print("{}\n", std::to_string(Bounds.size()));
                        fmt::print("neq\n");
                    }
                    // Indices的维数不足时需要补0
                    if (Indices.size() < Bounds.size()) {
                        fmt::print("补！\n");
                        for (int j = Indices.size(); j < Bounds.size(); j++) {
                            Indices.push_back(ConstantInt::Create(0));
                        }
                    }
                    fmt::print("start to create offset\n");
                    
                    fmt::print("create offset and load successfully\n");
                    
                    offset = OffsetInst::Create(Type::getIntegerTy(), getArg(i), Indices, Bounds);
                    root->instructions.push_back(offset);
                    LoadInst *load_inst = LoadInst::Create(offset);
                    root->instructions.push_back(load_inst);
                    return load_inst;
                    
                }
                    
                
                
                found = true;
                
                
                break;
            }
        }
    }
    // 全局变量中找
    if (!found) {
        fmt::print("find in the global variable list\n");
        for (auto &t : getParent()->getGlobalList()) {
            if (t.getName() == var->name) {
                if (var->isArray) {
                    fmt::print("isArray\n");
                    OffsetInst *offset;
                    std::vector<std::optional<std::size_t> > Bounds = t.getBounds();
                    std::vector<Value *> Indices;
                    for (auto exp : var->position) {
                        Indices.push_back(PostOrderTraversal(exp, root, true));
                    }
                    fmt::print("start to create offset\n");
                    
                    offset = OffsetInst::Create(Type::getIntegerTy(), &t, Indices, Bounds);
                    root->instructions.push_back(offset);
                    LoadInst *load_inst = LoadInst::Create(offset);
                    root->instructions.push_back(load_inst);
                    return load_inst;
                } else {
                    fmt::print("integer\n");
                        LoadInst *load_inst = LoadInst::Create(&t);
                        root->instructions.push_back(load_inst);
                        return load_inst;
                }
            }
        }
    }
    if (!found) {
        assert("can't find the variable anywhere\n");
    }
}



/*
跳转规律：
    1. 打印出来的永远都是 if_then -> if_end -> if_else的结构
    2. if_then如果后面紧跟if_else，说明内部没有再分裂出block，需要用jmp跳转到if_end;
    if_then如果后面紧跟if_then，说明内部再分裂出了if，需要用br跳转到if_then/if_else;
    3. if_end的跳转规则：跳转到当前depth的下一个block，若没有，则跳转到上一层的下一个block
    4. if_else与if_then的跳转规则保持一致
    4. 需要引入深度depth的概念
    4. br语句的两个地址，一定是if_then和if_else成对出现(如果没有else会直接去end)
*/