clang -emit-llvm -S *.c

for f in *.ll; do
    llvm-as $f
done

mv *.ll ./llvm_ll
mv *.bc ./bitcode
