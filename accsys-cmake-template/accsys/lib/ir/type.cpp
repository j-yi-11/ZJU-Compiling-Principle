#include "ir/type.h"
#include "utils/casting.h"

#include <stdexcept>

Type Type::IntegerTy(Type::IntegerTyID);
Type Type::UnitTy(Type::UnitTyID);
thread_local unsigned Type::FunctionTypeHandle = 0;
thread_local Buffer<unsigned, FunctionType> Type::FunctionTypes;
thread_local Buffer<Type *, PointerType> Type::PointerTypes;

Type *Type::getIntegerTy() {
    return &Type::IntegerTy;
}

Type *Type::getUnitTy() {
    return &Type::UnitTy;
}

Type *Type::getPrimitiveTy(unsigned tid) {
    switch (tid) {
    case Type::IntegerTyID: return getIntegerTy();
    case Type::UnitTyID: return getUnitTy();
    default:
        throw std::runtime_error("The type id is not of the primitive type!");
    }
}

Type *Type::getPointerElementType() const {
    return cast<PointerType>(this)->getElementType();
}

Type *Type::getFunctionParamType(unsigned index) const {
    return cast<FunctionType>(this)->getParamType(index);
}

unsigned Type::getFunctionNumParams() const {
    return cast<FunctionType>(this)->getNumParams();
}

PointerType::PointerType(Type *EltType)
    : Type(Type::PointerTyID), PointeeType(EltType) {}

PointerType *PointerType::get(Type *PointerElementType) {
    auto Insertion = Type::PointerTypes.insert_as(PointerElementType);

    PointerType *PTy = Insertion.first->second;
    if (Insertion.second) {
        new (PTy) PointerType(PointerElementType);
    }
    return PTy;
}

FunctionType::FunctionType(Type *Result, const std::vector<Type *> &Params)
    : Type(Type::FunctionTyID), Params(Params), Result(Result) { }

FunctionType *FunctionType::get(Type *Result, const std::vector<Type *> &Params) {
    auto Insertion = Type::FunctionTypes.insert_as(Type::FunctionTypeHandle++);

    FunctionType *FTy = Insertion.first->second;
    new (FTy) FunctionType(Result, Params);
    return FTy;
}

FunctionType *FunctionType::get(Type *Result) {
    return FunctionType::get(Result, {});
}

bool FunctionType::isValidReturnType(Type *RetTy) {
    return !RetTy->isFunctionTy();
}

bool FunctionType::isValidArgumentType(Type *ArgTy) {
    return ArgTy->isFirstClassType();
}