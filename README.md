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
Just copy all the files in the repository to the lib/Analysis/CDG 

The directory CDG is created by yourself and can be renamed to whatever names.

Then, modify the Cmakelist file in the lib/Analysis directory and add add_subdirecoty("CDG")

This should get the llvm to build the pass.

Then, one can print control dependency graph or data dependency graph by following command (should be ran ):

> opt -load path_to_your_so/LLVMCDG.so -dot-cdg path_to_test/test_file.bc 

## How to generate bc file?
Just write a valid C program and then use. (test.c in this example)

> clang -emit-llvm -S test.c

This should give you test.ll, which is represented in llvm IR. 

Then, use the llvm-as tool to generate bc file.

> llvm-as test.ll 

This should give you the bc file needed for testing.
