file (GLOB SOURCES
        CFLSolver/*.cpp
        CFLGraphs/*.cpp
        AA/*.cpp
        VFA/*.cpp
)

add_llvm_library(POCR_LIB STATIC ${SOURCES})

if(DEFINED IN_SOURCE_BUILD)
    add_dependencies(Svf intrinsics_gen)
endif()
