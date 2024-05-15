#pragma once

#include "ir/type.h"
#include "utils/list.h"
#include "utils/casting.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <string_view>


class Value;
class Instruction;
class Constant;
class BasicBlock;
class Function;
class Module;



/// \brief A use is a edge from the operands of a 'Value'
/// to the definition of the used 'Value', which maintains
/// 'use-def' chain.
/// The used value maintains a list of 'Use' objects
/// representing its users, while user maintains its operands
/// in a plain linear memory.
class Use {
public:
    Use() = delete;
    Use(const Use &) = delete;
    Use(Use &&) = delete;

    /// 'use-def' chain and 'def-use' chain are double linked list.
    void addToList(Use **);
    void removeFromList();


    operator Value *() const { return Val; }
    Value *get() const { return Val; }
    Value *getUser() const { return Parent; }

    Value *operator->() { return Val; }
    const Value *operator->() const { return Val; }
    /// set the used value 'Val' of a Use.
    void set(Value *V);
    Value *operator=(Value *RHS) {
        set(RHS);
        return RHS;
    }
    const Use &operator=(const Use &RHS) {
        set(RHS.Val);
        return *this;
    }
private:
    /// Priave constructor and destructor by design, 
    /// should be only called by subclasses of 'Value'.
    ~Use() {
        if (Val)
            removeFromList();
    }
    /// make emplace_back happy.
    explicit Use(Value *Parent);
private:
    // User value
    Value *Parent = nullptr;
    // Used value
    Value *Val = nullptr;
    // Next use in the list.
    Use *Next = nullptr;
    // Prev use in the list, reprensented by a list.
    Use **Prev = nullptr;
public:
    friend Value;
    friend Instruction;
    friend Constant;
};


class Value {
    const unsigned char SubclassID;
protected:
    Value(Type *Ty, unsigned scid);

public:
    Value(const Value &) = delete;
    Value &operator=(const Value &) = delete;
    virtual ~Value();

    /// Inner Use list operation, should be only be called by Use.
    void addUse(Use &U) { U.addToList(&UserList); }
    void removeUse(Use &U) { U.removeFromList(); }

    /// Traverse all the user of this 'Value'.
    /// 'def-use' chain maintained by a double linked list and
    /// represented by Use as well.
    struct UserView {
        struct user_iterator {
            Use *U;
            user_iterator &operator++() {
                U = U->Next;
                return *this;
            }
            bool operator==(const user_iterator &RHS) const {
                return U == RHS.U;
            }
            bool operator!=(const user_iterator &RHS) const {
                return U != RHS.U;
            }
            Use &operator*() const { return *U; }
            Use *operator->() const { return U; }
        };
        Use *UserList;
        decltype(auto) begin() const { return user_iterator {.U = UserList}; }
        decltype(auto) end() const {
            // null end iterator.
            return user_iterator {.U = nullptr};
        }
    };

    /// Return the user view of this value.
    [[nodiscard]] UserView getUserView() const { return UserView {.UserList = UserList}; }
    /// Return the number of uses of this value.
    /// Distinguish between Value's 'getNumUses' and Instructions's 'getNumOperands'.
    [[nodiscard]] unsigned getNumUses() const;
    /// \brief This is an important method if you'd like to write
    /// a optimization pass on Accipit IR. It replace all the uses 
    /// of this value with a new value V by tracing 'def-use' chain.
    /// This method ONLY ensures the updates of this value.
    void replaceAllUsesWith(Value *V);
private:
    Type *Ty;
    Use *UserList = nullptr;
    std::string Name;
public:
    /// Return the type of the value.
    Type *getType() const { return Ty; }
    /// Set the name of the value.
    void setName(std::string_view);
    /// Check if the value has a name, such as %name.
    /// By default, the name of a value is empty (annoymous value).
    /// Annoymous value will be assigned a slot number in IR print, such as %1.
    bool hasName() const;
    /// Return the name of the value.
    std::string_view getName() const { return Name; }

    /// Enumeration of the subclass value kinds.
    enum ValueKind {
#define ValueTypeDefine(subclass) subclass##Val,
#include "ir.def"

#define ConstantMarker(markname, constant) markname = constant##Val,
#include "ir.def"
    };

