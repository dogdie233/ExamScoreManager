#pragma once

template <typename T>
struct OneWayNode
{
    OneWayNode* pNext;
    T data;

    OneWayNode(T data) : data(data), pNext(nullptr) {}

    // ��������
    struct Iterator
    {
        OneWayNode* pNode;

        Iterator(OneWayNode* node) : pNode(node) {}

        // ���ؽ����ò�����
        T& operator*() const
        {
            return pNode->data;
        }

        // ���غ�׺����������
        Iterator operator++(int)
        {
            Iterator old = *this;
            pNode = pNode->pNext;
            return old;
        }

        // ������Ȳ�����
        bool operator==(const Iterator& other) const
        {
            return pNode == other.pNode;
        }

        // ���ز��Ȳ�����
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

    // �����������ʼλ�õ�����
    typename OneWayNode<T>::Iterator begin() const
    {
        return typename OneWayNode<T>::Iterator(pHead);
    }

    // ��������Ľ���λ�õ�����
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