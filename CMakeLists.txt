cmake_minimum_required(VERSION 3.4.3)
project(pdg)
find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

#
# We will build one library: libtmplugin.so.  It corresponds to a plugin that we
# run when compiling.
#
#option(USE_CXX_EXCEPTIONS "Enable C++ exception support" ON)
#
# Files associated with libtmplugin.so
#
option(ENABLE_FUNC_LIST "generate result based on func list" ON)

include_directories("include")
include_directories("src")

file(GLOB TPPSRC "src/*.tpp")
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.hpp")

add_library(pdg MODULE
    include/tree.hh
    ${HEADERS}
    ${TPPSRC}
    ${SOURCES}
)

if (ENABLE_FUNC_LIST)
    add_definitions(-DFUNC_LIST)
endif()

#
# Turn on C++11, turn off C++ RTTI.
#
target_compile_features(pdg PRIVATE cxx_range_for cxx_auto_type)
set_target_properties(pdg PROPERTIES COMPILE_FLAGS "-fno-rtti")

#
# OS X-specific configuration
#
if(APPLE)
    set_target_properties(pdg PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)

