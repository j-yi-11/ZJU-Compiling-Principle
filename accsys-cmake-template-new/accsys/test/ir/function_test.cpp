#include "ir/type.h"
#include "ir/ir.h"

#include "gtest/gtest.h"

#include <memory>
#include <iostream>

TEST(FunctionTest, ArgumentTest) {
    // Test the argument
    Type *IntegerType = Type::getIntegerTy();
    Type *UnitType = Type::getUnitTy();
    FunctionType *FT = FunctionType::get(UnitType, {IntegerType, PointerType::get(IntegerType)});
    Function *F = Function::Create(FT);
    ASSERT_EQ(F->getArg(0)->getArgNo(), 0);
    ASSERT_EQ(F->getArg(0)->getType(), IntegerType);
    ASSERT_EQ(F->getArg(1)->getArgNo(), 1);
    ASSERT_EQ(F->getArg(1)->getType(), PointerType::get(IntegerType));
    ASSERT_EQ(F->arg_size(), 2);
    // Manually delete the Function.
    // In the real use case, the instruction will be managed by a InstList in a Module.
    delete F;
}

TEST(FunctionTest, FunctionTypeTest) {
    // Test the argument
    Type *IntegerType = Type::getIntegerTy();
    Type *UnitType = Type::getUnitTy();
    FunctionType *FT = FunctionType::get(UnitType, {IntegerType, PointerType::get(IntegerType)});
    Function *F = Function::Create(FT);
    /// FIXME: The function type pointer 'equal' is purely pointer equality.
    /// Our type context currently does not support funtion type caching.
    // ASSERT_EQ(F->getFunctionType(), FT);
    ASSERT_EQ(F->getReturnType(), UnitType);
    ASSERT_EQ(F->getNumParams(), 2);

    delete F;
}

TEST(FunctionTest, FactorialTest) {
    // Test the factorial function
    Type *IntegerType = Type::getIntegerTy();
    Type *UnitType = Type::getUnitTy();
    auto M = std::make_unique<Module>();
    FunctionType *FT = FunctionType::get(IntegerType, {IntegerType});
    Function *F = Function::Create(FT, false, "factorial", M.get());
    BasicBlock *Entry = BasicBlock::Create(F);
    BasicBlock *True = BasicBlock::Create(F);
    BasicBlock *False = BasicBlock::Create(F);
    BasicBlock *Exit = BasicBlock::Create(F);
    // Constants
    ConstantInt *Zero = ConstantInt::Create(0);
    ConstantInt *One = ConstantInt::Create(1);
    // Entry block
    // %ret.addr = alloca i32, 1
    AllocaInst *RetAddr = AllocaInst::Create(IntegerType, 1, Entry);
    // %n.addr = alloca i32, 1
    AllocaInst *NAddr = AllocaInst::Create(IntegerType, 1, Entry);
    // %4 = store #n, %n.addr
    StoreInst *NArgStore = StoreInst::Create(F->getArg(0), NAddr, Entry);
    // %ans.addr = alloca i32, 1
    AllocaInst *AnsAddr = AllocaInst::Create(IntegerType, 1, Entry);
    // %6 = load %n.addr
    LoadInst *NLoad = LoadInst::Create(NAddr, Entry);
    // %cmp = eq i32 %6, 0
    BinaryInst *Cmp = BinaryInst::CreateEq(NLoad, Zero, IntegerType, Entry);
    // br %cmp, label %Ltrue, label %Lfalse
    BranchInst *Br = BranchInst::Create(True, False, Cmp, Entry);
    // If true branch
    // %10 = store 1, %ret.addr
    StoreInst *OneStore = StoreInst::Create(One, RetAddr, True);
    // jmp label %Lret
    JumpInst *JTE = JumpInst::Create(Exit, True);
    // If false branch
    // n - 1
    // %13 = load %n.addr
    LoadInst *NLoad2 = LoadInst::Create(NAddr, False);
    // %14 = sub %13, 1
    BinaryInst *Sub = BinaryInst::CreateSub(NLoad2, One, IntegerType, False);
    // %res = call @factorial, %14
    CallInst *Call = CallInst::Create(F, {Sub}, False);
    // n
    // %16 = load %n.addr
    LoadInst *NLoad3 = LoadInst::Create(NAddr, False);
    // %17 = mul %15, %res
    BinaryInst *Mul = BinaryInst::CreateMul(NLoad3, Call, IntegerType, False);
    // %18 = store %16, %ans.addr
    StoreInst *ResStore = StoreInst::Create(Mul, AnsAddr, False);
    // write ans to %ret.addr
    // %19 = load %ans.addr
    LoadInst *AnsLoad = LoadInst::Create(AnsAddr, False);
    // %20 = store %19, %ret.addr
    StoreInst *ResStore2 = StoreInst::Create(AnsLoad, RetAddr, False);
    // jmp label %Lret
    JumpInst *JFE = JumpInst::Create(Exit, False);
    // Exit block
    // %ret.val = load %ret.addr
    LoadInst *ResLoad2 = LoadInst::Create(RetAddr, Exit);
    // ret %ret.val
    RetInst *Ret = RetInst::Create(ResLoad2, Exit);

    // checks
    // basic block terminators.
    ASSERT_EQ(Entry->getTerminator(), Br);
    ASSERT_EQ(True->getTerminator(), JTE);
    ASSERT_EQ(False->getTerminator(), JFE);
    ASSERT_EQ(Exit->getTerminator(), Ret);
    // instruction counts
    ASSERT_EQ(Entry->size(), 7);
    ASSERT_EQ(True->size(), 2);
    ASSERT_EQ(False->size(), 9);
    ASSERT_EQ(Exit->size(), 2);
    // uses check
    ASSERT_EQ(NAddr->getNumUses(), 4);
    ASSERT_EQ(RetAddr->getNumUses(), 3);
    ASSERT_EQ(AnsAddr->getNumUses(), 2);
    // Try printing the function.
    // M->print(std::cout, false);
    // Manually delete the Function.
    // In the real use case, the instruction will be managed by a InstList in a Module.
    delete One;
    delete Zero;
}


TEST(FunctionTest, GetintTest) {
    // Test the factorial function
    Type *IntegerType = Type::getIntegerTy();
    auto M = std::make_unique<Module>();
    FunctionType *FT = FunctionType::get(IntegerType);
    Function *Getint = Function::Create(FT, true, "getint", M.get());
    ASSERT_EQ(M->getFunction("getint"), Getint);
    Function *Getch = Function::Create(FT, true, "getch", M.get());
    ASSERT_EQ(M->getFunction("getch"), Getch);
    M->print(std::cout, true);
}


TEST(FunctionTest, LocalModuleTest) {
    // Test the factorial function
    Type *IntegerType = Type::getIntegerTy();
    Module M;
    FunctionType *FT = FunctionType::get(IntegerType);
    Function *Getint = Function::Create(FT, true, "getint", &M);
    ASSERT_EQ(M.getFunction("getint"), Getint);
    Function *Getch = Function::Create(FT, true, "getch", &M);
    ASSERT_EQ(M.getFunction("getch"), Getch);
    M.print(std::cout, true);
}