    /// Return an ID for the concrete type of Value.
    unsigned getValueID() const {
        return SubclassID;
    }

    friend Use;
};


/// Base class of all constants.
class Constant: public Value {
protected:
    Constant(Type *Ty, unsigned VT);
public:
    Constant(const Constant &) = delete;
    static bool classof(const Value *V) {
        return V->getValueID() >= ConstantFirstVal && V->getValueID() <= ConstantLastVal;
    }
};


class ConstantInt: public Constant {
    std::uint32_t value;
protected:
    explicit ConstantInt(std::uint32_t Val);
public:
    static ConstantInt *Create(std::uint32_t Val);

public:
    /// Return the integer value of the constant.
    std::uint32_t getValue() const { return value; }

    static bool classof(const Value *V) {
        return V->getValueID() == Value::ConstantIntVal;
    }
};

class ConstantUnit: public Constant {
protected:
    ConstantUnit();
public:
    static ConstantUnit *Create();
    
    static bool classof(const Value *V) {
        return V->getValueID() == Value::ConstantUnitVal;
    }
};


/// Base class of all value binding and terminator instructions.
class Instruction: public Value, 
                   public ListNode<Instruction> {
public:
    Instruction(const Instruction &) = delete;
    Instruction &operator=(const Instruction &) = delete;
    ~Instruction() override;

    // All instructions has two explicit static constructing methods,
    // specifying the instruction parameters, operands and the insertion position.
    // The first one is to insert the instruction before a specified instruction.
    // The second one is to insert the instruction at the end of a specified basic block.
    // Basically, the constructor of Instruction is private, and the only way to create
    // an instruction is to call the static method 'Create'.
    // The memory management is handled implicitly by intrusive lists.
    Instruction(Type *Ty, unsigned Opcode, 
                const std::vector<Value *> &Ops,
                Instruction *InsertBefore);
    Instruction(Type *Ty, unsigned Opcode, 
                const std::vector<Value *> &Ops,
                BasicBlock *InsertAtEnd);

    using InstListType = List<Instruction>;
    using op_iterator = Use *;
    using const_op_iterator = const Use *;
private:
    unsigned NumUserOperands;
    Use *Uses = nullptr;
    BasicBlock *Parent = nullptr;

    void setParent(BasicBlock *BB);
public:
    /// Insert an unlinked instruction into a basic block immediately before the specified instruction.
    void insertBefore(Instruction *InsertPos);
    void insertBefore(InstListType::iterator InsertPos);
    /// Inserts an unlinked instruction into ParentBB at position It and returns the iterator of the inserted instruction.
    void insertBefore(BasicBlock &BB, InstListType::iterator IT);
    /// Insert an unlinked instruction into a basic block immediately after the specified instruction.
    void insertAfter(Instruction *InsertPos);
    /// Inserts an unlinked instruction into ParentBB at position It and returns the iterator of the inserted instruction.
    InstListType::iterator insertInto(BasicBlock *ParentBB, InstListType::iterator IT);

 	// This method unlinks 'this' from the containing basic block, but does not delete it.
    void removeFromParent();
 	// This method unlinks 'this' from the containing basic block and deletes it.
    InstListType::iterator eraseFromParent();

    const BasicBlock *getParent() const { return Parent; }
    BasicBlock *getParent() { return Parent; }
    /// Return the integer reptresentation of the instruction opcode enumeration,
    /// which can be 'BinaryOps', 'MemoryOps', 'TerminatorOps' or 'OtherOps'.
    unsigned getOpcode() const { return getValueID() - InstructionVal; }
    /// Return the operands (use information) of Instruction, represented by
    /// a 'Use' class.
    const Use *getOperandList() const { return Uses; }
    Use *getOperandList() { return Uses; }
    /// Return the i-th operand of the instruction.
    Value *getOperand(unsigned i) const {
        assert(i < getNumOperands() && "getOperand() out of range!");
        return getOperandList()[i];
    }
    const Use &getOperandUse(unsigned index) const {
        return getOperandList()[index];
    }
    Use &getOperandUse(unsigned index) {
        return getOperandList()[index];
    }
    /// Return the number of operands of the instruction.
    unsigned getNumOperands() const { return NumUserOperands; }
    /// Operands iteration.
    op_iterator op_begin() { return getOperandList(); }
    const_op_iterator op_begin() const { return getOperandList(); }
    op_iterator op_end() {
        return getOperandList() + NumUserOperands;
    }
    const_op_iterator op_end() const {
        return getOperandList() + NumUserOperands;
    }

    /// Check if the instruction has a binary opcode.
    bool isBinaryOp() const { return isBinaryOp(getOpcode()); }
    /// Check if the instruction has a terminator opcode.
    bool isTerminator() const { return isTerminator(getOpcode()); }

    static inline bool isBinaryOp(unsigned Opcode) {
        return Opcode >= BinaryOpsBegin && Opcode <= BinaryOpsEnd;
    }

    static inline bool isTerminator(unsigned Opcode) {
        return Opcode >= TerminatorOpsBegin && Opcode <= TerminatorOpsEnd;
    }

    static bool classof(const Value *V) {
        return V->getValueID() >= InstructionVal;
    }

    // Instruction Opcode enumerations.
    enum BinaryOps {
#define   FirstBinaryInst(Num)                   BinaryOpsBegin = Num,
#define BinaryInstDefine( Num, Opcode, Subclass) Opcode = Num,
#define   LastBinaryInst( Num)                   BinaryOpsEnd = Num,
#include "ir.def"     
    };

    enum MemoryOps {
#define   FirstMemoryInst(Num)                   MemoryOpsBegin = Num,
#define MemoryInstDefine( Num, Opcode, Subclass) Opcode = Num,
#define   LastMemoryInst( Num)                   MemoryOpsEnd = Num,
#include "ir.def"
    };

    enum TerminatorOps {
#define   FirstTerminator(Num)                   TerminatorOpsBegin = Num,
#define TerminatorDefine( Num, Opcode, Subclass) Opcode = Num,
#define   LastTerminator( Num)                   TerminatorOpsEnd = Num,
#include "ir.def"
    };

    enum OtherOps {
#define   FirstOtherInst(Num)                   OtherInstBegin = Num,
#define OtherInstDefine( Num, Opcode, Subclass) Opcode = Num,
#define   LastOtherInst( Num)                   OtherInstEnd = Num,
#include "ir.def"
    };    
};


