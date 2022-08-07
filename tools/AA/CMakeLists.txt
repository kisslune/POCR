set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(aa aa.cpp)

target_link_libraries(aa POCR_LIB ${SVF_LIB} ${llvm_libs})

set_target_properties(aa PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
