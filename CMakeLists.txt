cmake_minimum_required(VERSION 3.13.4)

project("POCR-svf")

# To support both in- and out-of-source builds,
# we check for the presence of the add_llvm_loadable_module command.
# - if this command is not present, we are building out-of-source
if (NOT COMMAND add_llvm_library)
    if (DEFINED LLVM_DIR)
        set(ENV{LLVM_DIR} "${LLVM_DIR}")
    endif ()
    if (DEFINED ENV{LLVM_DIR})
        # We need to match the build environment for LLVM:
        # In particular, we need C++14 and the -fno-rtti flag
        set(CMAKE_CXX_STANDARD 14)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
        # add -std=gnu++14
        set(CMAKE_CXX_EXTENSIONS ON)

        if (CMAKE_BUILD_TYPE MATCHES "Debug")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fno-rtti")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -O0")
        else ()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fno-rtti")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
        endif ()

        find_package(LLVM REQUIRED CONFIG)

        list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
        include(AddLLVM)

        add_definitions(${LLVM_DEFINITIONS})
        include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

        if (LLVM_LINK_LLVM_DYLIB)
            set(llvm_libs LLVM)
        else ()
            llvm_map_components_to_libnames(llvm_libs bitwriter core ipo irreader instcombine instrumentation target linker analysis scalaropts support transformutils)
        endif ()

    else ()
        message(FATAL_ERROR "\
WARNING: The LLVM_DIR var was not set (required for an out-of-source build)!\n\
Please set this to environment variable to point to the LLVM build directory\
(e.g. on linux: export LLVM_DIR=/path/to/llvm/build/dir)")
    endif ()
else ()
    set(IN_SOURCE_BUILD 1)
endif ()

if (DEFINED Z3_DIR)
    set(ENV{Z3_DIR} "${Z3_DIR}")
endif ()
find_library(Z3_LIBRARIES NAMES libz3.a
        HINTS $ENV{Z3_DIR}
        PATH_SUFFIXES lib bin)
find_path(Z3_INCLUDES NAMES z3++.h
        HINTS $ENV{Z3_DIR}
        PATH_SUFFIXES include z3)
if (NOT Z3_LIBRARIES OR NOT Z3_INCLUDES)
    message(FATAL_ERROR "Z3 not found!")
endif ()
message(STATUS "Found Z3: ${Z3_LIBRARIES}")
message(STATUS "Z3 include dir: ${Z3_INCLUDES}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Z3_INCLUDES})


if (EXISTS "${SVF_DIR}")
else()
    set(SVF_DIR $ENV{SVF_DIR})
    if(EXISTS "${SVF_DIR}")
    else()
        message(FATAL_ERROR "\
WARNING: The SVF_DIR var was not set (required for an out-of-source build)!\n\
Please set this to environment variable to point to the SVF_DIR directory or set this variable to cmake configuration\n
(e.g. on linux: export SVF_DIR=/path/to/SVF/dir) \n or \n \n(make the project via: cmake -DSVF_DIR=your_path_to_SVF) ")
    endif()
endif()

set(SVF_LIB "${SVF_DIR}/Release-build/svf/libSvfCore.a")
set(LLVMCudd "${SVF_DIR}/Release-build/lib/CUDD/libCudd.a")
set(SVF_HEADER "${SVF_DIR}/svf/include") 
set(SVF_LLVM "${SVF_DIR}/svf-llvm/include")
set(SVF_Config "${SVF_DIR}/Release-build/include")
include_directories(${SVF_HEADER} ${SVF_LLVM} ${SVF_Config})


add_subdirectory(lib)
add_subdirectory(tools)
