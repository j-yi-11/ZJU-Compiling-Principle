#include "ir/ir.h"
#include "ir/type.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string_view>
#include <vector>

Use::Use(Value *Parent) : Parent(Parent) { }


void Use::removeFromList() {
    *Prev = Next;
    if (Next) {
        Next->Prev = Prev;
    }
}

void Use::addToList(Use **UseList) {
    Next = *UseList;
    if (Next) {
        Next->Prev = &Next;
    }
    Prev = UseList;
    *Prev = this;
}

void Use::set(Value *V) {
    if (Val)
        removeFromList();
    Val = V;
    if (V)
        V->addUse(*this);
}


Value::Value(Type *Ty, unsigned scid)
    : SubclassID(scid), Ty(Ty) {} 

Value::~Value() {
    // remove all uses.
    replaceAllUsesWith(nullptr);
}

unsigned Value::getNumUses() const {
    auto view = getUserView();
    unsigned size = 0;
    for (auto I = view.begin(), IE = view.end(); I != IE; ++I)
        ++size;
    return size;
}

void Value::replaceAllUsesWith(Value *V) {
    std::vector<Use *> Uses;
    auto view = getUserView();
    for (auto &I : view)
        Uses.push_back(&I);
    
    for (auto I: Uses)
        I->set(V);
}

void Value::setName(std::string_view Name) {
    // 'Constant' has no name
    if (isa<Constant>(this))
        return;
    this->Name = Name;
}

bool Value::hasName() const {
    return !Name.empty();
}


Constant::Constant(Type *Ty, unsigned VT)
    : Value(Ty, VT) {}

ConstantInt::ConstantInt(std::uint32_t Val)
    : Constant(Type::getIntegerTy(), Value::ConstantIntVal),
      value(Val) {}

ConstantInt *ConstantInt::Create(std::uint32_t Val) {
    return new ConstantInt(Val);
}

ConstantUnit::ConstantUnit()
    : Constant(Type::getUnitTy(), Value::ConstantUnitVal) {}

ConstantUnit *ConstantUnit::Create() {
    return new ConstantUnit;
}


Instruction::Instruction(Type *Ty, unsigned Opcode, 
                         const std::vector<Value *> &Ops,
                         Instruction *InsertBefore) 
    : Value(Ty, Value::InstructionVal + Opcode),
      NumUserOperands(Ops.size()) {
    if (NumUserOperands > 0) {
        Uses = std::allocator<Use>().allocate(NumUserOperands);
        for (unsigned i = 0, e = NumUserOperands; i != e; ++i) {
            new (Uses + i) Use(this);
            Uses[i].set(Ops[i]);
        }
    }
    if (InsertBefore) {
        BasicBlock *BB = InsertBefore->getParent();
        assert(BB && "Instruction to insert before is not in a basic block!");
        insertInto(BB, BasicBlock::iterator(InsertBefore));
    }
}

Instruction::Instruction(Type *Ty, unsigned Opcode,
                         const std::vector<Value *> &Ops, 
                         BasicBlock *InsertAtEnd)
    : Value(Ty, Value::InstructionVal + Opcode), 
      NumUserOperands(Ops.size()) {
    if (NumUserOperands > 0) {
        Uses = std::allocator<Use>().allocate(NumUserOperands);
        for (unsigned i = 0, e = NumUserOperands; i != e; ++i) {
            new (Uses + i) Use(this);
            Uses[i].set(Ops[i]);
        }
    }
    if (InsertAtEnd) {
        insertInto(InsertAtEnd, InsertAtEnd->end());
    }
}

Instruction::~Instruction() {
    if (NumUserOperands > 0) {
        for (unsigned i = 0, e = NumUserOperands; i != e; ++i) {
            Uses[i].~Use();
        }
        std::allocator<Use>().deallocate(Uses, NumUserOperands);
        Uses = nullptr;
    }
}

void Instruction::setParent(BasicBlock *BB) {
    Parent = BB;
}

void Instruction::insertBefore(Instruction *InsertBefore) {
    insertBefore(BasicBlock::iterator(InsertBefore));
}


void Instruction::insertBefore(BasicBlock::iterator InsertBefore) {
    insertBefore(*InsertBefore->getParent(), InsertBefore);
}

void Instruction::insertBefore(BasicBlock &BB, InstListType::iterator IT) {
    BB.getInstList().insert(IT, this);
    setParent(&BB);
}

