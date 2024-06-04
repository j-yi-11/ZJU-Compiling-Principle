#pragma once

#include <type_traits>
#include <cassert>

// The core implmentation of isa<X>.
template <typename To, typename From, typename Enabler = void>
struct isa_impl {
    static inline bool doit(const From &Val) { return To::classof(&Val); }
};

// Always allow upcasting
template <typename To, typename From>
struct isa_impl<To, From, std::enable_if_t<std::is_base_of_v<To, From>>> {
    static inline bool doit(const From &) { return true; }
};


// Handling const & pointer decoration.
template <typename To, typename From>
struct isa_impl_cl {
    static inline bool doit(const From &Val) {
        return isa_impl<To, From>::doit(Val);
    }
};

template <typename To, typename From>
struct isa_impl_cl<To, From *> {
    static inline bool doit(const From *Val) {
        return isa_impl<To, From>::doit(*Val);
    }
};

template <typename To, typename From>
struct isa_impl_cl<To, From *const> {
    static inline bool doit(const From *Val) {
        return isa_impl<To, From>::doit(*Val);
    }
};

template <typename To, typename From>
struct isa_impl_cl<To, const From *> {
    static inline bool doit(const From *Val) {
        return isa_impl<To, From>::doit(*Val);
    }
};

template <typename To, typename From>
struct isa_impl_cl<To, const From *const> {
    static inline bool doit(const From *Val) {
        return isa_impl<To, From>::doit(*Val);
    }
};

template <typename To, typename FromTy>
struct isa_wrap {
    static inline bool doit(const FromTy &Val) {
        return isa_impl_cl<To, FromTy>::doit(Val);
    }
};


// Calculate what type the 'cast' function should return, based on a requested
// type of To and a source type of From.
template <class To, class From>
struct cast_retty_impl {
    using ret_type = To &; // Normal case, return Ty&
};
template <class To, class From>
struct cast_retty_impl<To, const From> {
    using ret_type = const To &; // Normal case, return Ty&
};

template <class To, class From>
struct cast_retty_impl<To, From *> {
    using ret_type = To *; // Pointer arg case, return Ty*
};

template <class To, class From>
struct cast_retty_impl<To, const From *> {
    using ret_type = const To *; // Constant pointer arg case, return const Ty*
};

template <class To, class From>
struct cast_retty_impl<To, const From *const> {
    using ret_type = const To *; // Constant pointer arg case, return const Ty*
};

template <typename To, typename From>
struct cast_retty {
    using ret_type = typename cast_retty_impl<To, From>::ret_type;
};


/// implementation of casting
template <class To, class FromTy>
struct cast_convert_val{
  // If it's a reference, switch to a pointer to do the cast and then deref it.
    static typename cast_retty<To, FromTy>::ret_type doit(const FromTy &Val) {
        return *(std::remove_reference_t<typename cast_retty<To, FromTy>::ret_type>
                 *)&const_cast<FromTy &>(Val);
    }
};

template <class To, class FromTy>
struct cast_convert_val<To, FromTy *> {
    // If it's a pointer, we can use c-style casting directly.
    static typename cast_retty<To, FromTy *>::ret_type doit(const FromTy *Val) {
        return (typename cast_retty<To, FromTy *>::ret_type) const_cast<FromTy *>(
            Val);
    }
};

/// isa<X> - Return true if the parameter to the template is an instance of one
/// of the template type arguments.  Used like this:
///
///  if (isa<Type>(myVal)) { ... }
///  if (isa<Type0, Type1, Type2>(myVal)) { ... }
template <typename To, typename From>
[[nodiscard]] bool isa(const From &Val) {
    return isa_wrap<To, const From>::doit(Val);
}

template <typename First, typename Second, typename... Rest, typename From>
[[nodiscard]] bool isa(const From &Val) {
    return isa<First>(Val) || isa<Second, Rest...>(Val);
}


/// dyn_cast<X> - Return the argument parameter cast to the specified type. This
/// casting operator returns null if the argument is of the wrong type, so it
/// can be used to test for a type as well as cast if successful. This should be
/// used in the context of an if statement like this:
///
///  if (const Instruction *I = dyn_cast<Instruction>(myVal)) { ... }
template <typename To, typename From>
typename cast_retty<To, const From>::ret_type
dyn_cast(const From &Val) {
    if (isa<To>(Val)) {
        return cast_convert_val<To, const From>::doit(const_cast<From &>(Val));
    }
    return nullptr;
}

template <typename To, typename From>
typename cast_retty<To, From>::ret_type
dyn_cast(From &Val) {
    if (isa<To>(Val)) {
        return cast_convert_val<To, From>::doit(Val);
    }
    return nullptr;
}

template <typename To, typename From>
typename cast_retty<To, From *>::ret_type
dyn_cast(From *Val) {
    if (isa<To>(Val)) {
        return cast_convert_val<To, From *>::doit(Val);
    }
    return nullptr;
}

/// checked cast.
/// cast<X> - Return the argument parameter cast to the specified type.  This
/// casting operator asserts that the type is correct, so it does not return
/// null on failure. It is typically used like this:
///
///  cast<Instruction>(myVal)->getParent()
template <typename To, typename From>
decltype(auto) cast(const From &Val) {
    assert(isa<To>(Val) && "cast<Ty>() argument of incompatible type!");
    return cast_convert_val<To, From>::doit(const_cast<From &>(Val));
}

template <typename To, typename From>
decltype(auto) cast(From &Val) {
    assert(isa<To>(Val) && "cast<Ty>() argument of incompatible type!");
    return cast_convert_val<To, From>::doit((Val));
}

template <typename To, typename From>
decltype(auto) cast(From *Val) {
    assert(isa<To>(Val) && "cast<Ty>() argument of incompatible type!");
    return cast_convert_val<To, From *>::doit((Val));
}