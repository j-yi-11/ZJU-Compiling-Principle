#include "ir/type.h"
#include "ir/ir.h"
#include "utils/casting.h"

#include "gtest/gtest.h"
#include <cstddef>
#include <optional>
#include <vector>


TEST(InstructionTest, AddTest) {
    // Test the add instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Add = BinaryInst::CreateAdd(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Add->getOperand(0), V1);
    ASSERT_EQ(Add->getOperand(1), V2);
    ASSERT_EQ(Add->getType(), IntegerType);
    ASSERT_EQ(Add->getOpcode(), Instruction::Add);
    ASSERT_TRUE(isa<BinaryInst>(Add));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Add;
    delete V1;
    delete V2;
}

TEST(InstructionTest, SubTest) {
    // Test the sub instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Sub = BinaryInst::CreateSub(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Sub->getOperand(0), V1);
    ASSERT_EQ(Sub->getOperand(1), V2);
    ASSERT_EQ(Sub->getType(), IntegerType);
    ASSERT_EQ(Sub->getOpcode(), Instruction::Sub);
    ASSERT_TRUE(isa<BinaryInst>(Sub));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Sub;
    delete V1;
    delete V2;
}

TEST(InstructionTest, MulTest) {
    // Test the mul instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Mul = BinaryInst::CreateMul(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Mul->getOperand(0), V1);
    ASSERT_EQ(Mul->getOperand(1), V2);
    ASSERT_EQ(Mul->getType(), IntegerType);
    ASSERT_EQ(Mul->getOpcode(), Instruction::Mul);
    ASSERT_TRUE(isa<BinaryInst>(Mul));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Mul;
    delete V1;
    delete V2;
}

TEST(InstructionTest, DivTest) {
    // Test the div instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Div = BinaryInst::CreateDiv(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Div->getOperand(0), V1);
    ASSERT_EQ(Div->getOperand(1), V2);
    ASSERT_EQ(Div->getType(), IntegerType);
    ASSERT_EQ(Div->getOpcode(), Instruction::Div);
    ASSERT_TRUE(isa<BinaryInst>(Div));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Div;
    delete V1;
    delete V2;
}

TEST(InstructionTest, RemTest) {
    // Test the rem instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Rem = BinaryInst::CreateMod(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Rem->getOperand(0), V1);
    ASSERT_EQ(Rem->getOperand(1), V2);
    ASSERT_EQ(Rem->getType(), IntegerType);
    ASSERT_EQ(Rem->getOpcode(), Instruction::Mod);
    ASSERT_TRUE(isa<BinaryInst>(Rem));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Rem;
    delete V1;
    delete V2;
}

TEST(InstructionTest, LtTest) {
    // Test the lt instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Lt = BinaryInst::CreateLt(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Lt->getOperand(0), V1);
    ASSERT_EQ(Lt->getOperand(1), V2);
    ASSERT_EQ(Lt->getType(), IntegerType);
    ASSERT_EQ(Lt->getOpcode(), Instruction::Lt);
    ASSERT_TRUE(isa<BinaryInst>(Lt));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Lt;
    delete V1;
    delete V2;
}

TEST(InstructionTest, GtTest) {
    // Test the gt instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Gt = BinaryInst::CreateGt(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Gt->getOperand(0), V1);
    ASSERT_EQ(Gt->getOperand(1), V2);
    ASSERT_EQ(Gt->getType(), IntegerType);
    ASSERT_EQ(Gt->getOpcode(), Instruction::Gt);
    ASSERT_TRUE(isa<BinaryInst>(Gt));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Gt;
    delete V1;
    delete V2;
}

TEST(InstructionTest, LeTest) {
    // Test the le instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Le = BinaryInst::CreateLe(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Le->getOperand(0), V1);
    ASSERT_EQ(Le->getOperand(1), V2);
    ASSERT_EQ(Le->getType(), IntegerType);
    ASSERT_EQ(Le->getOpcode(), Instruction::Le);
    ASSERT_TRUE(isa<BinaryInst>(Le));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Le;
    delete V1;
    delete V2;
}

TEST(InstructionTest, GeTest) {
    // Test the ge instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Ge = BinaryInst::CreateGe(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Ge->getOperand(0), V1);
    ASSERT_EQ(Ge->getOperand(1), V2);
    ASSERT_EQ(Ge->getType(), IntegerType);
    ASSERT_EQ(Ge->getOpcode(), Instruction::Ge);
    ASSERT_TRUE(isa<BinaryInst>(Ge));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Ge;
    delete V1;
    delete V2;
}

TEST(InstructionTest, EqTest) {
    // Test the eq instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Eq = BinaryInst::CreateEq(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Eq->getOperand(0), V1);
    ASSERT_EQ(Eq->getOperand(1), V2);
    ASSERT_EQ(Eq->getType(), IntegerType);
    ASSERT_EQ(Eq->getOpcode(), Instruction::Eq);
    ASSERT_TRUE(isa<BinaryInst>(Eq));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Eq;
    delete V1;
    delete V2;
}

