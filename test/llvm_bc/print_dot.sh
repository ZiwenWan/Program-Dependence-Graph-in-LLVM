#!/bin/bash

pdg_path="/home/yongzhe/llvm-versions/llvm-5.0/build/lib/"
bin_name="LLVMCDG.so"

for file in *.bc; do
    opt -load "$pdg_path/$bin_name" -dot-pdg < $file > /dev/null
done

for file in *.dot; do
    filelen=${#file}
    echo $filelen
    filelen=$(($filelen - 4))
    echo $filelen
    file_sub_string=$(echo $file | cut -c $file 1-$(($filelen)) )
    dot -Tps "$file" -o "$file_sub_string.ps"
done
