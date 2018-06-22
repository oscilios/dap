set (target dap_threadsafe)
set (headers
    ${CMAKE_CURRENT_SOURCE_DIR}/AtomicLock.h
    ${CMAKE_CURRENT_SOURCE_DIR}/RingBuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Stack.h
    )
add_library (${target} INTERFACE)
target_sources (${target} INTERFACE ${headers})
target_link_libraries (${target} INTERFACE dap_base)
add_subdirectory (test)
