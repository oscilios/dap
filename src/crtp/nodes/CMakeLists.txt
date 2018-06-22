set (target dap_crtp_nodes)
set (headers
    ${CMAKE_CURRENT_SOURCE_DIR}/Node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/make_node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/BinaryNodeOpsImpl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/UnaryNodeOpsImpl.hpp
    )
add_library (${target} INTERFACE)
target_sources (${target} INTERFACE ${headers})
target_link_libraries (${target} INTERFACE dap_base)
add_subdirectory (test)