class BinaryInst: public Instruction {
protected:
    BinaryInst(BinaryOps Op, Value *LHS, Value *RHS, Type* Ty,
               Instruction *InsertBefore);
    
    BinaryInst(BinaryOps Op, Value *LHS, Value *RHS, Type *Ty,
               BasicBlock *InsertAtEnd);
public:
    static BinaryInst *Create(BinaryOps Op, Value *LHS, Value *RHS, Type *Ty,
                              Instruction *InsertBefore = nullptr);
    
    static BinaryInst *Create(BinaryOps Op, Value *LHS, Value *RHS, Type *Ty,
                              BasicBlock *InsertAtEnd);

public:
    /// forward to Create, useful when you know what type of instruction you are going to create.
#define BinaryInstDefine(Num, Opcode, Subclass) \
    static BinaryInst *Create##Opcode(Value *LHS, Value *RHS, Type *Ty, Instruction *I) { \
        return Create(Instruction::Opcode, LHS, RHS, Ty, I); \
    }
#include "ir.def"
#define BinaryInstDefine(Num, Opcode, SubClass) \
    static BinaryInst *Create##Opcode(Value *LHS, Value *RHS, Type *Ty, BasicBlock *BB) { \
        return Create(Instruction::Opcode, LHS, RHS, Ty, BB); \
    }
#include "ir.def"

    static bool classof(const Value *V) { 
        return isa<Instruction>(V) && dyn_cast<Instruction>(V)->isBinaryOp();
    }
};

class AllocaInst: public Instruction {
protected:
    AllocaInst(Type *PointeeTy, std::size_t NumElements, Instruction *InsertBefore);
    AllocaInst(Type *PointeeTy, std::size_t NumElements, BasicBlock *InsertAtEnd);
private:
    Type *AllocatedType;
    std::size_t NumElements;
public:
    static AllocaInst *Create(Type *PointeeTy, std::size_t NumElements,
                              Instruction *InsertBefore = nullptr);
    static AllocaInst *Create(Type *PointeeTy, std::size_t NumElements,
                              BasicBlock *InsertAtEnd);
    /// Return the type of the allocated elements.
    Type *getAllocatedType() const { return AllocatedType; }
    /// Return the number of elements allocated.
    std::size_t getNumElements() const { return NumElements; }


