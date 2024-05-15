#include "ir/type.h"
#include "ir/ir.h"
#include "utils/casting.h"

#include <stdexcept>
#include <ostream>


enum PrefixType {
    GlobalPrefix,
    ArgPrefix,
    LocalPrefix,
    NoPrefix
};

/// Turn the specified name into an 'Accipit name', which is either prefixed with %
static void PrintAccipitName(std::ostream &OS, std::string_view Name, PrefixType Prefix) {
    switch (Prefix) {
    case NoPrefix:
        break;
    case GlobalPrefix:
        OS << '@';
        break;
    case ArgPrefix:
        OS << "#";
        break;
    case LocalPrefix:
        OS << '%';
        break;
    }
    assert(!Name.empty() && "Cannot get an empty identifier!");
    OS << Name;
}

/// Turn the specified name into an 'Accipit name', which is either prefixed with %
static void PrintAccipitName(std::ostream &OS, const Value *V) {
    PrintAccipitName(OS, V->getName(),
                isa<GlobalVariable>(V) ? GlobalPrefix : LocalPrefix);
}


class ListSeparator {
    std::string_view Seperator;
    bool FirstItem = true;
public:
    ListSeparator(std::string_view Separator = ", ") : Seperator(Separator) {}
    ~ListSeparator() = default;
    operator std::string_view()  {
        if (FirstItem) {
            FirstItem = false;
            return {};
        } else {
            return Seperator;
        }
    }
};


namespace {

class TypePrinter {
public:
    void print(Type *Ty, std::ostream &os);
};

} // end of annoymous namespace


void TypePrinter::print(Type *Ty, std::ostream &OS) {
    switch (Ty->getTypeID()) {
    case Type::IntegerTyID:
        OS << "i32";
        return;
    case Type::UnitTyID:
        OS << "()";
        return;
    case Type::PointerTyID: {
        print(cast<PointerType>(Ty)->getElementType(), OS);
        OS << "*";
        return;
    }
    case Type::FunctionTyID: {
        OS << "fn(";
        ListSeparator LS;
        for (Type *ParamTy : cast<FunctionType>(Ty)->params()) {
            OS << (std::string_view)LS;
            print(ParamTy, OS);
        }
        OS << " ) -> ";
        print(cast<FunctionType>(Ty)->getReturnType(), OS);
        return;
    }
    default:
        throw std::runtime_error("Invalid Type ID");
    }
}

/// Assign the slot number to annoymous values
class SlotTracker {
    const Module *M;

    // Global scope slots.
    std::unordered_map<Function *, unsigned> FunctionSlots;
    std::unordered_map<GlobalVariable *, unsigned> GlobalSlots;
    unsigned gNext = 0;
    // Local scope slots, binding to a specific function.
    Function *F;
    std::unordered_map<BasicBlock *, unsigned> BasicBlockSlots;
    std::unordered_map<Value *, unsigned> LocalSlots;
    unsigned lNext = 0;

public:

    explicit SlotTracker(const Module *M);

    void incorporateFunction(const Function *F);

    std::optional<unsigned> getGlobalSlot(const GlobalVariable *GV);
    std::optional<unsigned> getFunctionSlot(const Function *F);

    std::optional<unsigned> getBasicBlockSlot(const BasicBlock *BB);
    std::optional<unsigned> getLocalSlot(const Value *V);
};

SlotTracker::SlotTracker(const Module *M) : M(M) {
    for (auto GI = M->global_begin(), GE = M->global_end(); GI != GE; ++GI) {
        if (!GI->hasName())
            GlobalSlots[&*GI] = gNext++;
    }

    for (auto & FI : *M) {
        if (!FI.hasName())
            FunctionSlots[&FI] = gNext++;
    }
}


