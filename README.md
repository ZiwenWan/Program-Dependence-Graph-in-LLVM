# PDG (llvm 5.0) 

## Project Intro

This project aims at building a program dependency graph(PDG) for C program. The program dependency graph buliding  consists of two part 

1. Control Dependency Graph 
2. Data Dependency Graph

The built program dependency graph is field senstive, context-insensitive, flow-insensitive.

## How to use

```shell
mkdir build
cd build
cmake ..
make
opt -load libpdg.so -dot-pdg < test.bc
```

After above commands, a dot file will be created. Open it with [Graphviz](http://www.graphviz.org/).

## How to generate bc file

Just write a valid C program and then use. (test.c in this example)

> **clang -emit-llvm -S test.c**

This should give you **test.ll**.

Then, use the llvm-as tool to generate bc file.

> **llvm-as test.ll**

This should give you the bc file needed for testing.

## Avaliable Passes

**-pdg:** build program dependency graph in memory (inter-procedural)

**-cdg:** build control dependency graph in memory (intra-procedural)

**-ddg:** build data dependency graph in memory (intra-procedural)

**-dot-*:** virtualize above dependency dependency. (dot)

## Running tests
In this project, we use catch2 to build the project.
Catch2 is a light wegith C++ testing framework. As catch2 can be used with a couple stand alone header files, they are included in the project.
It is put under directory lib.
When building the project, the test is also built. 
We build all tests into an executable. User can verify the basic utitlies in PDG by running the following command:
> ./build/test/pdg-test

The test case is built in the pdgtest.cpp file, which is placed under test directory. 

A PDG example looks like this:
![Image of PDG]
(https://bitbucket.org/psu_soslab/pdg-llvm5.0/src/pdg_plugin/demo/pdg.svg)
