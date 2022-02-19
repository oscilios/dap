set (target dap_crtp_nodes)
add_library (${target} INTERFACE)
target_sources (${target} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Processor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/private/BinaryNodeOpsImpl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/UnaryNodeOpsImpl.hpp
    )
target_link_libraries (${target} INTERFACE dap_base)
add_subdirectory (test)
