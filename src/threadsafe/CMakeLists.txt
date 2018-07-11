set (target dap_threadsafe)
set (headers
    ${CMAKE_CURRENT_SOURCE_DIR}/AtomicLock.h
    )
add_library (${target} INTERFACE)
target_sources (${target} INTERFACE ${headers})
