# llvm-pdg-5.0

## Outline of the project

This project aims at printing a program dependency for a program. It is a upgraded version of an old PDG program written in llvm-3.5.
The program dependency is consisted of two part 
  1. Control Dependency Graph
  2. Data Dependency Graph

## Control Dependency Graph
Control Dependency Graph is built based on the PostDominantTree Pass in LLVM. 

## Data Dependency Graph
Data Dependency Graph is consisted of Def-use chain and flow dependency analysis. 
Currently, the flow dependency analysis module has not been upgraded.  

## How to use
This project is built accord to llvm 5.0 build system. 

### Using Plug-in 
To get start quickly. Simply **type** 

1. **mkdir build**

2. make

3. **opt -load ./build/libpdg.so -dot-pdg path_to_test/test_file.bc**

Then, you will see a couple of dot files are generated. Click the one named by **pdgraph.main.dot** to view (Need a dot viewer).

### Using CMake

The other way that accord to llvm 5.0 and above buld is described as follow:

1. Copy all the files in the repository to the **lib/Analysis/PDG** 

The directory **PDG** is created by yourself and can be renamed to whatever names.

2. Rename **CMakeLists_For_Standard_Build.txt** to **CMakeLists.txt** (Remember firstly change the original **CMakeLists.txt** to other name).
3. Modify the **CMakelist.txt** file in the **lib/Analysis** directory and add **add_subdirecoty("PDG")**
4. **cd llvm/build/** (Assume you have already build from source)
5. make

Then, one can print control dependency graph or data dependency graph by following command (should be ran ):

> opt -load path_to_your_so/LLVMCDG.so -dot-cdg path_to_test/test_file.bc 

## How to generate bc file?
Just write a valid C program and then use. (test.c in this example)

> **clang -emit-llvm -S test.c**

This should give you test.ll, which is represented in llvm IR. 

Then, use the llvm-as tool to generate bc file.

> **llvm-as test.ll** 

This should give you the bc file needed for testing.
