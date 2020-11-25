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


## Examples

### createMiniDST

Simple example creating a miniDST file with EventSummary event at the end.

Prerequisites:
- copy some files needed for this from ILDConfig

```sh
cd examples/createMiniDST

export ILDConfig=/usr/local/ilcsoft/HEAD/ILDConfig/HEAD  # change path as needed

cp $ILDConfig/LCFIPlusConfig/vtxprob/d0probv2_ildl5_6q500.root .
cp $ILDConfig/LCFIPlusConfig/vtxprob/z0probv2_ildl5_6q500.root .

mkdir lcfiweights
cd lcfiweights
tar zxvf $ILDConfig/LCFIPlusConfig/lcfiweights/6q500_v04_p00_ildl5.tar.gz
cd -
```