    static bool classof(const Instruction *I) {
        return I->getOpcode() == Instruction::Alloca;
    }

    static bool classof(const Value *V) {
        return isa<Instruction>(V) && classof(cast<Instruction>(V));
    }
};

class StoreInst: public Instruction {
protected:
    StoreInst(Value *Val, Value *Ptr, Instruction *InsertBefore);
    StoreInst(Value *Val, Value *Ptr, BasicBlock *InsertAtEnd);
public:
    static StoreInst *Create(Value *Val, Value *Ptr,
                             Instruction *InsertBefore = nullptr);
    static StoreInst *Create(Value *Val, Value *Ptr,
                             BasicBlock *InsertAtEnd);
public:
    // The stored value operand of the store instruction.
    Value *getValueOperand() const { return getOperand(0); }
    Type *getValueOperandType() const { return getValueOperand()->getType(); }
    // The pointer operand of the store instruction.
    Value *getPointerOperand() const { return getOperand(1); }
    Type *getPointerOperandType() const { return getPointerOperand()->getType(); }

    static bool classof(const Instruction *I) {
        return I->getOpcode() == Instruction::Store;
    }

    static bool classof(const Value *V) {
        return isa<Instruction>(V) && classof(cast<Instruction>(V));
    }
};


class LoadInst: public Instruction {
protected:
    LoadInst(Value *Ptr, Instruction *InsertBefore);
    LoadInst(Value *Ptr, BasicBlock *InsertAtEnd);
public:
    static LoadInst *Create(Value *Ptr, Instruction *InsertBefore = nullptr);
    static LoadInst *Create(Value *Ptr, BasicBlock *InsertAtEnd);
    // The pointer operand of the load instruction.
    Value *getPointerOperand() const { return getOperand(0); }
    Type *getPointerOperandType() const { return getPointerOperand()->getType(); }

    static bool classof(const Instruction *I) {
        return I->getOpcode() == Instruction::Load;
    }

    static bool classof(const Value *V) {
        return isa<Instruction>(V) && classof(cast<Instruction>(V));
    }
};


class OffsetInst: public Instruction {
protected:
    OffsetInst(Type *PointeeTy,
               std::vector <Value *> &Ops,
               std::vector <std::optional<std::size_t>> &BoundList,
               Instruction *InsertBefore = nullptr);
    OffsetInst(Type *PointeeTy,
               std::vector <Value *> &Ops,
               std::vector <std::optional<std::size_t>> &BoundList,
               BasicBlock *InsertAtEnd);
private:
    Type *ElementTy;
    // Bounds are stored as explicit integer literal rather than operands, 
    // indices as uses of Value (inherited from class Instruction).
    // std::optional<std::uint32_t> is either 'nullopt' or a uint32_t value.
    // 'nullopt' stands for 'none' bound of the corresponding dimension.
    // See https://en.cppreference.com/w/cpp/utility/optional for more details.
    std::vector<std::optional<std::size_t>> Bounds;

    void AssertOK() const;
public:
    static OffsetInst *Create(Type *PointeeTy, Value *Ptr,
                              std::vector <Value *> &Indices,
                              std::vector <std::optional<std::size_t>> &Bounds,
                              Instruction *InsertBefore = nullptr);
    static OffsetInst *Create(Type *PointeeTy, Value *Ptr,
                              std::vector <Value *> &Indices,
                              std::vector <std::optional<std::size_t>> &Boundss,
                              BasicBlock *InsertAtEnd);

