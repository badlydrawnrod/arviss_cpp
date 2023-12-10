# ARVISS (cpp)

A RISC-V instruction set simulator implemented using C++ concepts. Compare and
contrast with the trait-based [Rust implementation](https://github.dev/badlydrawnrod/).

## Building

Example:
```
$ cmake -B build
-- Configuring done
-- Generating done
-- Build files have been written to: /home/rod/projects/arviss_cpp/build

$ cmake --build build
[2/2] Linking CXX executable arviss_cpp
```

## Running

Build the examples as described in [examples/README.md](examples/README.md). This will create some example
images in `examples/images/`.

Run the examples as follows:
```
$ build/arviss_cpp
Hello, world from C!
Hello, world from C!
...
Hello
```
