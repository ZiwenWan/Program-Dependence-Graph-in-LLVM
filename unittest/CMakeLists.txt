cmake_minimum_required(VERSION 3.1)
project(pdg-test)
find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

message(STATUS "include directory: " ${LLVM_INCLUDE_DIRS})
#
# We will build one library: libtmplugin.so.  It corresponds to a plugin that we
# run when compiling.
#

#
# Files associated with libtmplugin.so
#
add_executable(pdg-test test_ddg.cpp)

#
# Turn on C++11, turn off C++ RTTI.
#
target_compile_features(pdg-test PRIVATE cxx_range_for cxx_auto_type)
set_target_properties(pdg-test PROPERTIES COMPILE_FLAGS "-fno-rtti")
llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(pdg-test ${llvm_libs})

#
# OS X-specific configuration
#
if(APPLE)
    set_target_properties(pdg-test PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)


