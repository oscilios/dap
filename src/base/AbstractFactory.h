#ifndef DAP_BASE_ABSTRACT_FACTORY_H
#define DAP_BASE_ABSTRACT_FACTORY_H

#include "TypeTraits.h"
#include "Streamable.h"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <stdexcept>
#include <typeindex>

// Abstract factory that allows to call create methods with any number of constructor parameters

namespace dap
{
    template <typename T, typename... Args>
    class AbstractFactory;

    template <typename BaseType, typename ProductType, typename... Args>
    class Registrar;
}

template <typename T, typename... Args>
class dap::AbstractFactory final
{
    template <typename BaseType, typename ProductType, typename... Ts>
    friend class Registrar;

public:
    using Ptr  = std::unique_ptr<T>;
    using type = T;

    struct ProductNotFoundInFactory : std::exception
    {
        const char* what() const noexcept override
        {
            return "Could not find requested product or bad argument type ids given to create it.";
        }
    };
    struct ProductKeyAlreadyExist: std::exception
    {
        const std::string msg;
        explicit ProductKeyAlreadyExist(const std::string& name)
        : msg("Could not register type. Key " + name + " already exists")
        {
        }
        const char* what() const noexcept override
        {
            return msg.data();
        }
    };

    std::ostream& dump(std::ostream& out) const
    {
        for (auto p : getArguments())
        {
            out << "\t" << p.first << " (";
            const size_t size = p.second.size();
            if (size > 0)
            {
                for (size_t i = 0; i < size - 1; ++i)
                {
                    out << demangle(p.second[i].name()) << ", ";
                }
                out << demangle(p.second.back().name());
            }
            out << ")" << std::endl;
        }
        return out;
    }

private:
    // typedef Ptr (*FactoryMethod)(Args...);
    using FactoryMethod = Ptr (*)(Args&&...);

    // registered types
    static std::map<std::string, AbstractFactory::FactoryMethod>& getRegistry()
    {
        static std::map<std::string, AbstractFactory::FactoryMethod> Map{};
        return Map;
    }

    // creation arguments registered along with registered names
    static std::map<std::string, std::vector<std::type_index>>& getArguments()
    {
        static std::map<std::string, std::vector<std::type_index>> args{};
        return args;
    }

    static bool register_(const char* name, AbstractFactory::FactoryMethod createMethod)
    {
        auto p = AbstractFactory::getRegistry().insert(std::make_pair(name, createMethod));
        if (p.second)
        {
            // insert argument's type ids to be able to print them for debugging
            getArguments().insert(std::make_pair(name, get_parameter_pack_type_ids<Args...>()));
        }
        else
        {
            throw ProductKeyAlreadyExist(name);
        }
        return p.second; // true if it was inserted
    }

public:
    static Ptr create(const char* name, Args&&... args)
    {
        const auto types = AbstractFactory::getRegistry();
        auto p = types.find(name);
        if (p == types.end())
        {
            throw ProductNotFoundInFactory();
        }
        return p->second(std::forward<Args>(args)...);
    }
};

template <typename T, typename ProductType, typename... Args>
class dap::Registrar
{
    using ConcreteFactory = AbstractFactory<T, Args&&...>;
    static const volatile bool value;

public:
    static typename ConcreteFactory::Ptr create(Args&&... args)
    {
        return std::make_unique<ProductType>(std::forward<Args>(args)...);
    }
};

namespace dap
{
    // register a product given its basic type and concrete type
    template <typename BaseType, typename... Args>
    std::unique_ptr<BaseType> createProduct(const std::string& productName, Args&&... args)
    {
        using Factory = AbstractFactory<BaseType, Args&&...>;
        return Factory::create(productName, std::forward<Args>(args)...);
    }

    // dump Product names and its constructor arguments registered in the factory
    template <typename T, typename... Args>
    inline std::ostream& operator<<(std::ostream& out, const AbstractFactory<T, Args...>& factory)
    {
        factory.dump(out);
        return out;
    }
} // dap

#endif
