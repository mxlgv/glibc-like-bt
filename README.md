# GlibcBt
__GlibcBt__ is a simple independent backtrace library with a completely identical interface
and external behavior to
[GNU C Library backtrace](https://www.gnu.org/software/libc/manual/html_node/Backtraces.html).

## Compatibility
- OS: GNU/Linux, Windows;
- Arch: x86, x86_64.

## Building
For debugging and running tests
```sh
cmake -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build
```

For release recommended:
```sh
cmake -DCMAKE_BUILD_TYPE=MinSizeRel -B build
cmake --build build
```

When cross-compiling, you must specify the object file format for `nasm`. \
For example:
```sh
cmake -DCMAKE_ASM_NASM_OBJECT_FORMAT=coff -B build
```

## Running tests
Make sure you have a debug build and run:
```sh
ctest --test-dir build
```
