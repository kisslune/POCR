set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(cfl cfl.cpp)

target_link_libraries(cfl POCR_LIB ${SVF_LIB} ${llvm_libs})

set_target_properties(cfl PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
