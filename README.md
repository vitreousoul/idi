# project

idi currently exists as a collection of lexers/parsers

The goal is to write a few parsers for programming languages or other structured text, and produce uniform syntax structures.

From there we can write synatax transformers and who knows what else.

# setup build script
## debug build
* set `DEBUG=1` in `build.sh`
## release build
* set `DEBUG=0` in `build.sh`

# build
* run `./build.sh`

# run tests
* set `DEBUG=1` in `build.sh`
* run `./test.sh`

# running in release
* set `DEBUG=0` in `build.sh`
* run `./build.sh`
* run `cd build && ./idi.exe && cd ..`

all it does now is run tests...
