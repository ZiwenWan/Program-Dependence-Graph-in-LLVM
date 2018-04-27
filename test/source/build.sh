#!/bin/bash

clang -emit-llvm -S *.c
for file in *.ll; do
  llvm-as "$file"
done
mv *.bc ../llvm_bc/
rm -f *.ll *.bc
