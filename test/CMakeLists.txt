# set C++ standard
set (CMAKE_CXX_STANDARD 11)
include_directories("../include")
include_directories("../lib/catch2")
include_directories("../src")
add_executable(pdg-test "pdgtest.cpp")
# find_package(Catch2 REQUIRED)
llvm_map_components_to_libnames(llvm_libs support core irreader passes)
target_link_libraries(pdg-test ${llvm_libs} pdgStatic)
