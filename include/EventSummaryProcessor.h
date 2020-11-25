#ifndef EventSummaryProcessor_h
#define EventSummaryProcessor_h 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <vector>


// lcio fwd declarations
namespace IMPL {
  class LCEventImpl ;
  class LCCollectionVec ;
}


using namespace marlin ;


/**  EventSummaryProcessor creates a collection of EventSummary objects for every event and writes 
 *   the summary event to the output file - either a fresh file or appended to an existing file.
 * 
 * @param LCIOOutputFile     name the output LCIO file
 * @param AppendOutputFile   append to the LCIO output file - if false a new file is created. Default: true.
 * @param MCParticleCollection name of the MCParticle collection
 * @param ElectronCollection name of the isolated electron collection
 * @param MuonCollection     name of the isolated muon collectio
 * @param TauCollection      name of the isolated tau collection
 * @param PhotonCollection   name of the isolated photon collection
 * @param PFOCollection      name of the particle flow object collection
 * @param Verbosity          verbosity level of this processor ("DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT"
 * 
 * @author F. Gaede, DESY
 * @date Nov 2020
 */

class EventSummaryProcessor : public Processor {
  
public:
  
  virtual Processor*  newProcessor() { return new EventSummaryProcessor ; }
  
  
  EventSummaryProcessor() ;
  
  /** Called at the begin of the job before anything is read.
   *  Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;
  
  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ; 
  
  
  virtual void check( LCEvent * evt ) ; 
  
  
  /** Called after data processing for clean up.
   */
  virtual void end() ;
  
  
protected:

  /// Input collection names.
  std::vector<std::string> _colNames{} ;

  /// Output LCIO file name
  std::string _outFile ;

  /// use append mode for output file
  bool _useAppend = true ;
  
  LCEventImpl* _sumEvt ;
  LCCollectionVec* _sumCol ;

  int _nRun{} ;
  int _nEvt{} ;
} ;

#endif



