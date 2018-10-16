## Files used for testing:

Input : `kernel_example.c`
Output: `kernel_example.fieldaccess`

## Issue 1 - Receiving a struct field as an argument.
*[STATUS: TO RESOLVE]*

The read/write info for `new_fun` is shown as follows.
```
r function: new_fun]
[ new_fun ] Tree size: 2
Arg use information for arg no: 0
** Root type node **
Access Type: Write
Struct Layout is nullptr. Continue
```
While it correctly reports that a write operation is happening
in this function, it does not report the name of the structure field
that is being written upon. In this example, `a` is a reference
to `devops->devop_init_registered`, due to this call in `mimix_open`:
```
        new_fun(&devops->devop_init_registered);
```
 This is most likely because the 
current implementation reports read/writes for a structure field in a function, only 
when the field's entire parent structure is passed as an argument to the function. 

This should be easily fixable, since the implementation can track this alias relationship between `a` and `devop_init_registered`, as shown by the read/write output for `mimix_open`:
```
[For function: mimix_open]
[ mimix_open ] Tree size: 3
Arg use information for arg no: 0
** Root type node **
Access Type: Read
sub field name: open
Access Type: No Access
..................................................
sub field name: devop_init_registered
Access Type: Write
..................................................
```
From the above we can tell that `devop_init_registered` is undergoing a write access, which happens in the `new_fun` function.  

## Issue 2 - Main function
*[STATUS: TO RESOLVE]*

The main function does not report any read/write info. This may not be required at this stage, but we can reserve it for future discussion. 

### Commands used for testing

The following command was used to generate the .bc file from the
input .c file in the `build` folder. (No optimizations were used.)
```
$ clang -emit-llvm -g kernel_example.c -c -o kernel_example.bc
```

The following command was used to 
```
opt -load ./libpdg.so -dot-pdg kernel_example.bc >& kernel_example.fieldaccess
```
