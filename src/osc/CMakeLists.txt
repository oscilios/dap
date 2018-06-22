set (target dap_osc)
set (headers
    OscMessageLogger.h
    OscReceiver.h
    OscSender.h
    )
set (sources
    OscMessageLogger.cpp
    OscReceiver.cpp
    OscSender.cpp
    )
add_library (${target} ${sources} ${headers})
target_link_libraries (${target} dap_base oscpack)
add_subdirectory (test)
add_subdirectory (util)
