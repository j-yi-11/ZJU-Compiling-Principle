#pragma once

#include <type_traits>
#include <iterator>
#include <cstddef>
#include <cassert>

template <typename T> class ListNode;
template <typename T> class List;

template <typename T, bool IsReverse, bool IsConst>
class ListIterator {
private:
    ListNode<T> *NodePtr = nullptr;

public:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using difference_type = ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

    ListIterator() = default;
    explicit ListIterator(ListNode<T> *Node): NodePtr(Node) {}
    explicit ListIterator(const ListNode<T> *Node)
        : NodePtr(const_cast<ListNode<T> *>(Node)) { }

    /// Get the underlying Node
    ListNode<T> *getNodePtr() { return NodePtr; } 

    /// Allowing construct a const iterator from a nonconst iterator.
    template <bool RHSIsConst>
    explicit ListIterator(const ListIterator<T, IsReverse, RHSIsConst> &RHS,
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

    ListIterator operator--(int) {
        ListIterator tmp = *this;
        --*this;
        return tmp;
    }

    // Accessors.
    reference operator*() { return *NodePtr->getValuePtr(); }
    const_reference operator*() const { return operator*(); }
    pointer operator->() { return &operator*(); }

    friend bool operator==(const ListIterator &LHS, const ListIterator &RHS) {
        return LHS.NodePtr == RHS.NodePtr;
    }

    friend bool operator!=(const ListIterator &LHS, const ListIterator &RHS) {
        return LHS.NodePtr != RHS.NodePtr;
    }

    bool isNull() const { return *this == nullptr; }
};


template <typename T> class ListNode {
public:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;

    // Sential list node should have public destructor.
    // workaround for no ListNodeImpl interface.
    friend class List<T>;

    using iterator = ListIterator<T, false, false>;

protected:
    ListNode *Prev = nullptr;
    ListNode *Next = nullptr;
    // Other classes will be derived from intrusive list node by CRTP style.
    virtual ~ListNode() = default;

public:
    pointer getValuePtr() { return static_cast<pointer>(this); }
    const_pointer getValuePtr() const {
        return static_cast<const_pointer>(this);
    }

    pointer getNextNode() { return Next->getValuePtr(); }
    const_pointer getNextNode() const { return Next->getValuePtr(); }

    pointer getPrevNode() { return Prev->getValuePtr(); }
    const_pointer getPrevNode() const { return Prev->getValuePtr(); }

protected:
    /// Private insertion & removal interface.
    static iterator insert(iterator where, pointer Node) {
        if (Node) {
            if (where->Prev) {
                where->Prev->Next = Node;
            }
            Node->Next = where.getNodePtr();
            Node->Prev = where->Prev;
            where->Prev = Node;
        }
        return iterator(Node);
    }

    static void remove(reference Node) {
        if (Node.Prev) {
            Node.Prev->Next = Node.Next;
        }
        if (Node.Next) {
            Node.Next->Prev = Node.Prev;
        }
        Node.Prev = Node.Next = nullptr;
    }

};

/// An intrusive double linked list with ownership.
/// The list node class T should be derived from ListNode<T>.
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

    void resetSentinel() {
        Sentinel.Next = Sentinel.Prev = &Sentinel;
    }
public:
    List() { resetSentinel();}
    // Destruct all nodes in the list.
    ~List() { clear(); }
    // Intrusive list has the ownership of data, so we do not allow copy here.
    List(const List &) = delete;
    List &operator=(const List &) = delete;

    iterator begin() { return ++iterator(&Sentinel); }
    const_iterator cbegin() const { return ++const_iterator(&Sentinel); }
    iterator end() { return iterator(&Sentinel); }
    const_iterator cend() const { return const_iterator(&Sentinel); }
    reverse_iterator rbegin() { return ++reverse_iterator(&Sentinel); }
    const_reverse_iterator crbegin() const {
        return ++const_reverse_iterator(&Sentinel);
    }
    reverse_iterator rend() { return reverse_iterator(&Sentinel); }
    const_reverse_iterator crend() const {
        return const_reverse_iterator(&Sentinel);
    }

    /// Calculate the size in linear time.
    [[nodiscard]] size_type size() const {
        return std::distance(cbegin(), cend());
    }

    [[nodiscard]] bool empty() const {
        return Sentinel.getNextNode() == nullptr &&
               Sentinel.getPrevNode() == nullptr;
    }


    reference front() { return *begin(); }
    const_reference front() const { return *cbegin(); }
    reference back() { return *rbegin(); }
    const_reference back() const { return *crbegin(); }

    iterator insert(iterator pos, pointer New) {
        return ListNode<T>::insert(pos, New);
    }

    iterator insertAfter(iterator pos, pointer New) {
        if (empty()) {
            return insert(begin(), New);
        } else {
            return insert(++pos, New);
        }
    }
    /// Remove a node from list.
    pointer remove(iterator &IT) {
        pointer Node = &*IT++;
        ListNode<T>::remove(*Node);
        return Node;
    }

    pointer remove(const iterator &IT) {
        iterator MutIT = IT;
        return remove(MutIT);
    }

    pointer remove(pointer IT) { return remove(iterator(IT)); }
    pointer remove(reference IT) { return remove(iterator(IT)); }

    /// Remove a node from list and delete it, return the iterator forwarded.
    iterator erase(iterator IT) {
        iterator Node = IT++;
        delete (remove(Node));
        return IT;
    }

    iterator erase(pointer IT) { return erase(iterator(IT)); }
    iterator erase(reference IT) { return erase(iterator(IT)); }

    //===----------------------------------------------------------------------===
    // Functionality derived from other functions defined above...
    //

    /// Insert a node at front.
    void push_front(pointer Node) { insert(begin(), Node); }
    /// Insert a node at back.
    void push_back(pointer Node) { insert(end(), Node); }
    /// Remove the node at front and delete it.
    void pop_front() {
        assert(!empty() && "pop_front() on a empty list!");
        erase(begin());
    }
    /// Remove the node at back and delete it.
    void pop_back() {
        assert(!empty() && "pop_back() on a empty list!");
        iterator t = end();
        erase(--t);
    }

    iterator erase(iterator first, iterator last) {
        while (first != last)
            first = erase(first);
        return last;
    }
    
    void clear() { erase(begin(), end()); }
};