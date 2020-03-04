# Parameter-tree based Program Dependence Graph (PDG)

## Introduction

This project is a key component of our PtrSplit and Program-mandering works. It aims at building a modular inter-procedural program dependence graph (PDG) for practical use. 
Our program dependence graph is field senstive, context-insensitive and flow-insensitive. For more details, welcome to read our CCS'17 paper about PtrSplit:
[http://www.cse.psu.edu/~gxt29/papers/ptrsplit.pdf] If you find this tool useful, please cite the PtrSplit paper in your publication. Here's the bibtex entry:

@inproceedings{LiuTJ17Ptrsplit,

  author    = {Shen Liu and Gang Tan and Trent Jaeger},
  
  title     = {{PtrSplit}: Supporting General Pointers in Automatic Program Partitioning},
  
  booktitle = {24th ACM Conference on Computer and Communications Security ({CCS})},
  
  pages     = {2359--2371},
  
  year      = {2017}
  
}


We have upgraded the implementation to LLVM 9.0.0. Currently, we only support building PDGs for C programs.

A PDG example looks like this (the blue part corresponds to the parameter tree):

![](https://bitbucket.org/psu_soslab/program-dependence-graph-in-llvm/raw/4000cf407e9aeb44491eb41b2e808b16e61dc192/demo/pdg.png)

## Getting started quickly

```shell
mkdir build
cd build
cmake ..
make
opt -load libpdg.so -dot-pdg < test.bc
```

Once you finish these operations a dot file will be created. You can open it with [Graphviz](http://www.graphviz.org/).

## LLVM IR compilation

For simple C programs(e.g., test.c), do

> **clang -emit-llvm -S test.c -o test.bc**

Now you have a binary format LLVM bitcode file which can be directly used as the input for PDG generation.

You can also generate a human-readable bitcode file(.ll) if you would like to:

> **llvm-dis test.bc**

This will generate a human-readable format bitcode file (test.ll) for your debugging and testing.

For those large C software (e.g., wget), you can refer to this great article for help:

http://gbalats.github.io/2015/12/10/compiling-autotooled-projects-to-LLVM-bitcode.html

(We successfully compiled SPECCPU 2006 INT/thttpd/wget/telnet/openssh/curl/nginx/sqlite, thanks to the author!)

## Avaliable Passes

**-pdg:** generate the program dependence graph (inter-procedural)

**-cdg:** generate the control dependence graph (intra-procedural)

**-ddg:** generate the data dependence graph (intra-procedural)

**-dot-*:** for visualization. (dot)

For those large software, generating a visualizable PDG is not easy. Graphviz often fails to generate the .dot file for a program with
more than 1000 lines of C code. Fortunately, we rarely need such a large .dot file but only do kinds of analyses on the PDG, which is always in memory.

## Running tests
We use catch2 to build the project.
Catch2 is a lightweight C++ testing framework. As catch2 can be used with a couple stand alone header files, they are included in the project.
It is put under directory lib.
When building the project, the test is also built. 
We build all tests into an executable. User can verify the basic utitlies in PDG by running the following command:
> ./build/test/pdg-test

The test case is built in the pdgtest.cpp file, which is placed under test directory. 
