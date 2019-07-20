# Parameter-tree based Program Dependence Graph (PDG)

## Project Introduction

This project is a major part of our PtrSplit work. It aims at building a modular inter-procedural program dependency graph(PDG) for practical use. 
Our program dependency graph is field senstive, context-insensitive, flow-insensitive. For more details, please refer to our CCS'17 paper:
[http://www.cse.psu.edu/~gxt29/papers/ptrsplit.pdf]

A PDG example looks like this(the blue part corresponds to the parameter tree):
![](https://bitbucket.org/psu_soslab/pdg-llvm5.0/raw/34cf0959fae4c3507889785c15779db4355af36b/demo/pdg.svg)

We have upgraded the implementation to LLVM 5.0.0 now.


## How to get started

```shell
mkdir build
cd build
cmake ..
make
opt -load libpdg.so -dot-pdg < test.bc
```

Once you finish these operations a dot file will be created. You can open it with [Graphviz](http://www.graphviz.org/).

## LLVM bitcode compilation

For simple C programs(e.g. test.c)

> **clang -emit-llvm -S test.c**

This should give you **test.ll**.

Then, use the llvm-as tool to generate bc file.

> **llvm-as test.ll**

This should give you the bc file needed for testing.

For large practical software written in C, please refer to this great article for help:

http://gbalats.github.io/2015/12/10/compiling-autotooled-projects-to-LLVM-bitcode.html

(We successfully compiled thttpd/wget/telnet/openssh/nginx by following this article, thanks to the author!)

## Avaliable Passes

**-pdg:** build program dependency graph in memory (inter-procedural)

**-cdg:** build control dependency graph in memory (intra-procedural)

**-ddg:** build data dependency graph in memory (intra-procedural)

**-dot-*:** virtualize above dependency dependency. (dot)

## Running tests
We use catch2 to build the project.
Catch2 is a light wegith C++ testing framework. As catch2 can be used with a couple stand alone header files, they are included in the project.
It is put under directory lib.
When building the project, the test is also built. 
We build all tests into an executable. User can verify the basic utitlies in PDG by running the following command:
> ./build/test/pdg-test

The test case is built in the pdgtest.cpp file, which is placed under test directory. 