    using bound_iter = std::vector<std::optional<std::size_t>>::iterator;
    using const_bound_iter = std::vector<std::optional<std::size_t>>::const_iterator;


public:
    /// Return the element type of the offset instruction.
    Type *getElementType() const { return ElementTy; }
    // The base pointer operand of the offset instruction.
    Value *getPointerOperand() const { return getOperand(0); }
    Type *getPointerOperandType() const { return getPointerOperand()->getType(); }
    /// Return the i-th index operand.
    Value *getIndexOperand(unsigned i) const { return getOperand(i + 1); }
    // Iterations of bounds.
    std::vector<std::optional<std::size_t>> &bounds() { return Bounds; }
    const std::vector<std::optional<std::size_t>> &bounds() const { return Bounds; }
    std::optional<std::size_t> getBound(unsigned index) const {
        assert(index < Bounds.size() && "getBound() out of range!");
        return Bounds[index];
    }

	/// Accumulate the constant address offset by unit of element type if possible.
    /// This routine accepts an size_t into which it will try to accumulate the constant offset.
    /// Examples:
    /// int g[3][4][5], &g[1][2][3], we have i32, %g.addr, [1 < 3], [2 < 4], [3 < 5]
    /// accumulateConstantOffset(0) gets 0 + (1 * 4 * 5) + (2 * 5) + (3 * 1s) 
    bool accumulateConstantOffset(std::size_t &Offset) const;

    static bool classof(const Instruction *I) {
        return I->getOpcode() == Instruction::Offset;
    }

    static bool classof(const Value *V) {
        return isa<Instruction>(V) && classof(cast<Instruction>(V));
    }
};


class CallInst: public Instruction {
protected:
    CallInst(Function *Callee, const std::vector<Value *> &Args, Instruction *InsertBefore);
    CallInst(Function *Callee, const std::vector<Value *> &Args, BasicBlock *InsertAtEnd);
private:
    Function *Callee;
public:
    static CallInst *Create(Function *Callee, const std::vector<Value *> &Args, 
                            Instruction *InsertBefore = nullptr);
    static CallInst *Create(Function *Callee, const std::vector<Value *> &Args, 
                            BasicBlock *InsertAtEnd);
    /// Return the callee function of the call instruction.
    Function *getCallee() const { return Callee; }
    /// Return the i-th argument passed to the callee function.
    Value *getArgOperand(unsigned i) const { return getOperand(i); }

    static bool classof(const Instruction *I) {
        return I->getOpcode() == Instruction::Call;
    }

    static bool classof(const Value *V) {
        return isa<Instruction>(V) && classof(cast<Instruction>(V));
    }
};


class RetInst: public Instruction {
protected:
    RetInst(Value *RetVal, Instruction *InsertBefore);
    RetInst(Value *RetVal, BasicBlock *InsertAtEnd);
public:
    static RetInst *Create(Value *RetVal, Instruction *InsertBefore = nullptr);
    static RetInst *Create(Value *RetVal, BasicBlock *InsertAtEnd);
    /// Return the return value of the return instruction.
    Value *getReturnValue() const { return getOperand(0); }

    static bool classof(const Instruction *I) {
        return I->getOpcode() == Instruction::Ret;
    }

    static bool classof(const Value *V) {
        return isa<Instruction>(V) && classof(cast<Instruction>(V));
    }
};


class JumpInst: public Instruction {
protected:
    JumpInst(BasicBlock *Dest, Instruction *InsertBefore);
    JumpInst(BasicBlock *Dest, BasicBlock *InsertAtEnd);
private:
    BasicBlock *Dest;
public:
    static JumpInst *Create(BasicBlock *Dest, Instruction *InsertBefore = nullptr);
    static JumpInst *Create(BasicBlock *Dest, BasicBlock *InsertAtEnd);
    /// Return the destination basic block of the jump instruction.
    BasicBlock *getDestBasicBlock() const { return Dest; }

    static bool classof(const Instruction *I) {
        return I->getOpcode() == Instruction::Jump;
    }

    static bool classof(const Value *V) {
        return isa<Instruction>(V) && classof(cast<Instruction>(V));
    }
};


class BranchInst: public Instruction {
protected:
    BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond, Instruction *InsertBefore);
    BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond, BasicBlock *InsertAtEnd);
private:
    BasicBlock *IfTrue; 
    BasicBlock *IfFalse;

    void AssertOK() const;