void Instruction::insertAfter(Instruction *InsertPos) {
    BasicBlock *DestParent = InsertPos->getParent();

    DestParent->getInstList().insertAfter(BasicBlock::iterator(InsertPos), this);
    setParent(DestParent);
}

BasicBlock::iterator Instruction::insertInto(BasicBlock *BB, BasicBlock::iterator IT) {
    assert(getParent() == nullptr && "Expected detached instruction!");
    assert((IT == BB->end() || IT->getParent() == BB) && "IT not in ParentBB");
    insertBefore(*BB, IT);
    setParent(BB);
    return BasicBlock::iterator(this);
}

void Instruction::removeFromParent() {
    getParent()->getInstList().remove(BasicBlock::iterator(this));
}

BasicBlock::iterator Instruction::eraseFromParent() {
    return getParent()->getInstList().erase(BasicBlock::iterator(this));
}



BinaryInst::BinaryInst(BinaryOps Op, Value *LHS, Value *RHS, Type *Ty,
                       Instruction *InsertBefore) 
    : Instruction(Ty, Op, std::vector<Value *> {LHS, RHS}, InsertBefore) {

}

BinaryInst::BinaryInst(BinaryOps Op, Value *LHS, Value *RHS, Type *Ty,
                       BasicBlock *InsertAtEnd)
    : Instruction(Ty, Op, std::vector<Value *> {LHS, RHS}, InsertAtEnd) {

}

BinaryInst *BinaryInst::Create(BinaryOps Op, Value *LHS, Value *RHS, Type *Ty,
                              Instruction *InsertBefore) {
    assert(LHS->getType() == RHS->getType() &&
        "Cannot create binary operator with two operands of differing type!");
    return new BinaryInst(Op, LHS, RHS, Ty, InsertBefore);
}
    
BinaryInst *BinaryInst::Create(BinaryOps Op, Value *LHS, Value *RHS, Type *Ty,
                              BasicBlock *InsertAtEnd) {
    auto *Res = Create(Op, LHS, RHS, Ty);
    Res->insertInto(InsertAtEnd, InsertAtEnd->end());
    return Res;
}

AllocaInst::AllocaInst(Type *PointeeType, std::size_t NumElements, Instruction *InsertBefore)
    : Instruction(PointerType::get(PointeeType), Instruction::Alloca, 
    { }, InsertBefore),
      AllocatedType(PointeeType), NumElements(NumElements) {
    assert(!PointeeType->isUnitTy() && "Cannot allocate () type!");
}

AllocaInst::AllocaInst(Type *PointeeType, std::size_t NumElements, BasicBlock *InsertAtEnd)
    : Instruction(PointerType::get(PointeeType), Instruction::Alloca, 
    { }, InsertAtEnd),
      AllocatedType(PointeeType), NumElements(NumElements) {
    assert(!PointeeType->isUnitTy() && "Cannot allocate () type!");
}


AllocaInst *AllocaInst::Create(Type *PointeeTy, std::size_t NumElements,
                              Instruction *InsertBefore) {
    return new AllocaInst(PointeeTy, NumElements, InsertBefore);
}

AllocaInst *AllocaInst::Create(Type *PointeeTy, std::size_t NumElements,
                              BasicBlock *InsertAtEnd) {
    auto *Res = Create(PointeeTy, NumElements);
    Res->insertInto(InsertAtEnd, InsertAtEnd->end());
    return Res;
}

StoreInst::StoreInst(Value *Val, Value *Ptr, Instruction *InsertBefore)
    : Instruction(Type::getUnitTy(), Instruction::Store, { Val, Ptr }, InsertBefore) {

}

StoreInst::StoreInst(Value *Val, Value *Ptr, BasicBlock *InsertAtEnd)
    : Instruction(Type::getUnitTy(), Instruction::Store, { Val, Ptr }, InsertAtEnd) {
        
}

StoreInst *StoreInst::Create(Value *Val, Value *Ptr,
                             Instruction *InsertBefore) {
    return new StoreInst(Val, Ptr, InsertBefore);
}

StoreInst *StoreInst::Create(Value *Val, Value *Ptr,
                             BasicBlock *InsertAtEnd) {
    auto *Res = Create(Val, Ptr);
    Res->insertInto(InsertAtEnd, InsertAtEnd->end());
    return Res;
}

LoadInst::LoadInst(Value *Ptr, Instruction *InsertBefore)
    : Instruction(dyn_cast<PointerType>(Ptr->getType())->getElementType(),
    Instruction::Load, { Ptr }, InsertBefore) {
    assert(Ptr->getType()->isPointerTy() && "Cannot load from non-pointer type!");
}

