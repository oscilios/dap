set (target dap_fastmath)
set (headers
    ${CMAKE_CURRENT_SOURCE_DIR}/AlignedVector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AudioBuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Taylor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/VarArray.h
    ${CMAKE_CURRENT_SOURCE_DIR}/VariableUnaryExpressions_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Array.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FastmathAlignedAllocator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TypeTraits.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Variable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/array_ops_eigen_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ArrayOps.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Function.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Var.h
    ${CMAKE_CURRENT_SOURCE_DIR}/VariableBinaryExpressions_impl.h
    )
add_library (${target} INTERFACE)
target_sources (${target} INTERFACE ${headers})
#if (NOT CMAKE_BUILD_TYPE MATCHES Release)
    message (STATUS "EIGEN_NO_MALLOC defined")
    target_compile_definitions (${target} INTERFACE -DEIGEN_NO_MALLOC)
#endif ()
add_subdirectory (test)