void SlotTracker::incorporateFunction(const Function *F) {
    // clear
    this->F = const_cast<Function *>(F);
    BasicBlockSlots.clear();
    LocalSlots.clear();
    lNext = 0;
    // assign slot number.
    for (auto Arg = F->arg_begin(), ArgE = F->arg_end(); Arg != ArgE; ++Arg) {
        if (!Arg->hasName()) {
            LocalSlots[const_cast<Argument *>(Arg)] = lNext++;
        }
    }
    for (auto & BI : *F) {
        if (!BI.hasName())
            BasicBlockSlots[&BI] = lNext++;
        for (auto & I : BI) {
            if (!I.hasName())
                LocalSlots[&I] = lNext++;
        }
    }
}

std::optional<unsigned> SlotTracker::getGlobalSlot(const GlobalVariable *GV) {
    auto It = GlobalSlots.find(const_cast<GlobalVariable *>(GV));
    if (It == GlobalSlots.end())
        return std::nullopt;
    return It->second;
}

std::optional<unsigned> SlotTracker::getFunctionSlot(const Function *F) {
    auto It = FunctionSlots.find(const_cast<Function *>(F));
    if (It == FunctionSlots.end())
        return std::nullopt;
    return It->second;
}

std::optional<unsigned> SlotTracker::getBasicBlockSlot(const BasicBlock *BB) {
    auto It = BasicBlockSlots.find(const_cast<BasicBlock *>(BB));
    if (It == BasicBlockSlots.end())
        return std::nullopt;
    return It->second;
}

std::optional<unsigned> SlotTracker::getLocalSlot(const Value *V) {
    auto It = LocalSlots.find(const_cast<Value *>(V));
    if (It == LocalSlots.end())
        return std::nullopt;
    return It->second;
}



static void WriteConstantInternal(std::ostream &Out, const Constant *C,
                                  SlotTracker &Tracker, bool isForDebug) {
    if (const auto *CI = dyn_cast<ConstantInt>(C)) {
        Out << CI->getValue();
        return;
    }

    if (const auto *CU = dyn_cast<ConstantUnit>(C)) {
        Out << "()";
        return;
    }
    Out << "<placeholder or erroneous Constant>";
}

// Full implementation of printing a Value as an operand with support for
// TypePrinting, etc.
static void WriteAsOperandInternal(std::ostream &Out, const Value *V,
                                   SlotTracker &Tracker, bool isForDebug) {
    if (V->hasName()) {
        PrintAccipitName(Out, V);
        return;
    }

    if (auto *C = dyn_cast<Constant>(V)) {
        WriteConstantInternal(Out, C, Tracker, isForDebug);
        return;
    }

    char Prefix = '%';
    std::optional<unsigned> Slot;

    if (auto *GV = dyn_cast<GlobalVariable>(V)) {
        Slot = Tracker.getGlobalSlot(GV);
        Prefix = '@';
    } else if (auto *Arg = dyn_cast<Argument>(V)) {
        Slot = Tracker.getLocalSlot(Arg);
        Prefix = '#';
    } else {
        Slot = Tracker.getLocalSlot(V);
        Prefix = '%';
    }

    if (Slot.has_value()) {
        Out << Prefix << Slot.value();
        if (isForDebug) {
            Out << ": ";
            TypePrinter TP;
            TP.print(V->getType(), Out);
        }
    } else {
        Out << "<badref>";
    }
}

static void WriteFunctionOperandInternal(std::ostream &Out, const Function *F,
                                        SlotTracker &Tracker, bool isForDebug) {
    if (F->hasName()) {
        PrintAccipitName(Out, F->getName(), GlobalPrefix);
    } else {
        auto Slot = Tracker.getFunctionSlot(F);
        if (Slot.has_value()) {
            Out << '@' << Slot.value();
        } else {
            Out << "<badref>";
        }
    }
}

static void WriteBasicBlockOperandInternal(std::ostream &Out, const BasicBlock *BB,
                                          SlotTracker &Tracker, bool isForDebug) {
    if (BB->hasName()) {
        PrintAccipitName(Out, BB->getName(), LocalPrefix);
    } else {
        auto Slot = Tracker.getBasicBlockSlot(BB);
        if (Slot.has_value()) {
            Out << '%' << Slot.value();
        } else {
            Out << "<badref>";
        }
    }
}

