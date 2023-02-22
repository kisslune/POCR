file (GLOB SOURCES
        CFLData/*.cpp
        CFLSolver/*.cpp
        AA/*.cpp
        VFA/*.cpp
)

add_llvm_library(POCR_LIB STATIC ${SOURCES})

if(DEFINED IN_SOURCE_BUILD)
    add_dependencies(Svf intrinsics_gen)
endif()
