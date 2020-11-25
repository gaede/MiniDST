# MiniDST
A Marlin package for creation of miniDST LCIO files with an event summary.
- status: experimentall



## Quick steps to build MiniDST

```sh
# initialize the iLCSoft environment:
. /path/to/ilcsoft/installation/vXX-YY/init_ilcsoft.sh

# build the package
mkdir build
cd build
cmake -C $ILCSOFT/ILCSoft.cmake ..
make install # this creates the library: ../lib/libMiniDST.so(dylib)
```


