# Disruptor-cpp

[![Build status](https://travis-ci.org/Abc-Arbitrage/Disruptor-cpp.svg?branch=master)](https://travis-ci.org/Abc-Arbitrage/Disruptor-cpp)

## Overview

Disruptor-cpp is a fully functional C++ port of the [LMAX disruptor](https://lmax-exchange.github.io/disruptor/). 

## Building

### Linux

[Boost](http://www.boost.org/) must be available on your machine.  You can install it using your favorite package manager or build it on your own.
If boost has been installed into standard system locations the following commands will start the build:   

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=release
make
```

If [Boost](http://www.boost.org/) has been installed into a custom location you will probably need to specify **BOOST_ROOT** variable. Please refer to the [Find boost](https://cmake.org/cmake/help/v3.0/module/FindBoost.html) documentation for details.

Optionally you may want to compile and run the unit tests and benchmarks. The tests compilation is activated by means of **DISRUPTOR_BUILD_TESTS** flag: 

```sh
cmake .. -DCMAKE_BUILD_TYPE=release -DDISRUPTOR_BUILD_TESTS=true
```

### Windows 

The simplest way to compile the library on Windows is to use the provided Visual Studio solution files:
* Disruptor-lib.sln - the solution only includes the library.
* Disruptor-all.sln - the solution includes the library, benchmarks and unit tests.

[Boost](http://www.boost.org/)  must be available on your machine. The  [**boost.props**](https://github.com/Abc-Arbitrage/Disruptor-cpp/blob/master/boost.props) file already included into the solution but you may need to modify the headers and libraries directory according to your boost location and folder structure.

