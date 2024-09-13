# GlibcBt
__GlibcBt__ is a simple independent backtrace library with a completely identical interface
and external behavior to
[GNU C Library backtrace](https://www.gnu.org/software/libc/manual/html_node/Backtraces.html).

## Compatibility:
- OS: GNU/Linux;
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

## Running tests
Make sure you have a debug build and run:
```sh
ctest --test-dir build
```