public:
    static BranchInst *Create(BasicBlock *IfTrue, BasicBlock *IfFalse, 
                              Value *Cond, 
                              Instruction *InsertBefore = nullptr);
    static BranchInst *Create(BasicBlock *IfTrue, BasicBlock *IfFalse, 
                              Value *Cond, 
                              BasicBlock *InsertAtEnd);
    /// Return the condition value of the branch instruction.
    Value *getCondition() const { return getOperand(0); }
    /// Return the true branch basic block of the branch instruction.
    BasicBlock *getTrueBB() const { return IfTrue; }
    /// Return the false branch basic block of the branch instruction.
    BasicBlock *getFalseBB() const { return IfFalse; }

    static bool classof(const Instruction *I) {
        return I->getOpcode() == Instruction::Br;
    }

    static bool classof(const Value *V) {
        return isa<Instruction>(V) && classof(cast<Instruction>(V));
    }
};



/// Panic is a temporarily a placeholder terminator.
/// When a program encounters a 'PanicInst', it crashes.
/// You are NOT required to handle this instruction.
class PanicInst: public Instruction {
protected:
    explicit PanicInst(Instruction *InsertBefore);
    explicit PanicInst(BasicBlock *InsertAtEnd);
public:
    static PanicInst *Create(Instruction *InsertBefore = nullptr);
    static PanicInst *Create(BasicBlock *InsertAtEnd);

    static bool classof(const Instruction *I) {
        return I->getOpcode() == Instruction::Panic;
    }

    static bool classof(const Value *V) {
        return isa<Instruction>(V) && classof(cast<Instruction>(V));
    }
};


class BasicBlock final: public ListNode<BasicBlock> {
public:
    BasicBlock(const BasicBlock &) = delete;
    BasicBlock &operator=(const BasicBlock &) = delete;
public:
    using InstListType = List<Instruction>;
    using iterator = InstListType::iterator;
    using const_iterator = InstListType::const_iterator;
    using reverse_iterator = InstListType::reverse_iterator;
    using const_reverse_iterator = InstListType::const_reverse_iterator;

    friend class Instruction;

private:
    InstListType InstList;
    Function *Parent;
    std::string Name;

    InstListType &getInstList() { return InstList; }
    const InstListType &getInstList() const { return InstList; }

    BasicBlock(Function *Parent, BasicBlock *InsertBefore);
public:
    static BasicBlock *Create(Function *Parent = nullptr, BasicBlock *InsertBefore = nullptr);
    /// Insert an unlinked basic block into a function immediately before the specified basic block.
    void insertInto(Function *Parent, BasicBlock *InsertBefore = nullptr);

    void setParent(Function *F);
    Function *getParent() const { return Parent; }
    bool hasName() const;
    void setName(std::string_view Name);
    std::string_view getName() const { return Name; }
    
    /// Returns the terminator instruction if the block is well formed or null
    /// if the block is not well formed.
    const Instruction *getTerminator() const {
        if (InstList.empty() || !InstList.back().isTerminator())
            return nullptr;
        return &InstList.back();
    }
    Instruction *getTerminator() {
        return const_cast<Instruction *>(
            static_cast<const BasicBlock *>(this)->getTerminator()
        );
    }
    // container standard interface
    iterator begin() { return InstList.begin(); }
    const_iterator begin() const { return InstList.cbegin(); }
    iterator end() { return InstList.end(); }
    const_iterator end() const { return InstList.cend(); }
    reverse_iterator rbegin() { return InstList.rbegin(); }
    const_reverse_iterator rbegin() const { return InstList.crbegin(); }
    reverse_iterator rend() { return InstList.rend(); }
    const_reverse_iterator rend() const { return InstList.crend(); }
    [[nodiscard]] std::size_t size() const { return InstList.size(); }
    [[nodiscard]] bool empty() const { return InstList.empty(); }
    [[nodiscard]] const Instruction &front() const { return InstList.front(); }
    [[nodiscard]] Instruction &front() { return InstList.front(); }
    [[nodiscard]] const Instruction &back() const { return InstList.back(); }
    [[nodiscard]] Instruction &back() { return InstList.back(); }
};


