#pragma once

template <typename T>
struct OneWayNode
{
    OneWayNode* pNext;
    T data;

    OneWayNode(T data) : data(data), pNext(nullptr) {}

    // 迭代器类
    struct Iterator
    {
        OneWayNode* pNode;

        Iterator(OneWayNode* node) : pNode(node) {}

        // 重载解引用操作符
        T& operator*() const
        {
            return pNode->data;
        }

        // 重载后缀递增操作符
        Iterator operator++(int)
        {
            Iterator old = *this;
            pNode = pNode->pNext;
            return old;
        }

        // 重载相等操作符
        bool operator==(const Iterator& other) const
        {
            return pNode == other.pNode;
        }

        // 重载不等操作符
        bool operator!=(const Iterator& other) const
        {
            return !(*this == other);
        }
    };
};

template <typename T>
class OneWayLinkedList
{
private:
    typename OneWayNode<T>* pHead;
    typename OneWayNode<T>* pTail;

public:
    OneWayLinkedList() : pHead(nullptr), pTail(nullptr) {}

    // 返回链表的起始位置迭代器
    typename OneWayNode<T>::Iterator begin() const
    {
        return typename OneWayNode<T>::Iterator(pHead);
    }

    // 返回链表的结束位置迭代器
    typename OneWayNode<T>::Iterator end() const
    {
        return typename OneWayNode<T>::Iterator(nullptr);
    }

    void push_back(const T& v) noexcept
    {
        if (this->pHead == nullptr)
        {
            this->pHead = this->pTail = new OneWayNode<T>(v);
            return;
        }
        pTail->pNext = new OneWayNode(v);
    }
};