//
// Created by Sayama on 09/01/2026.
//

#pragma once

#include "../Core/Concepts.hpp"
#include "TMutex.hpp"

namespace Todo
{
    template<typename T, CMutex Mutex = std::mutex>
    class ThreadsafeList
    {
    private:
        struct Node
        {
            Node() : next() {}
            Node(const T& d) : data(std::make_shared<T>(d)) {};
            TMutex<Mutex> m;
            std::shared_ptr<T> data;
            std::unique_ptr<Node> next;
        };
    public:
        ThreadsafeList();
        ~ThreadsafeList();
        ThreadsafeList(const ThreadsafeList&) = delete;
        ThreadsafeList& operator=(const ThreadsafeList&) = delete;
    public:
        void push_front(const T& value);

        template<typename Function>
        void for_each(Function f);

        template<std::predicate Predicate>
        std::shared_ptr<T> find_first_if(Predicate p);

        template<std::predicate Predicate>
        void remove_if(Predicate p);


    private:
        Node head;
    };

    template <typename T, CMutex Mutex>
    ThreadsafeList<T, Mutex>::ThreadsafeList() {}

    template <typename T, CMutex Mutex>
    ThreadsafeList<T, Mutex>::~ThreadsafeList()
    {
        remove_if([](const Node&){return true;});
    }

    template <typename T, CMutex Mutex>
    void ThreadsafeList<T, Mutex>::push_front(const T& value)
    {
        std::unique_ptr<Node> newNode{new Node(value)};
        auto lock = head.m.UniqueGuard();
        newNode->next = std::move(head.next);
        head.next = std::move(newNode);
    }

    template <typename T, CMutex Mutex>
    template <typename Function>
    void ThreadsafeList<T, Mutex>::for_each(Function f)
    {
        auto lock = head.m.UniqueGuard();
        auto* current = &head;
        while (Node* next = current->next.get())
        {
            auto lk = next->m.UniqueGuard();
            lock.unlock();
            f(*next->data);
            current = next;
            lock = std::move(lk);
        }
    }

    template <typename T, CMutex Mutex>
    template <std::predicate Predicate>
    std::shared_ptr<T> ThreadsafeList<T, Mutex>::find_first_if(Predicate p)
    {
        auto lock = head.m.UniqueGuard();
        auto* current = &head;
        while (Node* next = current->next.get())
        {
            auto lk = next->m.UniqueGuard();
            lock.unlock();
            if (p(*next->data))
            {
                return next->data;
            }
            current = next;
            lock = std::move(lk);
        }
        return nullptr;
    }

    template <typename T, CMutex Mutex>
    template <std::predicate Predicate>
    void ThreadsafeList<T, Mutex>::remove_if(Predicate p)
    {
        auto lock = head.m.UniqueGuard();
        auto* current = &head;
        while (Node* next = current->next.get())
        {
            auto lk = next->m.UniqueGuard();
            if (p(*next->data))
            {
                std::unique_ptr<Node> old_next = std::move(current->next);
                current->next = std::move(next->next);
                lk.unlock();
            }
            else
            {
                lock.unlock();
                current = next;
                lock = std::move(lk);
            }
        }
    }
} // Todo