class Argument: public Value,
                public ListNode<Argument> {
protected:
    // You should not create an Argument value direcly.
    // The Function class will initialize its Argument values.
    explicit Argument(Type *Ty, Function *Parent = nullptr, unsigned ArgNo = 0);
private:
    Function *Parent;
    unsigned ArgNo;

    friend class Function;
public:
    const Function *getParent() const { return Parent; }
    Function *getParent() { return Parent; }
    /// Return the argument index in its parent function.
    unsigned getArgNo() const { return ArgNo; }

    static bool classof(const Value *V) {
        return V->getValueID() == Value::ArgumentVal;
    }
};


class Function final: public ListNode<Function> {
private:
    Function(FunctionType *FTy, bool ExternalLinkage, 
             std::string_view Name, Module *M);
public:
    Function(const Function&) = delete;
    void operator=(const Function&) = delete;
    ~Function() final;
    /// Basic blocks iteration.
    using BasicBlockListType = List<BasicBlock>;
    using iterator = BasicBlockListType::iterator;
    using const_iterator = BasicBlockListType::const_iterator;
    using reverse_iterator = BasicBlockListType::reverse_iterator;
    using const_reverse_iterator = BasicBlockListType::const_reverse_iterator;

    // Arguments iteration
    using arg_iterator = Argument *;
    using const_arg_iterator = const Argument *;
private:
    FunctionType *FTy;
    unsigned NumArgs = 0;
    Argument *Arguments = nullptr;
    bool ExternalLinkage = false;
    std::string Name;
    Module *Parent;
    BasicBlockListType BasicBlockList;

    friend class BasicBlock;

    // Private accessors to basic blocks and function arguments.
    BasicBlockListType &getBasicBlockList() { return BasicBlockList; }
    const BasicBlockListType &getBasicBlockList() const { return BasicBlockList; }
    
public:
    static Function *Create(FunctionType *FTy, bool ExternalLinkage = false,
                            std::string_view Name = "", Module *M = nullptr);
    /// Insert BB in the basic block list at Position.
    Function::iterator insert(Function::iterator Position, BasicBlock *BB);
    
    std::string_view getName() const { return Name; }
    bool hasName() const;
    Module *getParent() const { return Parent; }
    /// Returns the FunctionType.
    FunctionType *getFunctionType() const {
        return cast<FunctionType>(FTy);
    }
    /// Returns the type of the ret val.
    Type *getReturnType() const { return FTy->getReturnType(); }
    std::size_t getNumParams() const { return NumArgs; }
    /// Return the function is defined in the current module or has external linkage.
    bool hasExternalLinkage() const { return ExternalLinkage; }

    /// Get the first basic block of the function.
    const BasicBlock &getEntryBlock() const { return BasicBlockList.front(); }
    BasicBlock &getEntryBlock() { return BasicBlockList.front(); }

    // Basic block iteration.
    iterator begin() { return BasicBlockList.begin(); }
    const_iterator begin() const { return BasicBlockList.cbegin(); }
    iterator end() { return BasicBlockList.end(); }
    const_iterator end() const { return BasicBlockList.cend(); }
    reverse_iterator rbegin() { return BasicBlockList.rbegin(); }
    const_reverse_iterator rbegin() const { return BasicBlockList.crbegin(); }
    reverse_iterator rend() { return BasicBlockList.rend(); }
    const_reverse_iterator rend() const { return BasicBlockList.crend(); }
    // Basic blocks container method.
    [[nodiscard]] std::size_t size() const { return BasicBlockList.size(); }
    [[nodiscard]] bool empty() const { return BasicBlockList.empty(); }
    [[nodiscard]] const BasicBlock &front() const { return BasicBlockList.front(); }
    [[nodiscard]] BasicBlock &front() { return BasicBlockList.front(); }
    [[nodiscard]] const BasicBlock &back() const { return BasicBlockList.back(); }
    [[nodiscard]] BasicBlock &back() { return BasicBlockList.back(); }

    // Argument iteration.
    arg_iterator arg_begin() { return Arguments; }
    const_arg_iterator arg_begin() const { return Arguments; }
    arg_iterator arg_end() { return Arguments + NumArgs; }
    const_arg_iterator arg_end() const { return Arguments + NumArgs; }
    Argument *getArg(unsigned index) const {
        assert (index < NumArgs && "getArg() out of range!");
        return Arguments + index;
    }
    // Arguments container method.
    [[nodiscard]] std::size_t arg_size() const { return NumArgs; }
    [[nodiscard]] bool arg_empty() const { return arg_size() == 0; }
};


