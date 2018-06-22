#ifndef DAP_THREADSAFE_STACK_H
#define DAP_THREADSAFE_STACK_H

#include <atomic>
#include <iostream>
#include <memory>
#include <utility>

namespace dap
{
    namespace threadsafe
    {
        template <typename T>
        class Stack;
    }
}

template <typename T>
class dap::threadsafe::Stack final
{
    struct Node
    {
        T value{};
        std::shared_ptr<Node> next{nullptr};
    };

    std::shared_ptr<Node> m_head{
        nullptr}; // use atomic ops on this ptr, in C++17 it replace by atomic_shared_ptr

public:
    Stack()                 = default;
    Stack(const Stack&)     = delete;
    Stack(Stack&&) noexcept = delete;
    Stack& operator=(const Stack&) = delete;
    Stack& operator=(Stack&&) = delete;

    ~Stack()
    {
        while (auto p = pop_front())
        {
        }
    }

    class Reference
    {
        std::shared_ptr<Node> m_ptr{nullptr};

    public:
        explicit Reference(std::shared_ptr<Node> p)
        : m_ptr(std::move(p))
        {
        }
        T& operator*()
        {
            return m_ptr->value;
        }
        const T& operator*() const
        {
            return m_ptr->value;
        }
        T* operator->()
        {
            return &m_ptr->value;
        }
        const T* operator->() const
        {
            return &m_ptr->value;
        }
        explicit operator bool() const
        {
            return m_ptr != nullptr;
        }
    };
    template <typename Function>
    void foreach (Function f)
    {
        auto p = std::atomic_load_explicit(&m_head, std::memory_order_acquire);
        while (p)
        {
            f(*p);
            p = p->next;
        }
    }
    auto find(const T& value) const
    {
        auto p = std::atomic_load_explicit(&m_head, std::memory_order_acquire);
        while (p && p->value != value)
        {
            p = p->next;
        }
        return Reference{std::move(p)};
    }
    void push_front(const T& value)
    {
        auto p   = std::make_shared<Node>();
        p->value = value;
        p->next  = m_head;
        while (!std::atomic_compare_exchange_weak_explicit(
            &m_head, &p->next, p, std::memory_order_release, std::memory_order_relaxed))
        {
        }
    }
    void emplace_front(T&& value)
    {
        auto p   = std::make_shared<Node>();
        p->value = std::move(value);
        p->next  = m_head;
        while (!std::atomic_compare_exchange_weak_explicit(
            &m_head, &p->next, p, std::memory_order_release, std::memory_order_relaxed))
        {
        }
    }
    auto pop_front()
    {
        auto p = std::atomic_load_explicit(&m_head, std::memory_order_relaxed);
        while (p && !std::atomic_compare_exchange_weak_explicit(
                        &m_head, &p, p->next, std::memory_order_release, std::memory_order_relaxed))
        {
        }
        return Reference{std::move(p)};
    }
};

#endif // DAP_THREADSAFE_STACK_H
