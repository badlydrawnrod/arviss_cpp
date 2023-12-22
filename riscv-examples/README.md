# Building and running the examples

This is what I'm currently doing. It isn't all automated yet.

Much of this code comes from the original Arviss (C-Arviss), so at the time of writing there's only one example.

There are many sources online that will have you jump through hoops to install a `gcc` toolchain to target
RISC-V, but in my experience `clang` works well and is far less hassle to install.

## Building
### Linux
You'll need to install `cmake`, `clang` and `llvm` (for things like `llvm-objdump` and `llvm-objcopy`).
The examples use `Ninja` too, but you don't *need* it.

```
$ cd arviss_cpp/examples
$ cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang-18
$ cmake --build build
```

### Windows
You'll need to install `cmake` (it comes with the VS tools, or you can install it with `winget`) and
`clang`. The examples use `Ninja` too, but you don't *need* it.

To install `clang`, run `winget install -i LLVM.LLVM` (the `-i` flag tells it to run interactively so
that it prompts you to add LLVM to the PATH) then restart your shell so that the path changes come into effect.

```
C:> cd arviss_cpp\examples
C:> cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang
C:> cmake --build build
```

## Running
In both cases (Linux and Windows), building the examples populates the `arviss_cpp/examples/bin` directory
with two files:
- `hello` can be loaded by an Arviss CPU that has an ELF loader
- `hello.bin` is a raw image extracted from `hello` using `llvm-objcopy`. It can be loaded by an Arviss VM
  that does not know how to read ELF files.

Update `arviss_cpp` to set the path to `hello.bin`.

e.g.,
```cpp
std::ifstream fileHandler("examples/bin/hello.bin", std::ios::in | std::ios::binary | std::ios::ate);
```

You may also need to increase the number of iterations passed to `Run()`.
```cpp
Run(cpu, 100000);
```

Finally, build and run the the top-level `arviss` project.
