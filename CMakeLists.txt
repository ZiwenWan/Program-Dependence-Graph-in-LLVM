if(WIN32 OR CYGWIN)
    set(LLVM_LINK_COMPONENTS Core Support)
endif()

add_llvm_loadable_module( LLVMCDG
    ControlDependencies.cpp
    DataDependencies.cpp
    ProgramDependencies.cpp
    PDGHelper.cpp
    ConnectFunctions.cpp
    FunctionWrapper.cpp
    DepPrinter.cpp
    DEPENDS
    intrinsics_gen
    PLUGIN_TOOL
    opt
    )
