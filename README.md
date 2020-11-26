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

**PreSelectReader**

Dedicated processor that implements `marlin::DataSourceProcessor`. It reads an event with `lcio::EventSummary` objects ( created with 
the EventSummaryProcessor ) and then only reads full events fulfilling a pre-selection cut from the same, or another LCIO file.

See example **readEvents** below for details.


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

---

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


### readEvents

Simple example to read only events fulfilling a pre-selection cut with the PreSelectReader.

```sh
cd examples/createEventSummary
```

- assuming you ran the previous examples, create links to these files:

```sh
ln -s ../createEventSummary/event_summary.slcio .
ln -s ../createMiniDST/bbudsc_3evt_miniDST.slcio .
```

- then you can run the example

```sh
Marlin preselect_read.xml
```

Here the EventSummaries are read from the file `event_summary.slcio` pointing to events in `bbudsc_3evt_miniDST.slcio`.
One can also read the EventSummaries from the same file, e.g. `bbudsc_3evt_miniDST.slcio`.



The actual pre-selection cut that is used for selecting which events to read is defined in `include/PreSelectionCut.h`

**This has to be changed by the user before re-compiling in order to be effective !!**


Simply change this section in the file:

```c++
// *******************************************************************************************
//  bool cut = ( evtnum % 2 ) ; // only read every other event
//  bool cut = ( munum == 2  ) ; // only di-muon events

// only events with u,d, and s quarks in the Monte Carlo final state
  ProcessFlag mcf( mcproc ) ;
  bool cut = ( mcf.contains( { PF::dquarks, PF::uquarks, PF::squarks }  )  ) ;


// *******************************************************************************************
```


**Note that the PreSelectReader processor only works if there is no global LCIOInputFiles parameter
specified in the steering file (see preselect_read.xml):**

```xml
 <global>
  <!-- <parameter name="LCIOInputFiles"></parameter> -->
 </global>
```