LoadInst::LoadInst(Value *Ptr, BasicBlock *InsertAtEnd)
    : Instruction(dyn_cast<PointerType>(Ptr->getType())->getElementType(),
    Instruction::Load, { Ptr }, InsertAtEnd) {
    assert(Ptr->getType()->isPointerTy() && "Cannot load from non-pointer type!");
}

LoadInst *LoadInst::Create(Value *Ptr, Instruction *InsertBefore) {
    return new LoadInst(Ptr, InsertBefore);
}

LoadInst *LoadInst::Create(Value *Ptr, BasicBlock *InsertAtEnd) {
    auto *Res = Create(Ptr);
    Res->insertInto(InsertAtEnd, InsertAtEnd->end());
    return Res;
}

void OffsetInst::AssertOK() const {
    assert(getOperand(0)->getType()->isPointerTy() && "Offset pointer is not of the pointer type!");
    assert(dyn_cast<PointerType>(getOperand(0)->getType())->getElementType() == ElementTy &&
           "Element type of offset does not match the type of pointer!");
    assert(getNumOperands() == (unsigned)(bounds().size() + 1) && "Num of indices and bounds does not match!");
}

OffsetInst::OffsetInst(Type *PointeeTy,
               std::vector <Value *> &Ops,
               std::vector <std::optional<std::size_t>> &Bounds,
               Instruction *InsertBefore) 
    : Instruction(PointerType::get(PointeeTy), Instruction::Offset, Ops, InsertBefore),
      ElementTy(PointeeTy),
      Bounds(Bounds) {
    AssertOK();
}

OffsetInst::OffsetInst(Type *PointeeTy,
               std::vector <Value *> &Ops,
               std::vector <std::optional<std::size_t>> &Bounds,
               BasicBlock *InsertAtEnd) 
    : Instruction(PointerType::get(PointeeTy), Instruction::Offset, Ops, InsertAtEnd),
      ElementTy(PointeeTy),
      Bounds(Bounds) {
    AssertOK();
}

OffsetInst *OffsetInst::Create(Type *PointeeTy, Value *Ptr,
                              std::vector <Value *> &Indices,
                              std::vector <std::optional<std::size_t>> &Bounds,
                              Instruction *InsertBefore) {
    std::vector<Value *> Ops { Ptr };
    Ops.insert(Ops.end(), Indices.begin(), Indices.end());
    return new OffsetInst(PointeeTy, Ops, Bounds, InsertBefore);
}

OffsetInst *OffsetInst::Create(Type *PointeeTy, Value *Ptr,
                              std::vector <Value *> &Indices,
                              std::vector <std::optional<std::size_t>> &Bounds,
                              BasicBlock *InsertAtEnd) {

    std::vector<Value *> Ops { Ptr };
    Ops.insert(Ops.end(), Indices.begin(), Indices.end());
    auto *Res = new OffsetInst(PointeeTy, Ops, Bounds);
    Res->insertInto(InsertAtEnd, InsertAtEnd->end());
    return Res;
}


bool OffsetInst::accumulateConstantOffset(std::size_t &Offset) const {
    std::vector<std::size_t> indices;
    std::vector<std::size_t> no_option_bounds;
    for (auto BI = Bounds.begin() + 1; BI != Bounds.end(); ++BI) {
        no_option_bounds.emplace_back(BI->value());
    }
    no_option_bounds.emplace_back(1);

    for (const_op_iterator Op = op_begin() + 1, E = op_end(); Op != E; ++Op) {
        if (auto *Index = dyn_cast<ConstantInt>(Op->get())) {
            indices.emplace_back(Index->getValue());
        } else {
            return false;
        }
    }

    size_t TotalOffset = 0;
    for (size_t dim = 0; dim < indices.size(); ++dim) {
        TotalOffset = (TotalOffset + indices[dim]) * no_option_bounds[dim];
    }
    Offset += TotalOffset;

    return true;
}

CallInst::CallInst(Function *Callee, 
                   const std::vector<Value *> &Args,
                   Instruction *InsertBefore)
    : Instruction(Callee->getReturnType(), Instruction::Call, Args, InsertBefore),
      Callee(Callee) {
    assert(Callee->getNumParams() == Args.size() && "Number of arguments does not match number of parameters!");
}