class GlobalVariable: public Value,
                      public ListNode<GlobalVariable> {
protected:
    GlobalVariable(Type *EleTy, std::size_t NumElements, bool ExternalLinkage,
                   std::string_view Name, Module *M);
private:
    Type *EleTy;
    std::size_t NumElements;
    bool ExternalLinkage;
    std::string Name;
    Module *Parent;
public:
    static GlobalVariable *Create(Type *EleTy, std::size_t NumElements = 1, bool ExternalLinkage = false,
                                  std::string_view Name = "", Module *M = nullptr);
    /// Return the element type of the global variable.
    /// e.g. for '@a : region i32, 2', element type of '@a' is i32 while type of '@a' is i32*.
    Type *getElementType() const { return EleTy; }
    /// Return the region size of the global variable.
    std::size_t getNumElements() const { return NumElements; }
    /// Return the function is defined in the current module or has external linkage.
    bool hasExternalLinkage() const { return ExternalLinkage; }
    Module *getParent() const { return Parent; }
    std::string_view getName() const { return Name; }
    bool hasName() const;


    static bool classof(const Value *V) {
        return V->getValueID() == Value::GlobalVariableVal;
    }
};

class Module {
public:
    // Function iteration.
    using FunctionListType = List<Function>;
    using iterator = FunctionListType::iterator;
    using const_iterator = FunctionListType::const_iterator;
    using reverse_iterator = FunctionListType::reverse_iterator;
    using const_reverse_iterator = FunctionListType::const_reverse_iterator;

    // Global variable iteration.
    using GlobalListType = List<GlobalVariable>;
    using global_iterator = GlobalListType::iterator;
    using const_global_iterator = GlobalListType::const_iterator;

private:
    std::unordered_map<std::string_view, Function *> SymbolFunctionMap;
    std::unordered_map<std::string_view, GlobalVariable *> SymbolGlobalMap;
    FunctionListType FunctionList;
    GlobalListType GlobalVariableList;

    friend class Function;
    friend class GlobalVariable;
public:
    /// Implementation of IR dump to a output stream.
    /// if isForDebug is set, the IR writer will print verbose type annotations.
    void print(std::ostream &OS, bool isForDebug) const;
    /// Function accessor.
    /// Look up the specified function in the module symbol table.
    Function *getFunction(std::string_view Name) const;
    // Function iteration.
    iterator begin() { return FunctionList.begin(); }
    const_iterator begin() const { return FunctionList.cbegin(); }
    iterator end() { return FunctionList.end(); }
    const_iterator end() const { return FunctionList.cend(); }
    reverse_iterator rbegin() { return FunctionList.rbegin(); }
    const_reverse_iterator rbegin() const { return FunctionList.crbegin(); }
    reverse_iterator rend() { return FunctionList.rend(); }
    const_reverse_iterator rend() const { return FunctionList.crend(); }
    // Private functions and global variables accessors.
    FunctionListType &getFunctionList() { return FunctionList; }
    const FunctionListType &getFunctionList() const { return FunctionList; }

    [[nodiscard]] std::size_t size() const { return FunctionList.size(); }
    [[nodiscard]] bool empty() const { return FunctionList.empty(); }

    /// Global variable accessor.
    /// Look up the specified global variable in the module symbol table.
    GlobalVariable *getGlobalVariable(std::string_view Name) const;
    // Global iteration.
    global_iterator global_begin() { return GlobalVariableList.begin(); }
    const_global_iterator global_begin() const { return GlobalVariableList.cbegin(); }
    global_iterator global_end() { return GlobalVariableList.end(); }
    const_global_iterator global_end() const { return GlobalVariableList.cend(); }
    GlobalListType &getGlobalList() { return GlobalVariableList; }
    const GlobalListType &getGlobalList() const { return GlobalVariableList; }

    [[nodiscard]] std::size_t global_size() const { return GlobalVariableList.size(); }
    [[nodiscard]] bool global_empty() const { return GlobalVariableList.empty(); }
};