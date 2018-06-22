#ifndef DAP_BASE_CONTAINER_VIEW_H
#define DAP_BASE_CONTAINER_VIEW_H

#include <iterator>
#include <algorithm>

namespace dap
{
    template <typename Container>
    class ContainerView;
}

template <typename Container>
class dap::ContainerView
{
public:
    using iterator       = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;
    using value_type     = typename Container::value_type;

private:
    iterator m_begin;
    iterator m_end;

public:
    explicit ContainerView(Container& container)
    : m_begin{std::begin(container)}
    , m_end{std::end(container)}
    {
    }
    ContainerView(iterator begin, iterator end)
    : m_begin{begin}
    , m_end{end}
    {
    }
    iterator begin()
    {
        return m_begin;
    }
    iterator end()
    {
        return m_end;
    }
    const_iterator begin() const
    {
        return m_begin;
    }
    const_iterator end() const
    {
        return m_end;
    }
    bool contains(const value_type& value) const
    {
        return std::find(begin(), end(), value) != end();
    }
    template <typename UnaryFunction>
    ContainerView& apply(UnaryFunction&& f)
    {
        std::for_each(begin(), end(), f);
        return *this;
    }
};

#endif // DAP_BASE_CONTAINER_VIEW_H
