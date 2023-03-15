set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(fp fp.cpp)

target_link_libraries(fp POCR_LIB ${SVF_LIB} ${llvm_libs})

set_target_properties(fp PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