CallInst::CallInst(Function *Callee, 
                   const std::vector<Value *> &Args,
                   BasicBlock *InsertAtEnd)
    : Instruction(Callee->getReturnType(), Instruction::Call, Args, InsertAtEnd),
      Callee(Callee) {
    assert(Callee->getNumParams() == Args.size() && "Number of arguments does not match number of parameters!");
}

CallInst *CallInst::Create(Function *Callee, 
                           const std::vector<Value *> &Args,
                           Instruction *InsertBefore) {
    return new CallInst(Callee, Args, InsertBefore);
}

CallInst *CallInst::Create(Function *Callee, 
                           const std::vector<Value *> &Args,
                           BasicBlock *InsertAtEnd) {
    auto *Res = Create(Callee, Args);
    Res->insertInto(InsertAtEnd, InsertAtEnd->end());
    return Res;
}

JumpInst::JumpInst(BasicBlock *Dest, Instruction *InsertBefore)
    : Instruction(Type::getUnitTy(), Instruction::Jump, { }, InsertBefore),
      Dest(Dest) {
}

JumpInst::JumpInst(BasicBlock *Dest, BasicBlock *InsertAtEnd)
    : Instruction(Type::getUnitTy(), Instruction::Jump, { }, InsertAtEnd),
      Dest(Dest) {
}

JumpInst *JumpInst::Create(BasicBlock *Dest, Instruction *InsertBefore) {
    return new JumpInst(Dest, InsertBefore);
}

JumpInst *JumpInst::Create(BasicBlock *Dest, BasicBlock *InsertAtEnd) {
    auto *Res = Create(Dest);
    Res->insertInto(InsertAtEnd, InsertAtEnd->end());
    return Res;
}

RetInst::RetInst(Value *Val, Instruction *InsertBefore)
    : Instruction(Type::getUnitTy(), Instruction::Ret, { Val }, InsertBefore) {

}

RetInst::RetInst(Value *Val, BasicBlock *InsertAtEnd)
    : Instruction(Type::getUnitTy(), Instruction::Ret, { Val }, InsertAtEnd) {

}

RetInst *RetInst::Create(Value *Val, Instruction *InsertBefore) {
    return new RetInst(Val, InsertBefore);
}

RetInst *RetInst::Create(Value *Val, BasicBlock *InsertAtEnd) {
    auto *Res = Create(Val);
    Res->insertInto(InsertAtEnd, InsertAtEnd->end());
    return Res;
}

void BranchInst::AssertOK() const {
    assert(getCondition()->getType()->isIntegerTy() &&
           "May only branch on integer predicate!");
}

BranchInst::BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond, Instruction *InsertBefore)
    : Instruction(Type::getUnitTy(), Instruction::Br, 
    { Cond }, InsertBefore),
      IfTrue(IfTrue), IfFalse(IfFalse) {
    AssertOK();
} 

BranchInst::BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond, BasicBlock *InsertAtEnd)
    : Instruction(Type::getUnitTy(), Instruction::Br,
    { Cond }, InsertAtEnd),
      IfTrue(IfTrue), IfFalse(IfFalse) {
    AssertOK();
}


BranchInst *BranchInst::Create(BasicBlock *IfTrue, BasicBlock *IfFalse, 
                              Value *Cond, 
                              Instruction *InsertBefore) {
    return new BranchInst(IfTrue, IfFalse, Cond, InsertBefore);
}

BranchInst *BranchInst::Create(BasicBlock *IfTrue, BasicBlock *IfFalse, 
                              Value *Cond, 
                              BasicBlock *InsertAtEnd) {
    auto *Res = Create(IfTrue, IfFalse, Cond);
    Res->insertInto(InsertAtEnd, InsertAtEnd->end());
    return Res;
}


PanicInst::PanicInst(Instruction *InsertBefore)
    : Instruction(Type::getUnitTy(), Instruction::Panic, { }, InsertBefore) {

}

PanicInst::PanicInst(BasicBlock *InsertAtEnd)
    : Instruction(Type::getUnitTy(), Instruction::Panic, { }, InsertAtEnd) {

}

PanicInst *PanicInst::Create(Instruction *InsertBefore) {
    return new PanicInst(InsertBefore);
}

PanicInst *PanicInst::Create(BasicBlock *InsertAtEnd) {
    auto *Res = Create();
    Res->insertInto(InsertAtEnd, InsertAtEnd->end());
    return Res;
}

