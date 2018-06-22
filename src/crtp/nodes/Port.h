#ifndef DAP_CRTP_PORT_H
#define DAP_CRTP_PORT_H

#include "base/Any.h"

namespace dap
{
    namespace crtp
    {
      class Node;
      class Port;
    }
}

// runtime class for accessing inputs thru virtual interface
// TODO: add input names to ports
class dap::crtp::Port final
{
    Any m_data;
    dap::crtp::Node* m_parent;
    uint32_t m_id;

public:
    template <typename T>
    Port(Node* parent, uint32_t id, const T& data = T{})
    : m_data(data)
    , m_parent(parent)
    , m_id(id)
    {
    }
    uint32_t getId() const
    {
        return m_id;
    }
    const dap::crtp::Node* getParent() const
    {
        return m_parent;
    }
    template <typename T>
    const T& get() const
    {
        return *m_data.to<T*>();
    }
    template <typename T>
    void set(const T& x)
    {
        *m_data.to<T*>() = x;
    }
};

#endif // DAP_CRTP_PORT_H
