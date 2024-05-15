#pragma once

#include <type_traits>
#include <iterator>
#include <cstddef>
#include <cassert>

template <typename T> class ListNode;


template <typename T, bool IsReverse, bool IsConst>
class ListIterator {
private:
    ListNode<T> *NodePtr;

public:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;

    ListIterator() = default;
    ListIterator(ListNode<T> *Node): NodePtr(Node) {}

    /// Get the underlying Node
    ListNode<T> *getNodePtr() { return NodePtr; } 

    /// Allowing construct a const iterator from a nonconst iterator.
    template <bool RHSIsConst>
    ListIterator(const ListIterator<T, IsReverse, RHSIsConst> &RHS,
                 std::enable_if_t<IsConst || !RHSIsConst, void *> = nullptr)
        : NodePtr(RHS.Node) {}

    /// Allowing assign a nonconst iterator to a const iterator.
    template <bool RHSIsConst>
    std::enable_if_t<IsConst || !RHSIsConst, ListIterator &>
    operator=(const ListIterator<T, IsReverse, RHSIsConst> &RHS) {
        NodePtr = RHS.NodePtr;
        return *this;
    }

    ListIterator &operator++() {
        NodePtr = IsReverse ? NodePtr->getPrevNode() : NodePtr->getNextNode();
        return *this;
    }

    ListIterator &operator--() {
        NodePtr = IsReverse ? NodePtr->getNextNode() : NodePtr->getPrevNode();
        return *this;
    }
    ListIterator operator++(int) {
        ListIterator tmp = *this;
        ++*this;
        return tmp; 
    }

    ListIterator &operator--(int) {
        ListIterator tmp = *this;
        --*this;
        return *this;
    }

    // Accessors.
    reference operator*() const { return *getNodePtr()->getValuePtr(); }
    pointer operator->() const { return &operator*(); }

    friend bool operator==(const ListIterator &LHS, const ListIterator &RHS) {
        return LHS.NodePtr == RHS.NodePtr;
    }

    friend bool operator!=(const ListIterator &LHS, const ListIterator &RHS) {
        return LHS.NodePtr != RHS.NodePtr;
    }

};


template <typename T> class ListNode {
public:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;

protected:
    ListNode *Prev = nullptr;
    ListNode *Next = nullptr;
    virtual ~ListNode() = default;

public:
    pointer getValuePtr() { return static_cast<pointer>(this); }
    const_pointer getValuePtr() const {
        return static_cast<const_pointer>(this);
    }

    pointer getNextNode() { return Next; }
    const_pointer getNextNode() const { return Next; }

    pointer getPrevNode() { return Prev; }
    const_pointer getPrevNode() const { return Prev; }

    void insertBefore(pointer Node) {
        if (Node) {
            if (Prev) {
                Prev->Next = Node;
            }
            Node->Next = this;
            Node->Prev = Prev;
            Prev = Node;
        }
    }

    void insertAfter(pointer Node) {
        if (Node) {
            if (Next) {
                Next->Prev = Node;
            }
            Node->Next = Next;
            Node->Prev = this;
            Next = Node;
        }
    }

    void removeFromList() {
        if (Prev) {
            Prev->Next = Next;
        }
        if (Next) {
            Next->Prev = Prev;
        }
        Prev = Next = nullptr;
    }

    void removeAndDispose() {
        removeFromList();
        delete this;
    }
};


template <typename T>
class List {
public:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;

    using iterator = ListIterator<T, false, false>;
    using const_iterator = ListIterator<T, false, true>;
    using reverse_iterator = ListIterator<T, true, false>;
    using const_reverse_iterator = ListIterator<T, true, true>;

    using size_type = size_t;
    using difference_type = ptrdiff_t;

protected:
    ListNode<T> Sentinel;

public:
    List() = default;
    ~List() = default;

    List(const List &) = delete;
    List &operator=(const List &) = delete;

    iterator begin() { return ++iterator(Sentinel); }
    const_iterator begin() const { return ++const_iterator(Sentinel); }
    iterator end() { return iterator(Sentinel); }
    const_iterator end() const { return const_iterator(Sentinel); }
    reverse_iterator rbegin() { return ++reverse_iterator(Sentinel); }
    const_reverse_iterator rbegin() const {
        return ++const_reverse_iterator(Sentinel);
    }
    reverse_iterator rend() { return reverse_iterator(Sentinel); }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(Sentinel);
    }

    /// Calculate the size in linear time.
    [[nodiscard]] size_type size() const {
        return std::distance(begin(), end());
    }

    [[nodiscard]] bool empty() const {
        return Sentinel.getNextNode() == nullptr &&
               Sentinel.getPrevNode() == nullptr;
    }


    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *rbegin(); }
    const_reference back() const { return *rbegin(); }

    /// Insert a node by reference.
    iterator insert(iterator I, reference Node) {
        I->insertBefore(&Node);
        return iterator(&Node);
    }


    /// Insert a node at front.
    void push_front(reference Node) { insert(begin(), Node); }
    /// Insert a node at back.
    void push_back(reference Node) { insert(end(), Node); }
    /// Remove the node at front.
    void pop_front() { front().removeAndDispose(); }
    /// Remove the node at back.
    void pop_back() { front().removeAndDispose(); }

    /// Remove a node by reference.
    void remove(reference Node) { Node.removeFromList(); } 
    /// Remove a node by iterator.
    iterator erase(iterator I) {
        assert(I != end() && "Cannot remove end of list");
        remove(*I++);
        return I;
    }
}; 