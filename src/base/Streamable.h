#ifndef DAP_BASE_STREAMABLE_H
#define DAP_BASE_STREAMABLE_H

#include <string>
#include <utility>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <ostream>
#include <sstream>

#ifdef _WIN32
// nothing to add
#else
#include <cxxabi.h> // for demangling type_info names
#endif

namespace dap
{

    template <typename T>
    inline std::string demangle()
    {
        std::string name = typeid(T).name();
#ifdef _WIN32
        return name;
#else
        int status = -1;
        std::unique_ptr<char, void (*)(void*)> res{
            abi::__cxa_demangle(name.data(), nullptr, nullptr, &status), std::free};
        return (status == 0) ? res.get() : name.data();
#endif
    }

    inline std::string demangle(const char* name)
    {
#ifdef _WIN32
        return name;
#else
        int status = -1;
        std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, nullptr, nullptr, &status),
                                                   std::free};
        return (status == 0) ? res.get() : name;
#endif
    }

    template <typename T, typename U>
    inline std::ostream& operator<<(std::ostream& out, const std::pair<T, U>& p)
    {
        // out << "type:" <<  demangle<std::pair<T,U>>();
        out << p.first << ":" << p.second;
        return out;
    }

    // containers

    template <typename T>
    inline std::ostream& streamContainer(std::ostream& out, const T& c)
    {
        // out << "type: " << demangle<T>();
        out << "(" << c.size() << ") [ ";
        for (const auto& x : c)
        {
            out << x << " ";
        }
        out << "]";
        return out;
    }
    template <typename T>
    inline std::ostream& operator<<(std::ostream& out, const std::vector<T>& c)
    {
        return streamContainer(out, c);
    }
    template <typename T>
    inline std::ostream& operator<<(std::ostream& out, const std::set<T>& c)
    {
        return streamContainer(out, c);
    }
    template <typename T>
    inline std::ostream& operator<<(std::ostream& out, const std::list<T>& c)
    {
        return streamContainer(out, c);
    }
    template <typename T, typename U>
    inline std::ostream& operator<<(std::ostream& out, const std::map<T, U>& c)
    {
        return streamContainer(out, c);
    }
} // namespace

#endif // DAP_BASE_STREAMABLE_H
