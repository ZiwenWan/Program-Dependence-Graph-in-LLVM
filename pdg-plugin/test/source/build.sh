#!/bin/bash

clang -emit-llvm -S *.c
for file in *.ll; do
  llvm-as "$file"
done
mv *.bc ../llvm_bc/
mv *.ll ../llvm_ll/
rm -f *.ll *.bc