/// Print the Accipit IR to text form.
class AccipitWriter {
    std::ostream &Out;
    SlotTracker &Tracker;
    bool isForDebug;
public:
    AccipitWriter(std::ostream &OS, SlotTracker &Tracker, bool isForDebug)
        : Out(OS), Tracker(Tracker), isForDebug(isForDebug) { }

    void printInstruction(const Instruction *I);
    void printBasicBlock(const BasicBlock *BB);
    void printArgument(const Argument *Arg);
    void printFunction(const Function *F);
    void printGlobalVariable(const GlobalVariable *GV);
    void printModule(const Module *M);
};


void AccipitWriter::printInstruction(const Instruction *I) {
    if (I->isTerminator()) {
        Out << "  ";
    } else {
        Out << "  let ";
        WriteAsOperandInternal(Out, I, Tracker, isForDebug);
        Out << " = ";
    }

    if (auto *Binary = dyn_cast<BinaryInst>(I)) {
        switch (I->getOpcode()) {
            case Instruction::Add:
                Out << "add ";
                break;
            case Instruction::Sub:
                Out << "sub ";
                break;
            case Instruction::Mul:
                Out << "mul ";
                break;
            case Instruction::Div:
                Out << "div ";
                break;
            case Instruction::Mod:
                Out << "rem ";
                break;
            case Instruction::And:
                Out << "and ";
                break;
            case Instruction::Or:
                Out << "or ";
                break;
            case Instruction::Xor:
                Out << "xor ";
                break;
            case Instruction::Lt:
                Out << "lt ";
                break;
            case Instruction::Le:
                Out << "le ";
                break;
            case Instruction::Ge:
                Out << "ge ";
                break;
            case Instruction::Gt:
                Out << "gt ";
                break;
            case Instruction::Eq:
                Out << "eq ";
                break;
            case Instruction::Ne:
                Out << "ne ";
                break;
            default:
                Out << "<unknown binary opcode> ";
            }

        WriteAsOperandInternal(Out, Binary->getOperand(0), Tracker, isForDebug);
        Out << ", ";
        WriteAsOperandInternal(Out, Binary->getOperand(1), Tracker, isForDebug);
    } else {
        switch (I->getOpcode()) {
        case Instruction::Alloca: {
            Out << "alloca ";
            auto *Alloca = cast<AllocaInst>(I);
            TypePrinter TP;
            TP.print(Alloca->getAllocatedType(), Out);
            Out << ", ";
            Out << Alloca->getNumElements();
            break;
        }
        case Instruction::Load: {
            Out << "load ";
            auto *Load = cast<LoadInst>(I);
            WriteAsOperandInternal(Out, Load->getPointerOperand(), Tracker, isForDebug);
            break;
        }
        case Instruction::Store: {
            Out << "store ";
            auto *Store = cast<StoreInst>(I);
            WriteAsOperandInternal(Out, Store->getValueOperand(), Tracker, isForDebug);
            Out << ", ";
            WriteAsOperandInternal(Out, Store->getPointerOperand(), Tracker, isForDebug);
            break;
        }
        case Instruction::Offset: {
            Out << "offset ";
            auto *Offset = cast<OffsetInst>(I);
            TypePrinter TP;
            TP.print(Offset->getElementType(), Out);
            Out << ", ";
            WriteAsOperandInternal(Out, Offset->getPointerOperand(), Tracker, isForDebug);
            
            unsigned Dims = I->getNumOperands() - 1;
            for (unsigned i = 0; i < Dims; ++i) {
                Out << ", [";
                WriteAsOperandInternal(Out, Offset->getOperand(i + 1), Tracker, isForDebug);
                Out << ", ";
                auto Bound = Offset->getBound(i);
                if (Bound.has_value()) {
                    Out << Bound.value();
                } else {
                    Out << "none";
                }
                Out << "]";
            }
            break;
        }
        case Instruction::Call: {
            Out << "call ";
            auto *Call = cast<CallInst>(I);
            WriteFunctionOperandInternal(Out, Call->getCallee(), Tracker, isForDebug);
            for (unsigned i = 0; i < Call->getNumOperands(); ++i) {
                Out << ", ";
                WriteAsOperandInternal(Out, Call->getArgOperand(i), Tracker, isForDebug);
            }
            break;
        }
        case Instruction::Ret: {
            Out << "ret ";
            auto *Ret = cast<RetInst>(I);
            WriteAsOperandInternal(Out, Ret->getReturnValue(), Tracker, isForDebug);
            break;
        }
        // Termintors
        case Instruction::Br: {
            Out << "br ";
            auto *Br = cast<BranchInst>(I);
            WriteAsOperandInternal(Out, Br->getCondition(), Tracker, isForDebug);
            Out << ", label ";
            WriteBasicBlockOperandInternal(Out, Br->getTrueBB(), Tracker, isForDebug);
            Out << ", label ";
            WriteBasicBlockOperandInternal(Out, Br->getFalseBB(), Tracker, isForDebug);
            break;
        }
        case Instruction::Jump: {
            Out << "jmp label ";
            auto *Jump = cast<JumpInst>(I);
            WriteBasicBlockOperandInternal(Out, Jump->getDestBasicBlock(), Tracker, isForDebug);
            break;
        }
        case Instruction::Panic:
            Out << "panic ";
            break;
        default:
            Out << "<unknown opcode>";
            break;
        }
    }
    // dump user information in debug mode
    if (isForDebug) {
        Out << " // Users: ";
        for (auto &U : I->getUserView()) {
            WriteAsOperandInternal(Out, U.getUser(), Tracker, isForDebug);
            Out << ", ";
        }
    }
    Out << "\n";
}

