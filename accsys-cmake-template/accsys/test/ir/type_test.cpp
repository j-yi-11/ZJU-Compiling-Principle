#include "ir/type.h"
#include "utils/casting.h"

#include "gtest/gtest.h"


TEST(TypeTest, PrimitiveTypeTest) {
    // Test the primitive type
    Type *IntegerType = Type::getIntegerTy();
    Type *UnitTy = Type::getUnitTy();

    ASSERT_EQ(IntegerType->getTypeID(), Type::IntegerTyID);
    ASSERT_TRUE(IntegerType->isIntegerTy());
    ASSERT_EQ(UnitTy->getTypeID(), Type::UnitTyID);
    ASSERT_TRUE(UnitTy->isUnitTy());
}

TEST(TypeTest, PointerTypeTest) {
    // Test the pointer type
    Type *IntegerType = Type::getIntegerTy();
    // cast to Type * type.
    PointerType *PtrIntType = PointerType::get(IntegerType);
    ASSERT_EQ(PtrIntType->getElementType(), IntegerType);

    // up-casting
    Type *OpaqueType = PtrIntType;
    ASSERT_TRUE(OpaqueType->isPointerTy());
    ASSERT_EQ(OpaqueType->getTypeID(), Type::PointerTyID);
    ASSERT_TRUE(isa<PointerType>(OpaqueType));
    ASSERT_EQ(dyn_cast<PointerType>(OpaqueType)->getElementType(), IntegerType);
}


TEST(TypeTest, FunctionTypeTest) {
    // Test the function type
    Type *IntegerType = Type::getIntegerTy();
    Type *UnitTy = Type::getUnitTy();
    std::vector<Type *> Params = {IntegerType, UnitTy};
    FunctionType *FuncType = FunctionType::get(UnitTy, Params);

    ASSERT_EQ(FuncType->getNumParams(), 2);
    ASSERT_EQ(FuncType->getParamType(0), IntegerType);
    ASSERT_EQ(FuncType->getParamType(1), UnitTy);
    ASSERT_EQ(FuncType->getReturnType(), UnitTy);
    ASSERT_TRUE(isa<FunctionType>(FuncType));
}