#include "ir/ir.h"
#include "ir/type.h"
#include <string>

void Module::genGlobalList(NodePtr root)
{
    fmt::print("genGlobalList()\n");
    bool debug = false;
    if(root == nullptr) return;
    if(auto *comp_unit = root->as<CompUnit*>()) {
        if(debug) fmt::print("comp_unit->all.size() = {}\n",comp_unit->all.size());
        for(size_t i = 0; i < comp_unit->all.size(); i++) {
            auto decl = comp_unit->all[i]->as<Decl*>();
            if (decl == nullptr) 
                continue;
            std::vector<NodePtr> varDefs = decl->VarDecl->as<VarDecl*>()->VarDefs;
            if(debug) 
                fmt::print("varDefs.size() = {}\n",varDefs.size());
            for(size_t j = 0; j < varDefs.size(); j++) {
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

void Module::declExLinkFunction()
{
    fmt::print("declExLinkFunction()\n");
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
    auto f_getint = Function::Create(FTy_getint, true, "getint", this);
    // getch
    Type *Result_getch = Type::getIntegerTy();
    std::vector<Type *> Params_getch;  // empty
    FunctionType *FTy_getch = new FunctionType(Result_getch, Params_getch);
    auto f_getch = Function::Create(FTy_getch, true, "getch", this);
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
    fmt::print("genFuncList()\n");
    // Step1: 外部库函数的声明
    declExLinkFunction();
    
    // Step2: 内部函数的声明
    if(root == nullptr) return;
    if(auto *comp_unit = root->as<CompUnit*>()) {
        for(size_t i = 0; i < comp_unit->all.size(); i++){
            auto funcDef = comp_unit->all[i]->as<FuncDef*>();
            if (funcDef == nullptr)
                continue;
            // ----------------------------------------------------
            fmt::print("Create a local function\n");
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
            // fmt::print("1\n");
            f->AnalysisBlockOrder();
            // fmt::print("2\n");
            f->TempBlockConnect();
            // fmt::print("3\n");
            f->CreateBlocks();
            // fmt::print("4\n");
            f->BasicBlockConnect();
            // fmt::print("5\n");
            f->generateReturnBlock();
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
    // 分配参数的空间
    for (int i = 0; i < getNumParams(); i++) {
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
    fmt::print("generateTempBlock: \n");
    // generate Entry block
    if (parent == nullptr) {
        parent = generateEntryBlock();
    }
    // return;
    
    for (auto blockItem : rootBlock->BlockItems) {
        fmt::print("blockItem:\n");
        auto statement = blockItem->as<BlockItem*>()->Stmt;
        auto declare = blockItem->as<BlockItem*>()->Decl;
        // decl语句
        if (statement == nullptr) {
            fmt::print("find out a declare statement\n");
            if (auto varDeclStmt = declare->as<Decl*>()->VarDecl->as<VarDecl*>()) {
                std::vector<NodePtr> varDefs = varDeclStmt->VarDefs;
                for (auto t : varDefs) {
                    fmt::print("createLocalVariable for {}\n", t->as<VarDef*>()->name);
                    auto load_instr = createLocalVariable(t->as<VarDef*>(), parent); // alloca
                    // added
                    if (t->as<VarDef*>()->initialValue != nullptr) {
                        // assign statement
                        fmt::print("assign initial value for the local variable\n");
                        Value *rv = PostOrderTraversal(t->as<VarDef*>()->initialValue, parent);
                        Value *lv = load_instr;
                        StoreInst *store = StoreInst::Create(rv, lv);
                        parent->instructions.push_back(store);

                    }
                }
            }
            continue;
        }   
            
        // assign语句
        if (auto assignStmt = statement->as<AssignStmt*>()) {
            Value * lv;
            Value * rv;
            fmt::print("find out an assign: \n");
            
            rv = PostOrderTraversal(assignStmt->Exp, parent);
            fmt::print("rVal generate successfully\n");
            lv = findVariable(assignStmt->LVal->as<LVal*>(), parent);
            fmt::print("lVal generate successfully\n");
            
            StoreInst *store = StoreInst::Create(rv, lv);
            parent->instructions.push_back(store);

            fmt::print("create success\n");
            
            continue;
        }
        
        // while语句
        if (auto whileStmt = statement->as<WhileStmt*>()) {
            fmt::print("find out a while: \n");
            BlockStmt *then_block_stmt = whileStmt->then->as<BlockStmt*>();
            Block *then_block = then_block_stmt->Block->as<Block*>();
            
            // while_cond
            TempBlock *while_cond_block = new TempBlock(
                    TempBlock::TempBlockType::WHILE_COND, parent, cur_depth, 
                    if_index, else_index, while_index);
            block_exec_ordered.push_back(while_cond_block);
            while_index++;
            
            // condition
            Value *conditionVal = PostOrderTraversal(whileStmt->condition, while_cond_block);
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
            fmt::print("while : parent changed\n");
        }

        // expStmt语句
        if (auto expStmt = statement->as<ExpStmt*>()) {
            fmt::print("find out an expStmt\n");
            PostOrderTraversal(expStmt->Exp, parent);
        }
        
        // return语句
        if (auto returnStmt = statement->as<ReturnStmt*>()) {
            fmt::print("return---------------------------------------------------------------------\n");
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

                rv = PostOrderTraversal(returnStmt->result, parent);


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
            fmt::print("find out an if: \n");
            if(ifStmt->then==nullptr){
                fmt::print("ifStmt->then==nullptr\n");
            }
            if(ifStmt->then->as<BlockStmt*>()==nullptr){
                fmt::print("ifStmt->then->as<BlockStmt*>()==nullptr\n");
            }
            BlockStmt *then_block_stmt = ifStmt->then->as<BlockStmt*>();
            if (then_block_stmt == nullptr) {
                then_block_stmt = new BlockStmt();
                then_block_stmt->Block = new Block();
                auto blockItem = new BlockItem();
                blockItem->Stmt = ifStmt->then;
                then_block_stmt->Block->as<Block*>()->BlockItems.emplace_back(blockItem);
            }
            if(then_block_stmt->Block==nullptr){
                fmt::print("then_block_stmt->Block==nullptr\n");
            }
            if(then_block_stmt->Block->as<Block*>()==nullptr){
                fmt::print("then_block_stmt->Block->as<Block*>()==nullptr\n");
            }
            Block *then_block = then_block_stmt->Block->as<Block*>();
            fmt::print("0\n");
            // condition
            Value *conditionVal = PostOrderTraversal(ifStmt->condition, parent);
            if (conditionVal == nullptr) 
                assert("conditionVal is empty\n");
            parent->condition = conditionVal;
            fmt::print("1\n");
            TempBlock *if_then_block = new TempBlock(
                    TempBlock::TempBlockType::IF_THEN, parent, cur_depth, 
                    if_index, else_index, while_index);
            block_exec_ordered.push_back(if_then_block);
            fmt::print("2\n");
            if_index++;

            cur_depth++;
            generateTempBlock(then_block, if_then_block);
            cur_depth--;
            fmt::print("3\n");
            // matched if
            if(ifStmt->matched) {
                fmt::print("4\n");
                // if_else
                TempBlock *if_else_block;
                if_else_block = new TempBlock( 
                    TempBlock::TempBlockType::IF_ELSE, parent, cur_depth, 
                    if_then_block->if_index, if_then_block->else_index, while_index);
                block_exec_ordered.push_back(if_else_block);
                fmt::print("5\n");
                Block *els_block;
                // 三种可能性 IfStmt / BlockStmt / WhileStmt
                if (BlockStmt* els_block_stmt = ifStmt->els->as<BlockStmt*>()) {
                    fmt::print("the last else \n");
                    els_block = els_block_stmt->Block->as<Block*>();

                } else {
                    // 不是一个block，那就创建一个
                    fmt::print("create an els_block\n");
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
            fmt::print("6\n");
            
            TempBlock *if_end_block = new TempBlock(
                    TempBlock::TempBlockType::IF_END, parent, cur_depth, 
                    if_then_block->if_index, if_then_block->else_index, while_index);
            block_exec_ordered.push_back(if_end_block);
            fmt::print("7\n");
            // 需要修改parent
            parent = if_end_block;
            fmt::print("parent changed\n");
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
    for (size_t i = 0; i < block_exec_ordered.size(); i++)
    {
        if (i == block_exec_ordered.size()-1) {
            block_exec_ordered[i]->is_last = true;
            if(debug) fmt::print("[AnalysisBlockOrder]: is last block\n");
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
                fmt::print("if_then\n");
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
                    // 内部没有分裂出新的block，跳转到end
                    block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::IF_END;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i]->if_index;
                }
                break;
            case TempBlock::TempBlockType::IF_ELSE:
                if (block_exec_ordered[i+1]->depth > block_exec_ordered[i]->depth) {
                    
                    fmt::print("split new block\n");
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
                fmt::print("while_end\n");
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
                    TempBlock *temp_block;
                    temp_block = block_exec_ordered[i]->parent;
                    fmt::print("[AnalysisBlockOrder]: while_end jump to the parent\n");
                    while(temp_block != nullptr) {
                        fmt::print("loop\n");
                        if (temp_block->depth < block_exec_ordered[i]->depth && 
                            temp_block->type != TempBlock::TempBlockType::IF_END && 
                            temp_block->type != TempBlock::TempBlockType::WHILE_END) {
                            fmt::print("enter if\n");
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
                        
                        fmt::print("loop end\n");
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
        }
        // switch-case end
    }
}
// ok
void Function::TempBlockConnect()
{
    fmt::print("TempBlockConnect()\n");
    fmt::print("size: {}\n", std::to_string(block_exec_ordered.size()));
    for (auto t : block_exec_ordered) {
        t->print();
    }
    for (auto t : block_exec_ordered) {
        if (t->is_last) {
            t->next_block = std::nullopt;
        }
        for (auto next : block_exec_ordered) {
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
        fmt::print("{}\n", "size: " + std::to_string(block_exec_ordered.size()));
        
        for (int i = 0; i < block_exec_ordered.size(); i++)
        {
            block_exec_ordered[i]->print();
        }
    }
    int else_index = 0;
    if ((int)block_exec_ordered.size() == 0) return;


    for (auto t : block_exec_ordered) {
        
        if (t->type == TempBlock::TempBlockType::ENTRY) {
            BasicBlock *entry_block = BasicBlock::Create(this, &back());
            entry_block->setName("entry");
            t->basic_block = entry_block;
            BasicBlock::iterator it = entry_block->begin();
            for (int i = (int)t->instructions.size() - 1; i >= 0; i--) {
                it = t->instructions[i]->insertInto(entry_block, it);
            }
        }

        if (t->type == TempBlock::TempBlockType::IF_THEN) {
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
    fmt::print("BasicBlockConnect()\n");
    int index = 0;
    for (auto t : block_exec_ordered) {  
        if (!((t->next_block).has_value())) {
            JumpInst::Create(&back(), t->basic_block);
            continue;
        }      
        if (t->branch == TempBlock::BranchType::JMP) {
            fmt::print("which block?\n");
            t->print();
            fmt::print("next block is?\n");
            t->next_block.value()->print();
            fmt::print("jmp\n");
            if(t->basic_block == nullptr) fmt::print("basic_block is empty\n");
            if((t->next_block).value() == nullptr) fmt::print("next_block is empty\n");
            JumpInst::Create((t->next_block).value()->basic_block, t->basic_block);
            fmt::print("create jump finished\n");
        }
        if (t->branch == TempBlock::BranchType::BR) {
            fmt::print("br\n");
            BasicBlock *next_block1 = (t->next_block).value()->basic_block;
            BasicBlock *next_block2;
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
                    fmt::print("Can not found the next block\n");
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
                    fmt::print("Can not found the next block\n");
                }
            }
            
            if (next_block1 == nullptr) fmt::print("next_block1 empty\n");
            if (next_block2 == nullptr) fmt::print("next_block2 empty\n");
            if (t->condition == nullptr) fmt::print("condition empty\n");

            BranchInst::Create(next_block1, next_block2, t->condition, t->basic_block);
            fmt::print("br_end\n");
        }
        
    }
    fmt::print("out\n");
}

Value* Function::PostOrderTraversal(NodePtr root, TempBlock *parent) {
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
            return findVariable(root, parent);
        }
    }

    if (auto t = root->as<addExp*>()) {
        fmt::print("addExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);

        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Add, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<subExp*>()) {
        fmt::print("subExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Sub, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<mulExp*>()) {
        fmt::print("mulExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Mul, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<divExp*>()) {
        fmt::print("divExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Div, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<modExp*>()) {
        fmt::print("modExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Mod, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<andExp*>()) {
        fmt::print("andExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::And, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<orExp*>()) {
        fmt::print("orExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Or, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<lessExp*>()) {
        fmt::print("lessExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Lt, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<greatExp*>()) {
        fmt::print("greatExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::BinaryOps::Gt, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<leqExp*>()) {
        fmt::print("leqExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Le, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<geqExp*>()) {
        fmt::print("geqExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Ge, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<eqExp*>()) {
        fmt::print("eq\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Eq, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<neqExp*>()) {
        fmt::print("neqExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = PostOrderTraversal(t->rhs, parent);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Ne, val1, val2, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    }


    // 单目运算符
    else if (auto t = root->as<negExp*>()){
        fmt::print("negExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = ConstantInt::Create((std::uint32_t)0);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Sub, val2, val1, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<notExp*>()){
        fmt::print("notExp\n");
        auto val1 = PostOrderTraversal(t->lhs, parent);
        auto val2 = ConstantInt::Create((std::uint32_t)0);
        BinaryInst* inst;
        inst = BinaryInst::Create(Instruction::Eq, val2, val1, Type::getIntegerTy());
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<posExp*>()){
        fmt::print("posExp\n");
        return PostOrderTraversal(t->lhs, parent);
    } else if (auto t = root->as<funcallExp*>()){
        fmt::print("funcallExp name = {}\n", t->name);
        CallInst* inst = nullptr;
        Function *f = getParent()->getFunction(t->name);
        std::vector<Value *> Args;
        for (auto param : t->params) {
            auto arg = PostOrderTraversal(param, parent);
            Args.push_back(arg);
        }
        inst = CallInst::Create(f, Args);
        parent->instructions.push_back(inst);
        return inst;
    } else if (auto t = root->as<assignExp*>()) {
        fmt::print("assignExp is not implemented. \n");
        // 还剩assignExp没有处理
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
                        Indices.push_back(PostOrderTraversal(exp, root));
                    }
                    // Indices的维数不足时需要补0
                    if (Indices.size() < Bounds.size()) {
                        fmt::print("补！\n");
                        for (size_t j = Indices.size(); j < Bounds.size(); j++) {
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
                        Indices.push_back(PostOrderTraversal(exp, root));
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
                        for (size_t j = Indices.size(); j < Bounds.size(); j++) {
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
                        Indices.push_back(PostOrderTraversal(exp, root));
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


// Value * Function::findEntryVariable(TempBlock* root, NodePtr variable)
// {
//     bool debug = true;
//     if(debug) fmt::print("[findEntryVariable]:\n");
//     bool found = false;
//     auto var = variable->as<LVal*>();
//     List<Instruction>& instr_list = getEntryBlock().getInstList();
//     for (Instruction& tt : instr_list) {
//         Instruction *t = &tt;
//         if (AllocaInst::classof(t) && t->getName() == var->name) {
//             fmt::print("[findEntryVariable]: find out a local variable\n");
//             // 数组变量
//             if (var->isArray) {
//                 fmt::print("isArray\n");
//                 OffsetInst *offset;
//                 std::vector<std::optional<std::size_t>> Bounds = ((AllocaInst*)&t)->getBounds();
//                 std::vector<Value *> Indices;
//                 for (auto exp : var->position) {
//                     Indices.push_back(PostOrderTraversal(exp, root));
//                 }
//                 // Indices的维数不足时需要补0
//                 if (Indices.size() < Bounds.size()) {
//                     fmt::print("补！\n");
//                     for (size_t j = Indices.size(); j < Bounds.size(); j++) {
//                         Indices.push_back(ConstantInt::Create(0));
//                     }
//                 }
//                 fmt::print("start to create offset\n");
//                 if (root == nullptr) {
//                     offset = OffsetInst::Create(Type::getIntegerTy(), t, Indices, Bounds, &getEntryBlock());
//                     LoadInst *load_inst = LoadInst::Create(offset, &getEntryBlock());
//                     return load_inst;
//                 } else {
//                     offset = OffsetInst::Create(Type::getIntegerTy(), t, Indices, Bounds);
//                     root->instructions.push_back(offset);
//                     LoadInst *load_inst = LoadInst::Create(offset);
//                     root->instructions.push_back(load_inst);
//                     return load_inst;
//                 }
                
//                 found = true;
                
//                 break;
//             } else { // 整型
//                 fmt::print("integer\n");
//                 if (root == nullptr) {
//                     LoadInst *load_inst = LoadInst::Create(t, &getEntryBlock());
//                     return load_inst;
//                 } else {
//                     LoadInst *load_inst = LoadInst::Create(t);
//                     root->instructions.push_back(load_inst);
                    
//                     return load_inst;
//                 }
//                 found = true;
//                 break;
//             }
            
            
//         }
//     }
//     if (!found) return nullptr;
// }



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