void AccipitWriter::printBasicBlock(const BasicBlock *BB) {
    WriteBasicBlockOperandInternal(Out, BB, Tracker, isForDebug);
    Out << ":\n";

    for (auto & II : *const_cast<BasicBlock *>(BB)) {
        printInstruction(&II);
    }
}


void AccipitWriter::printArgument(const Argument *Arg) {
    // Do not write type annotation for argument.
    WriteAsOperandInternal(Out, Arg, Tracker, false);
    Out << ": ";
    TypePrinter TP;
    TP.print(Arg->getType(), Out);
}

void AccipitWriter::printFunction(const Function *F) {
    Tracker.incorporateFunction(F);
    Out << "fn ";
    WriteFunctionOperandInternal(Out, F, Tracker, isForDebug);
    Out << "(";
    ListSeparator LS;
    for (auto AI = F->arg_begin(), AE = F->arg_end(); AI != AE; ++AI) {
        Out << (std::string_view)LS;
        printArgument(&*AI);
    }
    Out << " ) -> ";
    TypePrinter TP;
    TP.print(F->getReturnType(), Out);

    if (F->hasExternalLinkage()) {
        Out << ";\n";
        return; 
    }

    Out << " {\n";

    Tracker.incorporateFunction(F);

    for (auto & BI : *F) {
        printBasicBlock(&BI);
    }

    Out << "}\n";
}

void AccipitWriter::printGlobalVariable(const GlobalVariable *GV) {
    TypePrinter TP;
    WriteAsOperandInternal(Out, GV, Tracker, isForDebug);
    Out << " : region ";
    TP.print(GV->getElementType(), Out);
    Out << ", ";
    Out << GV->getNumElements();
    Out << "\n";
}


void AccipitWriter::printModule(const Module *M) {
    for (auto GI = M->global_begin(), GE = M->global_end(); GI != GE; ++GI) {
        printGlobalVariable(&*GI);
    }

    for (auto & FI : *(M)) {
        printFunction(&FI);
    }
}

void Module::print(std::ostream &OS, bool isForDebug) const {
    SlotTracker Tracker(this);
    AccipitWriter Writer(OS, Tracker, isForDebug);
    Writer.printModule(this);
}