BasicBlock::BasicBlock(Function *Parent, BasicBlock *InsertBefore)
    : Parent(nullptr) {
    if (Parent) {
        insertInto(Parent, InsertBefore);
    } else {
        assert(!InsertBefore && "Cannot insert before another basic block with no function!");
    }
}

void BasicBlock::insertInto(Function *NewParent, BasicBlock *InsertBefore) {
    assert(NewParent && "Expected a parent function!");
    assert(!Parent && "BasicBlock already has a parent!");

    if (InsertBefore) {
        NewParent->getBasicBlockList().insert(Function::iterator(InsertBefore), this);
    } else {
        NewParent->getBasicBlockList().insert(NewParent->end(), this);
    }
    Parent = NewParent;
}

BasicBlock *BasicBlock::Create(Function *Parent, BasicBlock *InsertBefore) {
    return new BasicBlock(Parent, InsertBefore);
}


void BasicBlock::setParent(Function *Parent) {
    this->Parent = Parent;
}

bool BasicBlock::hasName() const {
    return !Name.empty();
}


void BasicBlock::setName(std::string_view Name) {
    this->Name = Name;
}


Argument::Argument(Type *Ty, Function *Parent, unsigned ArgNo)
    : Value(Ty, Value::ArgumentVal), Parent(Parent), ArgNo(ArgNo) {
}

Function::Function(FunctionType *FTy, bool ExternalLinkage,
                   std::string_view Name, Module *M)
    : FTy(FTy), NumArgs(FTy->getNumParams()),
      ExternalLinkage(ExternalLinkage), Name(Name), Parent(M) {
    // check return type.
    assert(FunctionType::isValidReturnType(getReturnType()) &&
            "invalid return type");
    // build parameters and check parameter types.
    if (NumArgs > 0) {
        Arguments = std::allocator<Argument>().allocate(NumArgs);
        for (unsigned i = 0, e = NumArgs; i != e; ++i) {
            Type *ArgTy = FTy->getParamType(i);
            assert(FunctionType::isValidArgumentType(ArgTy) && "Badly typed arguments!");
            new (Arguments + i) Argument(ArgTy, this, i);
        }
    }

    if (M) {
        M->getFunctionList().push_back(this);
        if (hasName())
            M->SymbolFunctionMap[getName()] = this;
    }
}

Function::~Function() {
    if (NumArgs > 0) {
        for (unsigned i = 0, e = NumArgs; i != e; ++i) {
            Arguments[i].~Argument();
        }
        std::allocator<Argument>().deallocate(Arguments, NumArgs);
        Arguments = nullptr;
    }
    // remove symbol table entry.
    if (Parent) {
        Parent->SymbolFunctionMap.erase(getName());
    }
}

Function *Function::Create(FunctionType *FTy, bool ExternalLinkage, 
                           std::string_view Name, Module *M) {
    return new Function(FTy, ExternalLinkage, Name, M);
}

Function::iterator Function::insert(Function::iterator Position, BasicBlock *BB) {
    Function::iterator FIT = getBasicBlockList().insert(Position, BB);
    BB->setParent(this);
    return FIT;
}

bool Function::hasName() const {
    return !Name.empty();
}

GlobalVariable::GlobalVariable(Type *EleTy, std::size_t NumElements, bool ExternalLinkage,
                               std::string_view Name, Module *M)
    : Value(PointerType::get(EleTy), Value::GlobalVariableVal),
      EleTy(EleTy), NumElements(NumElements),
      ExternalLinkage(ExternalLinkage), Name(Name), Parent(M) {
    assert(!EleTy->isUnitTy() && "Cannot create global variable of () type!");
    if (M) {
        M->getGlobalList().push_back(this);
        if (hasName())
            M->SymbolGlobalMap[getName()] = this;
    }
}

GlobalVariable *GlobalVariable::Create(Type *EleTy, std::size_t NumElements, bool ExternalLinkage,
                                        std::string_view Name, Module *M) {
    return new GlobalVariable(EleTy, NumElements, ExternalLinkage, Name, M);
}

bool GlobalVariable::hasName() const {
    return !Name.empty();
}

Function *Module::getFunction(const std::string_view Name) const {
    auto IT = SymbolFunctionMap.find(Name);
    if (IT != SymbolFunctionMap.end())
        return IT->second;
    return nullptr;
}

GlobalVariable *Module::getGlobalVariable(const std::string_view Name) const {
    auto IT = SymbolGlobalMap.find(Name);
    if (IT != SymbolGlobalMap.end())
        return IT->second;
    return nullptr;
}