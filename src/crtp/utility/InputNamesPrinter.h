#ifndef CRTP_UTILITY_INPUT_NAMES_PRINTER_H
#define CRTP_UTILITY_INPUT_NAMES_PRINTER_H

#include "NodeVisitor.h"
#include <iostream>

namespace dap
{
    namespace crtp
    {
        class InputNamesPrinter;
    }
}
class dap::crtp::InputNamesPrinter
{
    template <typename T>
    void printInputNames(T t)
    {
        std::cout << "\t" << to_string(t) << std::endl;
    }
    template <typename T, typename... Ts>
    void printInputNames(T t, Ts... ts)
    {
        printInputNames(t);
        printInputNames(ts...);
    }
    template <typename... Ts, size_t... Is>
    void printInputNames(std::tuple<Ts...> t, std::index_sequence<Is...>&&)
    {
        printInputNames(std::get<Is>(t)...);
    }

public:
    template <typename... Ts>
    void visit(ProcessorNode<Ts...>& node)
    {
        std::cout << "ProcessorNode with " << node.inputCount() << " inputs" << std::endl;
        using Indices = std::make_integer_sequence<std::size_t, ProcessorNode<Ts...>::inputCount()>;
        printInputNames(node.inputNames(), Indices{});
    }
    template <typename T>
    void visit(ValueNode<T>& node)
    {
        std::cout << "ValueNode" << std::endl;
    }
    template <typename T, DAP_REQUIRES(std::is_fundamental<T>::value || std::is_enum<T>::value)>
    void visit(T& t)
    {
        std::cout << "value: " << t << std::endl;
    }
    template <typename... Ts>
    void visit(Ts&...)
    {
        // nothing to print
    }
};

#endif // CRTP_UTILITY_INPUT_NAMES_PRINTER_H
