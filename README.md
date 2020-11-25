# MiniDST
A Marlin package for creation of miniDST LCIO files with an event summary.
- status: experimentall


## Overview

Collection of processors dealing with miniDSTs and event summary files that 
can be used to read only selected events with a fast pre-selection based
on the lcio::EventSummary object.

### Processors

**EventSummaryProcessor**

Creates an `lcio::EventSummary` object for every event in the input file and adds it to an
event summary event, which is written at the end of the job, either appended to an existing 
file (e.g. the one that is produced in this job) or a standalone file.

These summary events can be used for a fast pre-selection in later analysis.

See [https://github.com/iLCSoft/LCIO/blob/master/examples/cpp/delphes2lcio/examples/higgs_recoil_plots_fast.C](https://github.com/iLCSoft/LCIO/blob/master/examples/cpp/delphes2lcio/examples/higgs_recoil_plots_fast.C) for an example ROOT script.

---

**NB: As this fast pre-selection mechanism depends on direct-access functionality in LCIO, which works only for one file at a time, it is
highly recommended to also only create EventSummary events for one LCIO file at a time**

---

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

- add library to your Marlin_DLL

```sh
export MARLIN_DLL=$MARLIN_DLL:$PWD/../lib/libMiniDST.dylib
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
cd ..

```

- now you can create a miniDST file from an existing DST file, e.g.:

```sh
Marlin miniDSTEvtSum.xml --global.LCIOInputFiles=bbudsc_3evt_DST.slcio --constant.LCIOOutputFile=bbudsc_3evt_miniDST.slcio
```

- check the result with

```sh
dumpevent bbudsc_3evt_miniDST.slcio -99 -99
```


### createEventSummary

Simple example creating a new event summary file for an existing (miniDST) LCIO file.


```sh
cd examples/createEventSummary
```

- create a standalone event summary file, e.g. for the miniDST file created in example createMiniDST:

```sh
Marlin eventSummary.xml --global.LCIOInputFiles=../createMiniDST/bbudsc_3evt_miniDST.slcio 
```

- this creates the file `event_summary.slcio `:

```sh
dumpevent event_summary.slcio -99 -99
```
