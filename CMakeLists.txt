cmake_minimum_required(VERSION 3.1)
project(pdg)
find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
set (CMAKE_CXX_STANDARD 14)

#
# We will build one library: libtmplugin.so.  It corresponds to a plugin that we
# run when compiling.
#
#option(USE_CXX_EXCEPTIONS "Enable C++ exception support" ON)
#
# Files associated with libtmplugin.so
#

include_directories("include")
include_directories("src")

file(GLOB TPPSRC "src/*.tpp")
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.hpp")

#build static libray to be used by pdgtest
add_library(pdgStatic STATIC
    include/tree.hh
    ${HEADERS}
    ${TPPSRC}
    ${SOURCES}
)

# build plugin to be used by opt
add_library(pdg MODULE
    include/tree.hh
    ${HEADERS}
    ${TPPSRC}
    ${SOURCES}
)

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