TEST(InstructionTest, NeTest) {
    // Test the ne instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Ne = BinaryInst::CreateNe(V1, V2, IntegerType, (Instruction *)nullptr);
    ASSERT_EQ(Ne->getOperand(0), V1);
    ASSERT_EQ(Ne->getOperand(1), V2);
    ASSERT_EQ(Ne->getType(), IntegerType);
    ASSERT_EQ(Ne->getOpcode(), Instruction::Ne);
    ASSERT_TRUE(isa<BinaryInst>(Ne));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Ne;
    delete V1;
    delete V2;
}

TEST(InstructionTest, AllocaTest) {
    // Test the alloca test
    Type *IntegerType = Type::getIntegerTy();
    std::size_t NumElements = 100;
    AllocaInst *Alloca = AllocaInst::Create(IntegerType, NumElements);
    ASSERT_EQ(Alloca->getType(), PointerType::get(IntegerType));
    ASSERT_EQ(Alloca->getAllocatedType(), IntegerType);
    ASSERT_EQ(Alloca->getNumElements(), NumElements);
    ASSERT_EQ(Alloca->getOpcode(), Instruction::Alloca);
    ASSERT_TRUE(isa<AllocaInst>(Alloca));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Alloca;
}

TEST(InstructionTest, StoreTest) {
    // Test the store instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V = ConstantInt::Create(1);
    AllocaInst *Alloca = AllocaInst::Create(IntegerType, 1);
    StoreInst *Store = StoreInst::Create(V, Alloca);
    ASSERT_EQ(Store->getType(), Type::getUnitTy());
    ASSERT_EQ(Store->getNumUses(), 0);
    ASSERT_EQ(Store->getNumOperands(), 2);
    ASSERT_EQ(Store->getValueOperand(), V);
    ASSERT_EQ(Store->getPointerOperand(), Alloca);
    ASSERT_EQ(Store->getOpcode(), Instruction::Store);
    ASSERT_EQ(Store->getValueOperandType(), IntegerType);
    ASSERT_EQ(Store->getPointerOperandType(), PointerType::get(IntegerType));
    ASSERT_TRUE(isa<StoreInst>(Store));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Store;
    delete Alloca;
    delete V;
}


TEST(InstructionTest, LoadInst) {
    // Test the load instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *V = ConstantInt::Create(1);
    AllocaInst *Alloca = AllocaInst::Create(IntegerType, 1);
    StoreInst *Store = StoreInst::Create(V, Alloca);
    LoadInst *Load = LoadInst::Create(Alloca);
    ASSERT_EQ(Load->getType(), IntegerType);
    ASSERT_EQ(Load->getNumUses(), 0);
    ASSERT_EQ(Load->getNumOperands(), 1);
    ASSERT_EQ(Load->getPointerOperand(), Alloca);
    ASSERT_EQ(Load->getOpcode(), Instruction::Load);
    ASSERT_EQ(Load->getPointerOperandType(), PointerType::get(IntegerType));
    ASSERT_TRUE(isa<LoadInst>(Load));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Load;
    delete Store;
    delete Alloca;
    delete V;
}

TEST(InstructionTest, OffsetTest) {
    // Test the offset instruction
    Type *IntegerType = Type::getIntegerTy();
    Value *Index0 = ConstantInt::Create(1);
    Value *Index1 = ConstantInt::Create(2);
    Value *Index2 = ConstantInt::Create(3);
    AllocaInst *Alloca = AllocaInst::Create(IntegerType, 100);
    std::vector<Value *> Indices {Index0, Index1, Index2};
    std::vector<std::optional<std::size_t>> Bounds { std::nullopt, 4, 5 };
    OffsetInst *Offset = OffsetInst::Create(IntegerType, Alloca, 
        Indices,
        Bounds);
    ASSERT_EQ(Offset->getType(), PointerType::get(IntegerType));
    ASSERT_EQ(Offset->getNumUses(), 0);
    // 1 ptr, 3 indices
    ASSERT_EQ(Offset->getNumOperands(), 4);
    ASSERT_EQ(Offset->getPointerOperand(), Alloca);
    ASSERT_EQ(Offset->getPointerOperandType(), PointerType::get(IntegerType));
    ASSERT_EQ(Offset->getElementType(), IntegerType);
    std::size_t ConstOffset = 0;
    Offset->accumulateConstantOffset(ConstOffset);
    ASSERT_EQ(ConstOffset, 1 * 4 * 5 + 2 * 5 + 3 * 1);
    ASSERT_EQ(Offset->getOpcode(), Instruction::Offset);
    ASSERT_TRUE(isa<OffsetInst>(Offset));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Offset;
    delete Alloca;
    delete Index0;
    delete Index1;
    delete Index2;
}


