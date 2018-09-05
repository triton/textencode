# textencode

textencode is a library for base-n and nix string encoding as well as
a CLI to perform the translations.

## Building
For a standard release build, you want something like:
```
./bootstrap.sh
./configure --disable-tests
make
make install
```

For a test / debug build, a typical configuration is
```
./bootstrap.sh
./configure --enable-tests --enable-coverage --enable-valgrind
make
make check
make check-valgrind
make check-code-coverage
```
