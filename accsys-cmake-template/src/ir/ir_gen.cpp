#include "ir/ir.h"
#include "ir/type.h"
#include <string>
#include <iostream>

void Module::genGlobalList(NodePtr root)
{
    // Module * M = new Module;
    // 可调print完成打印
    // 如何生成GlobalVariableList? -> List<GlobalVariable>

    // 在语法树的哪里？
    // CompUnit里的all存放了所有的儿子 -> std::vector<NodePtr> all;
    // all里面匹配Decl*类型的，-> NodePtr VarDecl -> VarDefs

    if(root == nullptr) return;
    if(auto *comp_unit = root->as<CompUnit*>()){
        for(size_t i = 0; i < comp_unit->all.size(); i++){
            auto decl = comp_unit->all[i]->as<Decl*>();
            if(decl == nullptr) {
                // GenerateVarSymTable(decl, currentTable, funcTable, funcName);
                continue;
            }
            std::vector<NodePtr> varDefs = decl->VarDecl->as<VarDecl*>()->VarDefs;
            for(size_t j = 0; j < varDefs.size(); j++){
                // static GlobalVariable *Create(Type *EleTy, std::size_t NumElements = 1, bool ExternalLinkage = false,
                //                  std::string_view Name = "", Module *M = nullptr);
                // 找到type  -> 一个静态的成员，通过接口访问
                // 找到name  -> varDef->name
                // NumElements -> 
                // ExternalLinkage -> false
                auto varDef = varDefs[j]->as<VarDef*>();
                if(varDef != nullptr) {
                    // 数组全局变量
                    if(varDef->isArray) {
                        // 把数组维数的乘积算出来
                        int dim_product = 1;
                        for(auto k : varDef->dimensions){
                            dim_product *= k;
                        }
                        GlobalVariable::Create(Type::getIntegerTy(), dim_product, false, varDef->name, this);
                    } else {    // 单个变量
                        GlobalVariable::Create(Type::getIntegerTy(), 1, false, varDef->name, this);
                    }
                    // M->getGlobalList.
                    // 不用在这里维护，new的时候已经做好了
                }
            }
        }
    }

}


void Module::genFuncList(NodePtr root)
{
    // 用同样的方式找到funcDef
    // FuncDef中有name, ReturnType, argList
    // argList -> vector<NodePtr>
    // NodePtr -> FuncFParam
    // FuncParam中有什么？ -> name, isArray, dimensions -> 这和global的处理方式一致


    // Step1: 库函数需不需要放到FuncList中？ -> 待定
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
    // Step2



    if(root == nullptr) return;
    if(auto *comp_unit = root->as<CompUnit*>()){
        for(size_t i = 0; i < comp_unit->all.size(); i++){
            auto funcDef = comp_unit->all[i]->as<FuncDef*>();
            if(funcDef == nullptr) {
                // GenerateVarSymTable(decl, currentTable, funcTable, funcName);
                continue;
            }
            // ----------------------------------------
            // 要调用哪个类似于Create的函数？
            // -> 依然用Create
            // static Function *Create(FunctionType *FTy, bool ExternalLinkage = false,
            //                 std::string_view Name = "", Module *M = nullptr);
            
            // 1. FunctionType需要在外部new好
    //         FunctionType::FunctionType(Type *Result, const std::vector<Type *> &Params)
    // : Type(Type::FunctionTyID), Params(Params), Result(Result) { }
            // 第一个参数： FunctionType
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

            // ----------------------------------------
            // setName
            
            int index = 0;
            for (auto arg : funcDef->argList) {
                // 遍历每一个参数
                f->getArg(index)->setName(arg->as<FuncFParam*>()->name);
                index++;
            }

            // add basic block -> 
            // entry after_return exit
            // entry: alloca -> store -> jmp
            f->genEntryBlock(funcDef);
            f->genExitBlock(funcDef);

            // 中间要生成这些if/while block
            f->genSubBlock(funcDef->block->as<Block*>(), nullptr);
            f->genExitStmt();
            fmt::print("1\n");
            f->AnalysisBlockOrder();
            fmt::print("2\n");
            f->TempBlockConnect();
            fmt::print("3\n");
            // if (debug) fmt::print("out2\n");
            f->CreateBlocks();
            fmt::print("4\n");
            f->genReturnBlock(funcDef);
            fmt::print("5\n");
            
            fmt::print("6\n");
            f->BasicBlockConnect();
            fmt::print("7\n");
            
        }
    }

}