TEST(InstructionTest, CallTest) {
    // Test the call instruction
    Type *IntegerType = Type::getIntegerTy();
    Type *UnitType = Type::getUnitTy();
    FunctionType *FT = FunctionType::get(UnitType, {IntegerType, IntegerType});
    Function *F = Function::Create(FT);
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    CallInst *Call = CallInst::Create(F, {V1, V2});
    ASSERT_EQ(Call->getType(), UnitType);
    ASSERT_EQ(Call->getNumUses(), 0);
    // Function is not the operands in accipit IR, but two arguements are operands
    // of the call instruction.
    ASSERT_EQ(Call->getNumOperands(), 2);
    ASSERT_EQ(Call->getCallee(), F);
    ASSERT_EQ(Call->getOperand(0), V1);
    ASSERT_EQ(Call->getOperand(1), V2);
    ASSERT_EQ(Call->getOpcode(), Instruction::Call);
    ASSERT_TRUE(isa<CallInst>(Call));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete Call;
    delete F;
    delete V1;
    delete V2;
}


TEST(InstructionTest, RetTest) {
    // Test the ret instruction
    Type *IntegerType = Type::getIntegerTy();
    Type *UnitType = Type::getUnitTy();
    FunctionType *FT = FunctionType::get(UnitType, {IntegerType, IntegerType});
    Function *F = Function::Create(FT);
    // Create a basic block as the container of return instruction.
    BasicBlock *BB = BasicBlock::Create(F);
    // Check basic block is inserted into the function.
    ASSERT_EQ(&F->getEntryBlock(), BB);
    Value *V = ConstantUnit::Create();
    RetInst *Ret = RetInst::Create(V, BB);
    // Type of return instruction makes no sense.
    ASSERT_EQ(Ret->getNumUses(), 0);
    ASSERT_EQ(Ret->getNumOperands(), 1);
    ASSERT_EQ(Ret->getReturnValue(), V);
    ASSERT_EQ(Ret->getParent(), BB);
    ASSERT_EQ(Ret->getOpcode(), Instruction::Ret);
    ASSERT_TRUE(isa<RetInst>(Ret));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete F;
    /// FIXME: constant data is not managed by InstList.
    /// Currently you have to manually delete it. 
    delete V;
}

TEST(InstructionTest, JumpTest) {
    // Test the jump instruction
    Type *IntegerType = Type::getIntegerTy();
    Type *UnitType = Type::getUnitTy();
    FunctionType *FT = FunctionType::get(UnitType, {IntegerType, IntegerType});
    Function *F = Function::Create(FT);
    // Create a basic block as the container of return instruction.
    BasicBlock *BB = BasicBlock::Create(F);
    // Check basic block is inserted into the function.
    ASSERT_EQ(&F->getEntryBlock(), BB);
    // Insert a jumo destination block.
    BasicBlock *DestBB = BasicBlock::Create(F);
    ASSERT_EQ(&F->front(), BB);
    ASSERT_EQ(&F->back(), DestBB);
    JumpInst *Jump = JumpInst::Create(DestBB, BB);
    ASSERT_EQ(Jump->getNumUses(), 0);
    ASSERT_EQ(Jump->getNumOperands(), 0);
    ASSERT_EQ(Jump->getDestBasicBlock(), DestBB);
    ASSERT_EQ(Jump->getParent(), BB);
    ASSERT_EQ(Jump->getOpcode(), Instruction::Jump);
    ASSERT_TRUE(isa<JumpInst>(Jump));
    // Manually delete the instruction.
    // In the real use case, the instruction will be managed by a InstList in a BasicBlock.
    delete F;
}

TEST(InstructionTest, BranchTest) {
    // Test the branch instruction
    Type *IntegerType = Type::getIntegerTy();
    Type *UnitType = Type::getUnitTy();
    FunctionType *FT = FunctionType::get(UnitType, {IntegerType, IntegerType});
    Function *F = Function::Create(FT);
    // Create a basic block as the container of return instruction.
    BasicBlock *BB = BasicBlock::Create(F);
    // Check basic block is inserted into the function.
    ASSERT_EQ(&F->getEntryBlock(), BB);
    // Insert a jumo destination block.
    BasicBlock *TrueBB = BasicBlock::Create(F);
    BasicBlock *FalseBB = BasicBlock::Create(F);
    ASSERT_EQ(&F->front(), BB);
    ASSERT_EQ(&F->back(), FalseBB);
    // In BB do comparison.
    Value *V1 = ConstantInt::Create(1);
    Value *V2 = ConstantInt::Create(2);
    BinaryInst *Lt = BinaryInst::CreateLt(V1, V2, IntegerType, BB);
    BranchInst *Branch = BranchInst::Create(TrueBB, FalseBB, Lt, BB);
    ASSERT_EQ(Lt->getNumUses(), 1);
    // cond operand
    ASSERT_EQ(Branch->getNumOperands(), 1);
    ASSERT_EQ(Branch->getTrueBB(), TrueBB);
    ASSERT_EQ(Branch->getFalseBB(), FalseBB);
    ASSERT_EQ(Branch->getCondition(), Lt);
    ASSERT_EQ(Branch->getParent(), BB);
    ASSERT_EQ(Branch->getOpcode(), Instruction::Br);
    ASSERT_TRUE(isa<BranchInst>(Branch));

    delete F;
    delete V1;
    delete V2;
}