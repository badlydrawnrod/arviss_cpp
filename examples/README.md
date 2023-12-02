# Building and running the examples

This is what I'm currently doing. It isn't all automated yet. Much of this code comes from the original Arviss (C-Arviss), so at the time of writing there's only one example.

You need `clang` and `llvm` (for things like `llvm-objdump` and `llvm-objcopy`).

## Building
Obviously you don't *need* Ninja.
```
$ cd arviss_cpp/examples
$ cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang-18
$ cmake --build build
```

## Extracting binary from ELF
Necessary because arviss_cpp doesn't have an ELF-loader yet.
```
$ cd arviss_cpp/examples/bin
$ llvm-objcopy hello -O binary hello.bin
```

## Running
Update `arviss_cpp` to set the path to `hello.bin`.

e.g.,
```cpp
std::ifstream fileHandler("examples/bin/hello.bin", std::ios::in | std::ios::binary | std::ios::ate);
```

You may also need to increase the number of iterations passed to `Run()`.
```cpp
Run(cpu, 100000);
```