void Function::genExitStmt() {
    // load
    LoadInst *load;
    for (auto& t : getEntryBlock().getInstList()) {
        // t不是指针类型
        if (AllocaInst::classof(&t) && t.getName() == "ret_val.addr") {
            // load_inst = LoadInst::Create(&t);
            // rootTempBlock->instructions.push_back(load_inst);
            // lv = &t;
            // Bounds = ((AllocaInst*)&t)->getBounds();
            // break;
            load = LoadInst::Create(&t, &back());
            break;
        }
    }
    // ret
    RetInst::Create(load, &back());
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



void Function::genSubBlock(Block *rootBlock, TempBlock *rootTempBlock)
{
    
    // Entry
    bool entry = rootTempBlock == nullptr ? true : false;
    // alloca return value
    if (entry) {
        if (this->getFunctionType()->getReturnType() == Type::getIntegerTy()) {
            auto alloca_return_var = AllocaInst::Create(Type::getIntegerTy(), 1, &getEntryBlock());
            alloca_return_var->setName("ret_val.addr");
        }
        
    }
    bool debug = true;
    // if (debug) fmt::print("genSubBlock: \n");
    if (rootBlock == nullptr) {
        fmt::print("Error: the root block is empty\n");
    }
    for (auto blockItem : rootBlock->BlockItems) {
        // if (debug) fmt::print("search ifStmt in the instructions list: \n");
        
        auto statement = blockItem->as<BlockItem*>()->Stmt;

        // 每个blockItem都对应于一个语句，每次遇到if都意味着一个新的block的开始
        
        if (statement == nullptr) {
            // if (debug) fmt::print("IS A DECL\n");
            auto declare = blockItem->as<BlockItem*>()->Decl;
            // if (declare->as<Decl*>()->VarDecl->as<VarDecl*>() == nullptr) if(debug) fmt::print("null\n");
            if (auto varDeclStmt = declare->as<Decl*>()->VarDecl->as<VarDecl*>()) {
                if (debug) fmt::print("find out a Var Decl Stmt\n");
                std::vector<NodePtr> varDefs = varDeclStmt->VarDefs;
                for(size_t j = 0; j < varDefs.size(); j++){
                    // static GlobalVariable *Create(Type *EleTy, std::size_t NumElements = 1, bool ExternalLinkage = false,
                    //                  std::string_view Name = "", Module *M = nullptr);
                    // 找到type  -> 一个静态的成员，通过接口访问
                    // 找到name  -> varDef->name
                    // NumElements -> 
                    // ExternalLinkage -> false
                    auto varDef = varDefs[j]->as<VarDef*>();
                    
                    if(varDef != nullptr) {
                        int dim_product = 1;
                        // 数组全局变量
                        if(varDef->isArray) {
                            // 把数组维数的乘积算出来
                            
                            for (auto k : varDef->dimensions){
                                dim_product *= k;
                            }
                            // GlobalVariable::Create(Type::getIntegerTy(), dim_product, false, varDef->name, this);
                            
                        }
                        AllocaInst *alloca_instr;
                        if (entry) {
                            if (debug) fmt::print("EntryBlock Insert\n");
                            alloca_instr = AllocaInst::Create(Type::getIntegerTy(), dim_product, &getEntryBlock());
                            alloca_instr->setName(varDef->name);
                        } else {
                            alloca_instr = AllocaInst::Create(Type::getIntegerTy(), dim_product);
                            alloca_instr->setName(varDef->name);
                            rootTempBlock->instructions.push_back(alloca_instr);
                        }
                        if(varDef->isArray) {
                            // 把数组维数的乘积算出来
                            
                            for (auto k : varDef->dimensions){
                                // dim_product *= k;
                                alloca_instr->setBounds(k);
                            }
                            // GlobalVariable::Create(Type::getIntegerTy(), dim_product, false, varDef->name, this);
                            
                        }
                    }
                }
            }
            if(debug) fmt::print("out\n");
            continue;
        }   // is a decl, not a statement
            
        // assign语句
        
        if (auto assignStmt = statement->as<AssignStmt*>()) {
            Value * lv;
            Value * rv;
            if (debug) fmt::print("  find out an assign: \n");
            
            // 生成左/右值之前，要看是不是offset
            // 生成右值
            // 右值也可以是offset
            rv = PostOrderTraversal(assignStmt->Exp, rootTempBlock);
            fmt::print("right success\n");
            
            // 生成左值
            // 左值是alloca中的一个
            TempBlock* temp = rootTempBlock;
            bool found = false;
            auto var = assignStmt->LVal->as<LVal*>();
            


/*
    static OffsetInst *Create(Type *PointeeTy, Value *Ptr,
                              std::vector <Value *> &Indices,
                              std::vector <std::optional<std::size_t>> &Bounds,
                              Instruction *InsertBefore = nullptr);
    static OffsetInst *Create(Type *PointeeTy, Value *Ptr,
                              std::vector <Value *> &Indices,
                              std::vector <std::optional<std::size_t>> &Boundss,
                              BasicBlock *InsertAtEnd);
*/

            std::vector<std::optional<std::size_t>> Bounds;
            while(temp != nullptr) {
                for (auto t : rootTempBlock->instructions) {
                    fmt::print("loop\n");
                    if (AllocaInst::classof(t) && t->getName() == var->name) {
                        // load_inst = LoadInst::Create(t);
                        // rootTempBlock->instructions.push_back(load_inst);
                        lv = t;
                        Bounds = ((AllocaInst *)t)->getBounds();
                        found = true;
                        break;
                    }
                }
                if (found) break;
                temp = temp->parent;   
            }
            // 未找到，说明在entry里
            if (!found) {
                for (auto& t : getEntryBlock().getInstList()) {
                    // t不是指针类型
                    if (AllocaInst::classof(&t) && t.getName() == var->name) {
                        // load_inst = LoadInst::Create(&t);
                        // rootTempBlock->instructions.push_back(load_inst);
                        lv = &t;
                        Bounds = ((AllocaInst*)&t)->getBounds();
                        break;
                    }
                }
                // return load_inst;
            }
            fmt::print("left success\n");
            // 如果是offset，还要多封装一层
            OffsetInst *offset;
            if (var->isArray) {
                // 构造vector<size_t> Bounds ---> ok
                // 构造std::vector <Value *> &Indices
                std::vector<Value *> Indices;
                for (auto index : var->position) {
                    Indices.push_back(ConstantInt::Create((std::uint32_t)index));
                }
                fmt::print("start to create offset\n");

                // offsetInst将作为左值
                if (entry) {
                    offset = OffsetInst::Create(Type::getIntegerTy(), lv, Indices, Bounds, &getEntryBlock());
                } else {
                    offset = OffsetInst::Create(Type::getIntegerTy(), lv, Indices, Bounds);
                    rootTempBlock->instructions.push_back(offset);
                }
                
                fmt::print("create success\n");
                
                
                lv = offset;
            }
            
            
            
            if (entry)
                StoreInst::Create(rv, lv, &getEntryBlock());
            else {
                auto store = StoreInst::Create(lv, rv);
                rootTempBlock->instructions.push_back(store);
            }
            fmt::print("create success\n");
            
            // fmt::print("success\n");
            continue;
        }
        
        // if语句
        if (auto ifStmt = statement->as<IfStmt*>()) {
            if (debug) fmt::print("  find out an if: \n");
            // auto ifStmt = items->as<IfStmt*>();
            // then block是一定存在的
            
            BlockStmt *then_block_stmt = ifStmt->then->as<BlockStmt*>();
            Block *then_block = then_block_stmt->Block->as<Block*>();



            // 解析表达式
            // 要把tempBlock传进去的
            PostOrderTraversal(ifStmt->condition, rootTempBlock);
            // Value * val = CalculateExpVal(ifStmt->condition);
            // ifStmt->condition->as<>();
            fmt::print("Post out\n");

            TempBlock *temp_block_0 = new TempBlock();
            temp_block_0->depth = this->cur_depth;
            temp_block_0->type = TempBlock::TempBlockType::IF_THEN;
            temp_block_0->if_index = this->if_index;
            temp_block_0->else_index = this->else_index;
            temp_block_0->parent = rootTempBlock;
            block_exec_ordered.push_back(temp_block_0);
            fmt::print("{}\n", "push"+std::to_string(cur_depth));
            
            
            
            // if (debug) fmt::print("{}\n", "if_then_" + std::to_string(if_then_block->if_else_block_idx) + " depth: " + std::to_string(if_then_block->depth));
            if_index++;

            // 递归的做这件事
            this->cur_depth++;
            genSubBlock(then_block, temp_block_0);
            this->cur_depth--;
            // if (debug) fmt::print("out \n");

            // BasicBlock *if_end_block
            
            // unmatched if
            if(ifStmt->matched != false) {
                // if_else


                // 两种可能性 IfStmt / BlockStmt
                if (BlockStmt* els_block_stmt = ifStmt->els->as<BlockStmt*>()) {
                    // if (debug) fmt::print("the last else \n");
                    // 最后一个else
                    Block *els_block = els_block_stmt->Block->as<Block*>();
                    
                    TempBlock *temp_block_1 = new TempBlock();
                    temp_block_1->depth = this->cur_depth;
                    temp_block_1->type = TempBlock::TempBlockType::IF_ELSE;
                    temp_block_1->if_index = temp_block_0->if_index;
                    temp_block_1->else_index = temp_block_0->else_index;
                    temp_block_1->parent = rootTempBlock;

                    block_exec_ordered.push_back(temp_block_1);
                    fmt::print("{}\n", "push"+std::to_string(cur_depth));



                    // if (debug) fmt::print("{}\n", "if_else_" + std::to_string(if_else_block->if_else_block_idx) + " depth: " + std::to_string(if_else_block->depth));

                    this->cur_depth++;
                    genSubBlock(els_block, temp_block_1);
                    this->cur_depth--;
                    // if (debug) fmt::print("out \n");
                }

                else if (IfStmt* els_if_stmt = ifStmt->els->as<IfStmt*>()) {
                    
                    TempBlock *temp_block_1 = new TempBlock();
                    temp_block_1->depth = this->cur_depth;
                    temp_block_1->type = TempBlock::TempBlockType::IF_ELSE;
                    temp_block_1->if_index = temp_block_0->if_index;
                    temp_block_1->else_index = temp_block_0->else_index;
                    temp_block_1->parent = rootTempBlock;
                    block_exec_ordered.push_back(temp_block_1);
                    fmt::print("{}\n", "push"+std::to_string(cur_depth));
                    
                    
                    // if (debug) fmt::print("{}\n", "if_else_" + std::to_string(if_else_block->if_else_block_idx) + " depth: " + std::to_string(if_else_block->depth));
                    
                    
                    // 不是一个block，那就创建一个 qwq 不讲武德
                    Block *temp_block = new Block();
                    BlockItem *temp_blockItem = new BlockItem();
                    temp_blockItem->as<BlockItem*>()->Stmt = els_if_stmt;
                    temp_block->as<Block*>()->BlockItems.push_back(temp_blockItem);
                    this->cur_depth++;
                    genSubBlock(temp_block, temp_block_1);
                    this->cur_depth--;


                    // if (debug) fmt::print("out \n");
                    // genSubBlock(new Block())
                }
                else_index++;
            }
            

            TempBlock *temp_block_2 = new TempBlock();
            temp_block_2->depth = this->cur_depth;
            temp_block_2->type = TempBlock::TempBlockType::IF_END;
            temp_block_2->if_index = temp_block_0->if_index;
            temp_block_2->else_index = temp_block_0->else_index;
            temp_block_2->parent = rootTempBlock;
            block_exec_ordered.push_back(temp_block_2);
            fmt::print("{}\n", "push"+std::to_string(cur_depth));

            // 需要修改rootTempBlock
            rootTempBlock = temp_block_2;
            entry = false;
            fmt::print("rootTempBlock changed\n");
            

            // if_end中装的是所有剩余的语句 -> test里就是那个return 1;
            // 这里不用递归调用了

            // if (debug) fmt::print("{}\n", "if_end_" + std::to_string(if_end_block->if_else_block_idx) + " depth: " + std::to_string(if_end_block->depth));
            
        }
        // expStmt
        if (auto expStmt = statement->as<ExpStmt*>()) {
            PostOrderTraversal(expStmt->Exp, rootTempBlock);
        }

        if (auto returnStmt = statement->as<ReturnStmt*>()) {
            if (entry) {
                Value *lv;
                for (auto& t : getEntryBlock().getInstList()) {
                    // t不是指针类型
                    if (AllocaInst::classof(&t) && t.getName() == "ret_val.addr") {
                        // load_inst = LoadInst::Create(&t);
                        // rootTempBlock->instructions.push_back(load_inst);
                        lv = &t;
                        break;
                    }
                }
                Value *rv;
                rv = PostOrderTraversal(returnStmt->result, nullptr);
                StoreInst::Create(rv, lv, &getEntryBlock());
                // jump to exit
                JumpInst::Create(&back(), &getEntryBlock());
            }
        }
    }
    
    // 要根据block_exec_ordered中的tempBlock确定Block之间的跳转关系
    if (debug) fmt::print("out1\n");

}

    // static AllocaInst *Create(Type *PointeeTy, std::size_t NumElements,
    //                           Instruction *InsertBefore = nullptr);
    // static AllocaInst *Create(Type *PointeeTy, std::size_t NumElements,
    //                           BasicBlock *InsertAtEnd);


void Function::genEntryBlock(FuncDef *funcdef)
{
    // Create
    // static BasicBlock *Create(Function *Parent = nullptr, BasicBlock *InsertBefore = nullptr);
    auto entry_block = BasicBlock::Create(this, nullptr);
    // setName
    entry_block->setName("entry");
    // TODO: 调用新增的Block中的函数，生成语句
    return;
}
void Function::genReturnBlock(FuncDef *funcdef)
{
    auto return_block = BasicBlock::Create(this, &back());
    // setName
    return_block->setName("after return");
    // jump to exit
    JumpInst::Create(&back(), return_block);
    // TODO: 调用新增的Block中的函数，生成语句
    return;
}
void Function::genExitBlock(FuncDef *funcdef)
{
    auto exit_block = BasicBlock::Create(this, nullptr);
    // setName
    exit_block->setName("exit");
    // TODO: 调用新增的Block中的函数，生成语句
    return;
}



void Function::AnalysisBlockOrder()
{
    for (int i = 0; i < (int)block_exec_ordered.size(); i++)
    {
        if (i == (int)block_exec_ordered.size()-1) {
            block_exec_ordered[i]->is_last = true;
            break;
        }
        switch(block_exec_ordered[i]->type) {
            case TempBlock::TempBlockType::IF_THEN:
                if (block_exec_ordered[i+1]->depth > block_exec_ordered[i]->depth) {
                    // 内部又分裂出了新的block
                    block_exec_ordered[i]->branch = TempBlock::BranchType::BR;
                    block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index;
                } else {
                    // 内部没有分裂出新的block，跳转到end
                    block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::IF_END;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i]->if_index;
                }
                
                break;
            case TempBlock::TempBlockType::IF_ELSE:
                if (block_exec_ordered[i+1]->depth > block_exec_ordered[i]->depth) {
                    // 内部又分裂出了新的block
                    block_exec_ordered[i]->branch = TempBlock::BranchType::BR;
                    block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index; // 不是加1
                } else {
                    // 内部没有分裂出新的block，跳转到end
                    block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::IF_END;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i]->if_index;
                }
                break;
            case TempBlock::TempBlockType::IF_END:
                // 1. 跳转到当前层数下的下一个block
                if ((i != (int)block_exec_ordered.size() - 1) && (block_exec_ordered[i]->depth == block_exec_ordered[i+1]->depth)) {
                    block_exec_ordered[i]->branch = TempBlock::BranchType::BR;
                    block_exec_ordered[i]->next_block_type = block_exec_ordered[i+1]->type; // 一定是If_then
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index;
                } else if (i == block_exec_ordered.size()-1){
                    // 最后一个sub block
                    block_exec_ordered[i]->is_last = true;
                } else {
                    // 要跳转到上一级的if_end
                    block_exec_ordered[i]->branch = TempBlock::BranchType::JMP;
                    block_exec_ordered[i]->next_block_type = TempBlock::TempBlockType::IF_END;
                    block_exec_ordered[i]->next_block_index = block_exec_ordered[i+1]->if_index;
                }

                break;
        }
    }
}


