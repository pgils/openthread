# Project Domotica - node

## Toolchain

Install build dependencies:
 - [GNU toolchain for ARM Cortex-M][gnu-toolchain].
 - automake
 - g++
 - libtool
 - make  
For `make pretty`:
 - clang-format

[gnu-toolchain]: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads

## Build

```bash
$ cd <path-to-openthread>
$ ./bootstrap
$ ./build.sh
```

## Flash firmware

Install [nrfutil](https://github.com/NordicSemiconductor/pc-nrfutil)

```bash
$ cd <path-to-openthread>
$ ./flash.sh </path/to/zipfile>
```
