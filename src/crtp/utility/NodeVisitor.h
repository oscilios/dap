#ifndef CRTP_UTILITY_NODE_VISITOR_H
#define CRTP_UTILITY_NODE_VISITOR_H

#include "crtp/nodes/Node.h"

// given a node, visits the node and its inputs recursively
namespace dap
{
    namespace crtp
    {
        template <typename Visitor>
        class NodeVisitor;

        template <typename Visitor, typename... Args>
        auto make_node_visitor(Args&&...);
    }
}
template <typename Visitor>
class dap::crtp::NodeVisitor
{
    Visitor m_visitor;
    template <typename T, DAP_REQUIRES(!traits::IsCrtpNode<T>::value)>
    void visit(T& t)
    {
        m_visitor.visit(t);
    }
    template <typename... T>
    void visit(std::tuple<T...>& t)
    {
        for_each(t, *this);
    }
    template <typename T, DAP_REQUIRES(traits::IsCrtpNode<T>::value)>
    void visit(T& node)
    {
        m_visitor.visit(node);
        for_each(node.inputs(), *this);
    }

public:
    NodeVisitor() = default;
    NodeVisitor(Visitor&& visitor)
    : m_visitor(std::move(visitor))
    {
    }
    template <typename T>
    void operator()(T& t)
    {
        visit(t);
    }
};

template <typename Visitor, typename... Args>
auto dap::crtp::make_node_visitor(Args&&... args)
{
    return NodeVisitor<Visitor>(Visitor{std::forward<Args>(args)...});
}

#endif // CRTP_UTILITY_NODE_VISITOR_H
