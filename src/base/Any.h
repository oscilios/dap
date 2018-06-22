#ifndef DAP_BASE_ANY_H
#define DAP_BASE_ANY_H

#include <typeinfo>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <utility>
#include "TypeTraits.h"
#include "Streamable.h"
#include "SystemAnnotations.h"

// class to hold any type of element.

namespace dap
{
    class Any;
}

class dap::Any
{
    template <class T>
    using decay_type = typename std::decay<T>::type; // so T, T& and T&& are considered as same type

    struct any_bad_typeid : std::bad_typeid
    {
        char m_msg[128];
        const char* fmt = "Wrong cast in Any, from type '%s' to type '%s'";
        any_bad_typeid(const std::type_info& from, const std::type_info& to)
        {
            std::snprintf(m_msg, 128, fmt, from.name(), to.name());
        }
        const char* what() const noexcept override
        {
            return m_msg;
        }
    };
    struct any_is_null : std::exception
    {
        const char* what() const noexcept override
        {
            return "Any is null";
        }
    };

    struct Base
    {
        Base() = default;
        Base(const Base&) = default;
        Base(Base&&) noexcept = default; // NOLINT
        Base& operator=(const Base&) = default;
        Base& operator=(Base&&) noexcept = default; // NOLINT
        virtual ~Base() = default;
        virtual std::unique_ptr<Base> clone() const = 0;
        virtual std::string toString() const = 0;
    };

    template <typename T>
    struct Derived : Base
    {
        T value;

        explicit Derived(const T&  val) // NOLINT
        : value(val)
        {
        }
        explicit Derived(T&& val)
        : value(static_cast<T&&>(val))
        {
        }
        std::unique_ptr<Base> clone() const override
        {
            return std::move(std::make_unique<Derived<T>>(value));
        }
        std::string toString() const override
        {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }
    };

    std::unique_ptr<Base> m_ptr;
    const std::type_info& m_typeInfo;
    using void_t = void;

public:
    // default constructor
    explicit Any() noexcept : m_ptr(nullptr), m_typeInfo(typeid(void_t))
    {
    }

    // universal reference ctor (consumes l-value and r-value references)
    template <typename T,
              DAP_REQUIRES(!std::is_same<T, Any>::value && std::is_move_constructible<T>::value)>
    explicit Any(T&& uref) //  NOLINT [misc-forwarding-reference-overload]
    : m_ptr(std::make_unique<Derived<decay_type<T>>>(std::forward<T>(uref)))
    , m_typeInfo(typeid(T))
    {
    }

    // copy ctor, takes an l-value reference
    Any(const Any& other)
    : m_ptr(other.m_ptr ? other.m_ptr->clone() : nullptr)
    , m_typeInfo(other.type_info())
    {
    }

    // move ctor, takes an r-value reference. other will become invalid after being moved from it
    Any(Any&& other) noexcept
    : m_ptr(std::move(other.m_ptr))
    , m_typeInfo(other.type_info())
    {
        other.m_ptr = nullptr;
    }
    Any& operator=(const Any& other)
    {
        if (&other == this)
        { // protect against self assignment
            return *this;
        }

        if (m_typeInfo != other.m_typeInfo)
        {
            throw any_bad_typeid(other.m_typeInfo, m_typeInfo);
        }

        m_ptr = other.m_ptr->clone();
        return *this;
    }
    Any& operator=(const Any&& other) noexcept = delete;
    template <typename T>
    Any& operator=(const T& x)
    {
        if (!sameType(x))
        {
            throw any_bad_typeid(typeid(T), m_typeInfo);
        }
        dynamic_cast<Derived<T>*>(m_ptr.get())->value = x;
        return *this;
    }
    ~Any() = default;
    template <typename T>
    T& to()
    {
        if (!sameType<T>())
        {
            throw any_bad_typeid(typeid(T), m_typeInfo);
        }
        if (isNull())
        {
            throw any_is_null();
        }
        return dynamic_cast<Derived<T>*>(m_ptr.get())->value;
    }
    template <typename T>
    const T& to() const
    {
        if (!sameType<T>())
        {
            throw any_bad_typeid(typeid(T), m_typeInfo);
        }
        if (isNull())
        {
            throw any_is_null();
        }
        return dynamic_cast<Derived<T>*>(m_ptr.get())->value;
    }
    template <typename T>
    bool operator==(const T& x) const noexcept
    {
        if (!sameType(x))
        {
            return false;
        }
        return x == to<T>();
    }
    template <typename T>
    bool sameType(const T& x) const noexcept
    {
        return m_typeInfo == typeid(x);
    }
    template <typename T>
    bool sameType() const noexcept
    {
        return m_typeInfo == typeid(T);
    }
    bool isNull() const noexcept
    {
        return m_ptr == nullptr;
    }
    const std::type_info& type_info() const noexcept
    {
        return m_typeInfo;
    }
    std::string toString() const
    {
        return m_ptr->toString();
    }
};

#endif // DAP_BASE_ANY_H
