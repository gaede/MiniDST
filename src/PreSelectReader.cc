#include "PreSelectReader.h"

#include "marlin/ProcessorMgr.h"

//--- workaround for duplicate symbol in LCIO : operator<< needs to be inline 
#include "ProcessFlag.h"

#include "lcio.h"
#include "IO/LCReader.h"
#include "IOIMPL/LCFactory.h"
#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "EVENT/ReconstructedParticle.h"
#include "EVENT/LCEvent.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/LCIterator.h"
#include "UTIL/Operators.h"
#include "UTIL/EventSummary.h"

//---- definition of the user define pre-selection:
#include "PreSelectionCut.h"
//-----------------------------------------------

#include <sstream>


PreSelectReader aPreSelectReader ;


PreSelectReader::PreSelectReader() : marlin::DataSourceProcessor("PreSelectReader") {

  _description = "Reads an EventSummary event from an LCIO file and then reads only events fulfilling the pre-selection cut"
    " from the same, or optionally different, LCIO file."
    " Make sure to not specify any LCIOInputFiles in the steering in order to use this" ;

  registerProcessorParameter( "EventSummaryFileName" , 
			      "file name with event summary event (run=-99, evt=-99)"  ,
			      _evtSumName ,
			      std::string("bbudsc_3evt_miniDST.slcio") ) ;
    
  registerOptionalParameter( "EventFileName" , 
			     "optional lcio file with events if different from the one specified in ${EventSummaryFileName}"  ,
			     _lcioFileName ,
			     std::string("bbudsc_3evt_miniDST.slcio") ) ;
}

PreSelectReader*  PreSelectReader::newProcessor() { 
  return new PreSelectReader ;
}

void PreSelectReader::init() {    
  printParameters() ;    
}

void PreSelectReader::readDataSource( int numEvents ) {

  IO::LCReader* lcReader = IOIMPL::LCFactory::getInstance()->createLCReader(IO::LCReader::directAccess) ;
  lcReader->open( _evtSumName ) ;


  EVENT::LCEvent* evt ;
  
  //==================== read event summary  ============================================================

  evt = lcReader->readEvent( -99, -99 ) ; //  evt containing the event summaries

  if(! evt){
    std::stringstream err;
    err << "[PreSelectReader]: no event -99 run -99 with EventSummaries collection found in file: "  <<  _evtSumName ;
    throw std::runtime_error( err.str() ) ;
  }

  // take the collection from the event and keep it for the job
  auto escol = evt->takeCollection("EventSummaries") ;
 

  lcReader->close() ;

  // create a new reader - needed for direct access after close
  IO::LCReader* lcReaderEvt = IOIMPL::LCFactory::getInstance()->createLCReader(IO::LCReader::directAccess) ;
 

  std::string evtFile ;

  if( parameterSet( "EventFileName" ) ){  // open different file with real events

    lcReaderEvt->open( _lcioFileName ) ;
    
    streamlog_out( DEBUG ) << " opening file " << _lcioFileName << " for reading events "  << std::endl ;

    evtFile = _lcioFileName ;

  } else {

   lcReaderEvt->open( _evtSumName ) ;

    streamlog_out( DEBUG ) << " reopening file " << _evtSumName << " for reading events "  << std::endl ;

    evtFile = _evtSumName ;
  }

  
  //=== minimal consistency check ===============================

  int nEventTotal = lcReaderEvt->getNumberOfEvents() ;

  if( nEventTotal != escol->getNumberOfElements() ){

    std::stringstream err;
    err << "[PreSelectReader]: ERROR file " <<  evtFile  << " contains  " << nEventTotal << " events but "
	<< "EventSummaries collection has : " << escol->getNumberOfElements() << " elements " << std::endl;
    throw std::runtime_error( err.str() ) ;
  }



  //==================== now the event loop ============================================================
  
  int nReadEvt = std::min( nEventTotal, numEvents ) ;

  for(int i=0 ; i<nReadEvt ; ++i){

    EventSummary es( escol->getElementAt(i)  ) ;

    streamlog_out( DEBUG ) << " event summary : \n" << UTIL::header( &es , escol ) << UTIL::lcio_short<EVENT::LCGenericObject>(&es) << std::endl ;

    
    // read only events fulfilling the pre-cut defined in PreSelectionCut.h
    if( preselection_cut( es ) ) {

      ++_nEvt ;

      streamlog_out( DEBUG ) << " after pre-selection, reading event " << es.getI( ESI::evtnum ) << " from run " << es.getI( ESI::runnum ) << " for processing ! " << std::endl ;
      
      evt = lcReaderEvt->readEvent( es.getI( ESI::runnum ), es.getI( ESI::evtnum ) ) ;

      if( evt == nullptr )
	break ;

      marlin::ProcessorMgr::instance()->processEvent( evt ) ;
    }
  }
}

void PreSelectReader::end() {
  streamlog_out( MESSAGE ) << " read " << _nEvt << " fulfilling pre-selection cut " << std::endl ;
}

