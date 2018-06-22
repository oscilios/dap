set (target dap_base)
set (headers
    AbstractFactory.h
    Constants.h
    KeyValueTuple.h
    Streamable.h
    SystemCommon.h
    ThreadJoiner.h
    Any.h
    ContainerView.h
    Semaphore.h
    SystemAnnotations.h
    Thread.h
    TypeTraits.h
    VariadicOps.h
    )
set (sources
    Semaphore.cpp
    )
add_library (${target} ${sources} ${headers})
if (CMAKE_SYSTEM_NAME MATCHES "Darwin") # depends on coreaudio for setRealtimePriority in SystemCommon.h
    find_library (COREAUDIO_LIBRARY CoreAudio)
    target_link_libraries (${target} ${COREAUDIO_LIBRARY})
endif()
set_target_properties (${target} PROPERTIES LINKER_LANGUAGE CXX)
add_subdirectory (test)
