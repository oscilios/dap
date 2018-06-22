set (target dap_crtp_utility)
set (headers
    ${CMAKE_CURRENT_SOURCE_DIR}/NodeVisitor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/InputNamesPrinter.h
    )
add_library (${target} INTERFACE)
target_sources (${target} INTERFACE ${headers})
target_link_libraries (${target} INTERFACE dap_base dap_crtp_nodes)
