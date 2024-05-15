#pragma once


#include <unordered_map>
#include <utility>
#include <vector>


class PointerType;
class FunctionType;

/// Buffer class mamanges the memory of the derived types.
/// It serves as a type context in single threaded environment.
template <typename KeyTy, typename V>
class Buffer {
public:
    Buffer() = default;
    ~Buffer() {
        for (auto &[Key, Val]: buffer) {
            Val->~V();
            std::allocator<V>().deallocate(Val, 1);
        }
    }
protected:
    using BufferType = std::unordered_map<KeyTy, V *>;
    BufferType buffer;
public:
    using iterator = typename BufferType::iterator;
    std::pair<iterator, bool> insert_as(const KeyTy &Key) {
        V *DT = nullptr;
        auto Insertion = buffer.insert(std::make_pair(Key, nullptr));
        if (Insertion.second) {
            DT = std::allocator<V>().allocate(1);
            Insertion.first->second = DT;
        } else {
            DT = Insertion.first->second;
        }
        return Insertion;
    }
};

/// Type represents the primitive type of Accipit IR and
/// common interfaces
class Type {
public:
    Type(const Type &) = delete;
    Type &operator=(const Type &) = delete;
public:
    enum TypeID {
        // Primitive Types
        IntegerTyID,
        UnitTyID,
        // Derived Types
        PointerTyID,
        FunctionTyID,
    };
protected:
    TypeID ID;

    static Type IntegerTy, UnitTy;
    /// FIXME: `FunctionType` should be cached in the context. This
    /// is just a naive implementation. C++ 17 does not support transparant 
    /// key search for hash set. We have to contruct a temporary function
    /// type is this case, which is not intended.
    static thread_local unsigned FunctionTypeHandle;
    static thread_local Buffer<unsigned, FunctionType> FunctionTypes;
    static thread_local Buffer<Type *, PointerType> PointerTypes;

    // Type context should be handled by context.
    explicit Type(TypeID tid) : ID(tid){}
    ~Type() = default;

public:
    unsigned getTypeID() const { return ID; }

    bool isIntegerTy() const { return getTypeID() == IntegerTyID; }
    bool isUnitTy() const { return getTypeID() == UnitTyID; }
    bool isPointerTy() const { return getTypeID() == PointerTyID; }
    bool isFunctionTy() const { return getTypeID() == FunctionTyID; }
    /// Return true if the type is "first class", meaning it is a valid type for a real Value.
    /// e.g. You cannot pass a function type or unit type as an argument to a function.
    bool isFirstClassType() const {
        return getTypeID() != FunctionTyID && getTypeID() != UnitTyID;
    }

    /// Get the primitive types by type id.
    static Type *getPrimitiveTy(unsigned tid);
    static Type *getIntegerTy();
    static Type *getUnitTy();
    /// Shorthand methods handling derived method.
    /// e.g. getPointerElementType = cast<PointerType>()->getElementType().
    Type *getPointerElementType() const;
    Type *getFunctionParamType (unsigned index) const;
    unsigned getFunctionNumParams () const;
};

class PointerType: public Type {
    PointerType(Type *EltType);
public:
    PointerType(const PointerType &) = delete;
    PointerType &operator=(const PointerType &) = delete;
protected:
    Type *PointeeType;
public:
    Type *getElementType() const { return PointeeType; }

    static PointerType *get(Type *ElementType);
    static bool classof(const Type *Ty) {
        return Ty->getTypeID() == PointerTyID;
    }
}
;

class FunctionType: public Type {
    FunctionType(Type *Result, const std::vector<Type *> &Params);
public:
    FunctionType(const FunctionType &) = delete;
    FunctionType &operator=(const FunctionType &) = delete;
protected:

    std::vector<Type *> Params;
    Type *Result;
public:
    const std::vector<Type *> &params() const { return Params; }
    Type *getParamType(unsigned index) const { return Params.at(index); }
    unsigned getNumParams() const { return Params.size(); }
    Type *getReturnType() const { return Result; }

    /// This static method is the primary way of constructing a `FunctionType`.
    static FunctionType *get(Type *Result, const std::vector<Type *> &Params);
    
    /// Construct a `FunctionType` taking no parameters.
    static FunctionType *get(Type *Result);

    /// Return true if the specified type is valid as a return type.
    static bool isValidReturnType(Type *RetTy);
    /// Return true if the specified type is valid as an argument type.
    static bool isValidArgumentType(Type *ArgTy);

    static bool classof(const Type *Ty) {
        return Ty->getTypeID() == FunctionTyID;
    }
};