void Function::TempBlockConnect()
{
    for (auto t : block_exec_ordered) {
        // if (t->is_last) {
        //     t->next_block = nullptr;
        // }
        for (auto next : block_exec_ordered) {
            
            if (next->type == t->next_block_type && next->if_index == t->if_index) {
                t->next_block = next;
            }
        }
    }
    return;
}

// static JumpInst *Create(BasicBlock *Dest, Instruction *InsertBefore = nullptr);
// static JumpInst *Create(BasicBlock *Dest, BasicBlock *InsertAtEnd);


    // static BranchInst *Create(BasicBlock *IfTrue, BasicBlock *IfFalse, 
    //                           Value *Cond, 
    //                           Instruction *InsertBefore = nullptr);
    // static BranchInst *Create(BasicBlock *IfTrue, BasicBlock *IfFalse, 
    //                           Value *Cond, 
    //                           BasicBlock *InsertAtEnd);

/*
br needs a Value

*/

void Function::BasicBlockConnect()
{
    for (auto t : block_exec_ordered) {
        if (t->branch == TempBlock::BranchType::JMP) {
            if (t->is_last) {
                JumpInst::Create(&back(), t->basic_block);
            } else {
                JumpInst::Create(t->next_block->basic_block, t->basic_block); // 问题是exit和还没有创建
            }
            
        }
        if (t->branch == TempBlock::BranchType::BR) {
            // BranchInst::Create()
        }
    }
}


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
    // BasicBlock *if_end_block = BasicBlock::Create(this, nullptr);
    // if_end_block->depth = this->cur_depth;
    // if_end_block->if_else_block_idx = if_then_block->if_else_block_idx;
    // if_end_block->setName(if_then_block->if_else_block_idx == 0 ? "if_end" : "if_end_"+std::to_string(if_then_block->if_else_block_idx));
    int else_index = 0;
    if ((int)block_exec_ordered.size() == 0) return;
    for (int i = 0; i <= block_exec_ordered[(int)block_exec_ordered.size()-1]->if_index; i++) {
        // find if_then
        for (auto t : block_exec_ordered) {
            if (t->if_index == i && t->type == TempBlock::TempBlockType::IF_THEN) {
                BasicBlock *block0 = BasicBlock::Create(this, &back());
                block0->setName(i==0?"if_then":"if_then."+std::to_string(i));
                t->basic_block = block0;

                // insert instructions
                // iterator -> InstListType::iterator
                BasicBlock::iterator it = block0->begin();
                for (int i = (int)t->instructions.size() - 1; i >= 0; i--) {
                    it = t->instructions[i]->insertInto(block0, it);
                }
                // for (auto inst : t->instructions) {
                //     // void insertBefore(BasicBlock &BB, InstListType::iterator IT);
                //     it = inst->insertInto(block0, it);
                // }
            }
        }
        // find if_end
        for (auto t : block_exec_ordered) {
            if (t->if_index == i && t->type == TempBlock::TempBlockType::IF_END) {
                BasicBlock *block1 = BasicBlock::Create(this, &back());
                block1->setName(i==0?"if_end":"if_end."+std::to_string(i));
                t->basic_block = block1;


                // insert instructions
                // iterator -> InstListType::iterator
                BasicBlock::iterator it = block1->begin();
                for (int i = (int)t->instructions.size() - 1; i >= 0; i--) {
                    it = t->instructions[i]->insertInto(block1, it);
                }
                // for (auto inst : t->instructions) {
                //     // void insertBefore(BasicBlock &BB, InstListType::iterator IT);
                //     it = inst->insertInto(block1, it);
                // }
            }
        }
        // find if_else
        for (auto t : block_exec_ordered) {
            if (t->if_index == i && t->type == TempBlock::TempBlockType::IF_ELSE) {
                BasicBlock *block2 = BasicBlock::Create(this, &back());
                block2->setName(else_index==0?"if_else":"if_else."+std::to_string(else_index));
                t->basic_block = block2;
                else_index++;


                // insert instructions
                // iterator -> InstListType::iterator
                BasicBlock::iterator it = block2->begin();
                for (int i = (int)t->instructions.size() - 1; i >= 0; i--) {
                    it = t->instructions[i]->insertInto(block2, it);
                }
                // for (auto inst : t->instructions) {
                //     // void insertBefore(BasicBlock &BB, InstListType::iterator IT);
                //     it = inst->insertInto(block2, it);
                // }
            }
        }
    }

}






