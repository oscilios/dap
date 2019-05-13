#ifndef DAP_CRTP_NODES_PROCESSOR_H
#define DAP_CRTP_NODES_PROCESSOR_H

#include "crtp/nodes/Node.h"

namespace dap
{
    namespace crtp
    {
        template <typename TProcessor>
        struct processor
        {
            template <typename... TInputs>
            struct with_inputs
            {
                template <typename... TInputNames>
                using with_names_t =
                    ProcessorNode<TProcessor, Node::Inputs<TInputs...>, TInputNames...>;

                template <typename... Names>
                static constexpr auto named(Names&&...)
                {
                    return with_names_t<std::tuple<Names...>>{};
                }
                template <char... Chars>
                static constexpr auto prefixed_by(constexpr_string<Chars...>&& prefix)
                {
                    using names_t =
                        decltype(make_input_names(std::forward<constexpr_string<Chars...>>(prefix),
                                                  std::index_sequence<sizeof...(TInputs)>{}));
                    return with_names_t<names_t>{};
                }
            };
        };
    }
}

#endif // DAP_CRTP_NODES_PROCESSOR_H
