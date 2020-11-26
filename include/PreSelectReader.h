#ifndef PreSelectReader_h
#define PreSelectReader_h 1

#include "marlin/DataSourceProcessor.h"

  
/** Reads an EventSummary event from an LCIO file and then reads only events 
 *  fullfilling the pre-selection cut from the same, or optionally different,
 *  LCIO file.
 *  Has to be the first active processor
 *  and requires that no LCIO input collection is used (parameter LCIOInputFiles).
 *
 * @param EventSummaryFileName file name with event summary event (run=-99, evt=-99)
 * @param EventFileName  optional: lcio file with events if different from the one specified in ${EventSummaryFileName}"
 * 
 * @author F. Gaede, DESY
 * @date Nov 2020
 */
  
class PreSelectReader : public marlin::DataSourceProcessor {
    
public:

  PreSelectReader() ;

  virtual PreSelectReader*  newProcessor() ;


  /** Creates events with MCParticle collections from the StdHep input file and
   *  calls all active processors' processEvent() and processRunHeader Method.
   *
   */
  virtual void readDataSource( int numEvents ) ;
    
    
  virtual void init() ;
  virtual void end() ;
    
protected:
    
  std::string _evtSumName={} ;
  std::string _lcioFileName={} ;
  int _nEvt=0 ;
};
 

#endif