void TempBlock::addInstruction(unsigned Opcode)
{
    switch(Opcode) {
        case Instruction::Jump:
            // JumpInst::Create()
            break;
        case Instruction::Br:
            break;
    }
}

void Function::CalculateExpVal(NodePtr root)
{
    // bool debug = true;
    // if (auto addExp = root->as<addExp*>()) {
    //     if(debug) fmt::print("AddExp\n");
    //     addExp->lhs+
    // }

}

// 越靠近上层的表达式反而在中间代码上体现在后面
// 这应该是个后序遍历
// Lval / const 是叶子节点
Value* Function::PostOrderTraversal(NodePtr root, TempBlock *rootTempBlock) {
    fmt::print("PostOrderTraversal: \n");
    bool entry = rootTempBlock == nullptr ? true : false;
    // leaf
    if (root->as<intExp*>() != nullptr || root->as<LVal*>() != nullptr) {
        // constant
        if (auto constant = root->as<intExp*>()) {
            fmt::print("constant\n");
            // 要返回一个value类型回去
            return ConstantInt::Create((std::uint32_t)constant->value);
        }
        // variable
        if (auto var = root->as<LVal*>()) {
            fmt::print("LVal\n");
            // 区别是，Create的第一个参数不同，一个是Alloca,一个是Offset
            // if (rootTempBlock == nullptr) fmt::print("empty error\n");
            // new 一个Load类型
            std::vector<std::optional<std::size_t>> Bounds;
            LoadInst* load_inst;
            if (entry) {
                
                fmt::print("entry\n");
                for (auto& t : getEntryBlock().getInstList()) {
                    // t不是指针类型
                    if (AllocaInst::classof(&t) && t.getName() == var->name) {
                        
                        
                        if (var->isArray) {
                            OffsetInst *offset;
                            // 构造vector<size_t> Bounds ---> ok
                            Bounds = ((AllocaInst*)&t)->getBounds();
                            // 构造std::vector <Value *> &Indices
                            std::vector<Value *> Indices;
                            for (auto index : var->position) {
                                Indices.push_back(ConstantInt::Create((std::uint32_t)index));
                            }
                            fmt::print("start to create offset\n");

                            // offsetInst将作为左值
                            
                            offset = OffsetInst::Create(Type::getIntegerTy(), &t, Indices, Bounds, &getEntryBlock());
                            
                            
                            fmt::print("create success\n");
                            
                            load_inst = LoadInst::Create(offset, &getEntryBlock());
                            
                        } else {
                            load_inst = LoadInst::Create(&t, &getEntryBlock());
                        }
                        
                        
                        
                        // rootTempBlock->instructions.push_back(load_inst);
                        break;
                    }
                }
                return load_inst;
            }
            // static LoadInst *Create(Value *Ptr, Instruction *InsertBefore = nullptr);
            // 解决办法：增加一个stack记录好这些变量
            TempBlock* temp = rootTempBlock;
            
            bool found = false;
            while(temp != nullptr) {
                for (auto t : rootTempBlock->instructions) {
                    fmt::print("loop\n");
                    if (AllocaInst::classof(t) && t->getName() == var->name) {
                        Bounds = ((AllocaInst *)t)->getBounds();
                        
                        
                        load_inst = LoadInst::Create(t);
                        rootTempBlock->instructions.push_back(load_inst);
                        found = true;
                        break;
                    }
                }
                if (found) break;
                temp = temp->parent;   
            }
            // 未找到，说明在entry里
            if (!found) {
                for (auto& t : getEntryBlock().getInstList()) {
                    // t不是指针类型
                    if (AllocaInst::classof(&t) && t.getName() == var->name) {
                        Bounds = ((AllocaInst*)&t)->getBounds();
                        // ------------------------------
                        if (var->isArray) {
                            OffsetInst *offset;
                            // 构造vector<size_t> Bounds ---> ok
                            // 构造std::vector <Value *> &Indices
                            std::vector<Value *> Indices;
                            for (auto index : var->position) {
                                Indices.push_back(ConstantInt::Create((std::uint32_t)index));
                            }
                            fmt::print("start to create offset\n");

                            // offsetInst将作为左值
                            
                            offset = OffsetInst::Create(Type::getIntegerTy(), &t, Indices, Bounds);
                            rootTempBlock->instructions.push_back(offset);
                            
                            
                            fmt::print("create success\n");
                            
                            
                            // lv = offset;
                            load_inst = LoadInst::Create(offset);
                            rootTempBlock->instructions.push_back(load_inst);
                        }

                        // ------------------------------
                        else {
                            load_inst = LoadInst::Create(&t);
                            rootTempBlock->instructions.push_back(load_inst);
                        }
                        
                        break;
                    }
                }
                return load_inst;
            }
            


            
            // auto load_inst = LoadInst::Create();
            if (load_inst == nullptr) fmt::print("empty error\n");
            return load_inst;
        }
    }
    
    // post order
    // PostOrderTraversal(root->lhs);
    // PostOrderTraversal(root->rhs);
    if (auto t = root->as<addExp*>()) {
        // fmt::print("AddExp\n");
        // fmt::print("Left: \n");
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        // fmt::print("Out\n");
        // fmt::print("Right: \n");
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        // fmt::print("Out\n");
        // what to do
        // 先从叶子开始处理
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Add, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::Add, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<subExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Sub, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::BinaryOps::Sub, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<mulExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Mul, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::BinaryOps::Mul, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<divExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Div, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::BinaryOps::Div, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<modExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Mod, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::BinaryOps::Mod, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<andExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::And, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::BinaryOps::And, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<orExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Or, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::BinaryOps::Or, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<lessExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Lt, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::BinaryOps::Lt, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<greatExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Gt, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::BinaryOps::Gt, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<leqExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Le, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::Le, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<geqExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Ge, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::Ge, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<eqExp*>()) {
        fmt::print("eq\n");
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        fmt::print("eq finished\n");
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Eq, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::Eq, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<neqExp*>()) {
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = PostOrderTraversal(t->rhs, rootTempBlock);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Ne, val1, val2, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::Ne, val1, val2, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } 


    // 下面就是单目运算符了
    // fmt::print("judge\n");
    else if (auto t = root->as<negExp*>()){
        fmt::print("1\n");
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        fmt::print("2\n");
        auto val2 = ConstantInt::Create((std::uint32_t)0);
        fmt::print("3\n");
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Sub, val2, val1, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::Sub, val2, val1, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        fmt::print("4\n");
        return inst;
    } else if (auto t = root->as<notExp*>()){
        auto val1 = PostOrderTraversal(t->lhs, rootTempBlock);
        auto val2 = ConstantInt::Create((std::uint32_t)0);
        BinaryInst* inst;
        if (entry) inst = BinaryInst::Create(Instruction::Eq, val2, val1, Type::getIntegerTy(), &getEntryBlock());
        else {
            inst = BinaryInst::Create(Instruction::Eq, val2, val1, Type::getIntegerTy());
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    } else if (auto t = root->as<posExp*>()){
        return PostOrderTraversal(t->lhs, rootTempBlock);
    } else if (auto t = root->as<funcallExp*>()){
        CallInst* inst;
        // find the function
        // for (auto f : )
        fmt::print("func call\n");


        Function *f = getParent()->getFunction(t->name);
        std::vector<Value *> Args;
        for (auto param : t->params) {
            // 每个都是LVal*类型 -> 广义来讲，都是Exp
            // if (auto var = param->as<LVal*>()) {

            // }
            auto arg = PostOrderTraversal(param, rootTempBlock);
            Args.push_back(arg);
        }
        
        if (entry) inst = CallInst::Create(f, Args, &getEntryBlock());
        else {
            inst = CallInst::Create(f, Args);
            rootTempBlock->instructions.push_back(inst);
        }
        return inst;
    }
    // 还剩assignExp没有处理
    
}
/*

    static CallInst *Create(Function *Callee, const std::vector<Value *> &Args, 
                            Instruction *InsertBefore = nullptr);
    static CallInst *Create(Function *Callee, const std::vector<Value *> &Args, 
                            BasicBlock *InsertAtEnd);



有哪些双目运算符？
add sub mul div mod and or


// let %7: i32 = mul 3, 4 是如何产生的，binaryStmt?
BinaryInst::Create(BinaryOps::Add, val1, val2, Type::getIntegerTy(), )
这时候有block吗？

    static BinaryInst *Create(BinaryOps Op, Value *LHS, Value *RHS, Type *Ty,
                              BasicBlock *InsertAtEnd);
*/

