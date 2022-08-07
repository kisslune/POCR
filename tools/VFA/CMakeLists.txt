set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(vf vf.cpp)

target_link_libraries(vf POCR_LIB ${SVF_LIB} ${llvm_libs})

set_target_properties(vf PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
