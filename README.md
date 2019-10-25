# waves-c [![Build Status](https://travis-ci.org/wavesplatform/waves-c.svg?branch=master)](https://travis-ci.org/wavesplatform/waves-c)

C library for working with Waves.

## Building

This package uses CMake version 3.9 (or newer) for building and it depends on development package of OpenSSL version 1.1 (or newer).

To build it on **Linux**, install the OpenSSL 1.1 development package (usually called something like `openssl-dev`), then run the following commands:

```
cmake .
make
```

On **macOS**, [install OpenSSL using brew](http://brewformulas.org/Openssl), then pass the OpenSSL root directory path as a CMake parameter as follows:

```
brew install openssl@1.1
cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl@1.1 .
make
```

### Packaging

By default, the CMake configuration enables all supported CPack generators. Check out the `CPACK_GENERATOR` option for the list of supported CPack generators (see `src/CMakeLists.txt`). CPack generator can be overridden as follows:
```
cmake -DCPACK_GENERATOR=RPM .
```


## Installing

If there is no suitable CPack generator in the `CPACK_GENERATOR` list, then the only option is running `make install`.

## Quick Start

For a quick start, we took the [bcdev's waves_vanity source code](https://github.com/bcdev-/waves_vanity) as a basis. Thanks to bcdev for providing that!
