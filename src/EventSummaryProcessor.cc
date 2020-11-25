#include "EventSummaryProcessor.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h" 
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCIterator.h"
#include "UTIL/Operators.h"
#include "UTIL/EventSummary.h"
#include "UTIL/ProcessFlag.h"

#include <cmath>

using namespace marlin ;


namespace {
  /// helper enum for indices of collection names
  enum {
    mcpName = 0,
    pfoName,
    electronName,
    muonName,
    tauName,
    photonName,
    colNameSize 
  } ;

  /// helper function to get collections - returns nullptr if not in event
  EVENT::LCCollection* getCollection( EVENT::LCEvent* evt, const std::string& name){

    try{

      return evt->getCollection( name ) ;

    } catch( lcio::DataNotAvailableException ){

      streamlog_out( DEBUG ) << " collection " << name << " not found in event "
			     << evt->getEventNumber() << " , " << evt->getRunNumber() << std::endl ;
    }

    return nullptr ;
  }
}


EventSummaryProcessor aEventSummaryProcessor ;


EventSummaryProcessor::EventSummaryProcessor() : Processor("EventSummaryProcessor") {

  // modify processor description
  _description = "Creates a collection of EventSummary objects for every event and writes the summary"
    " event to the output file - either a fresh file or appended to an existing file." ;

  _colNames.resize( ::colNameSize ) ;
    
  registerProcessorParameter( "LCIOOutputFile",
			      "Name of the output LCIO file",
			      _outFile,
			      std::string("event_summary.slcio") );

  registerProcessorParameter( "AppendOutputFile",
			      "Append the summary event to the given LCIO file "
			      " - if false a new file is created. Default is true.",
			      _useAppend,
			      bool(true) );

  // register steering parameters: name, description, class-variable, default value
  registerInputCollection( LCIO::MCPARTICLE,
			   "MCParticleCollection" , 
			   "Name of the MCParticle collection"  ,
			   _colNames[ mcpName ],
			   std::string("MCParticlesSkimmed") );

  registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
			   "PFOCollection" , 
			   "Name of the particle flow object collection"  ,
			   _colNames[ pfoName ],
			   std::string("PandoraPFOs") );

  registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
			   "MuonCollection" , 
			   "Name of the isolated muon collection"  ,
			   _colNames[ muonName ],
			   std::string("IsolatedMuons") );

  registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
			   "TauCollection" , 
			   "Name of the isolated tau collection"  ,
			   _colNames[ tauName ],
			   std::string("IsolatedTaus") );

  registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
			   "ElectronCollection" , 
			   "Name of the isolated electron collection"  ,
			   _colNames[ electronName ],
			   std::string("IsolatedElectrons") );

  registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
			   "PhotonCollection" , 
			   "Name of the isolated photon collection"  ,
			   _colNames[ photonName ],
			   std::string("IsolatedPhotons") );

}



void EventSummaryProcessor::init() { 

  streamlog_out(DEBUG) << "   init called  " << std::endl ;

  // usually a good idea to
  printParameters() ;

//-- initialize the summary event and collection
    
  _sumEvt = new lcio::LCEventImpl ;
  _sumEvt->setRunNumber( -99 ) ;
  _sumEvt->setEventNumber( -99 ) ;
  _sumEvt->setWeight( 0. ) ;
  _sumCol = new lcio::LCCollectionVec( lcio::LCIO::LCGENERICOBJECT ) ;
  _sumEvt->addCollection( _sumCol, "EventSummaries" ) ;


  _nRun = 0 ;
  _nEvt = 0 ;

}


void EventSummaryProcessor::processRunHeader( LCRunHeader* ) { 

  _nRun++ ;
} 



void EventSummaryProcessor::processEvent( LCEvent * evt ) { 

  using namespace UTIL;
  

  if( isFirstEvent() ) {}

  
  // get the collections from the event

  auto * mcps      = ::getCollection( evt, _colNames[ ::mcpName ] ) ;
  auto * pfos      = ::getCollection( evt, _colNames[ ::pfoName ] ) ;
  auto * electrons = ::getCollection( evt, _colNames[ ::electronName ] ) ;
  auto * muons     = ::getCollection( evt, _colNames[ ::muonName ] ) ;
  auto * taus      = ::getCollection( evt, _colNames[ ::tauName ] ) ;
  auto * photons   = ::getCollection( evt, _colNames[ ::photonName ] ) ;



  //---------- fill event summary ----
  
    
  auto evts = new EventSummary ;
  _sumCol->addElement( evts ) ;

  evts->setI( ESI::runnum, evt->getRunNumber() ) ;
  evts->setI( ESI::evtnum, evt->getEventNumber() ) ;

  if( electrons) evts->setI( ESI::elnum,  electrons->getNumberOfElements() );
  if( muons)     evts->setI( ESI::munum,  muons->getNumberOfElements() );
  if( taus)      evts->setI( ESI::taunum, taus->getNumberOfElements() );
  if( photons)   evts->setI( ESI::phonum, photons->getNumberOfElements() );


  // only meaningful if only one jet collection ...
  //evts->setI( ESI::jetnum, jets->getNumberOfElements()  );


  //to do: which collection should we take these from,
  //  then need parse PID parameters ...
  // if( yflipCol != nullptr ){

  //   EVENT::FloatVec yflip ;
  //   yflipCol->getParameters().getFloatVals("ExclYflip12_78", yflip ) ;

  //   if( yflip.size() == 7 ){

  // 	evts->setF( ESF::y12, yflip[0] ) ;
  // 	evts->setF( ESF::y23, yflip[1] ) ;
  // 	evts->setF( ESF::y34, yflip[2] ) ;
  // 	evts->setF( ESF::y45, yflip[3] ) ;
  // 	evts->setF( ESF::y56, yflip[4] ) ;
  // 	evts->setF( ESF::y67, yflip[5] ) ;
  // 	evts->setF( ESF::y78, yflip[6] ) ;
  //   }
  // }


  double epfoTot(0), pfopx(0), pfopy(0), pfopz(0)  ;
  unsigned chPFONum(0), neuPFONum(0) ;

  if( pfos ){
    // total reconstructed energy

    lcio::LCIterator<lcio::ReconstructedParticle> pfoIT( pfos ) ;

    while( auto p = pfoIT.next() ){

      epfoTot += p->getEnergy() ;
      pfopx += p->getMomentum()[0] ;
      pfopy += p->getMomentum()[1] ;
      pfopz += p->getMomentum()[2] ;

      if( std::fabs( p->getCharge() ) > 0.1 )
	++chPFONum ;
      else
	++neuPFONum ;
    }

    evts->setI( ESI::chpfonum,  chPFONum );
    evts->setI( ESI::neupfonum, neuPFONum );

    evts->setF( ESF::epfotot, epfoTot ) ;
  }


  double emcpVis = 0;
  double emcpTot(0), mcppx(0), mcppy(0), mcppz(0)  ;

  if( mcps ){
    // total true and visible energy

    lcio::LCIterator<lcio::MCParticle> mcpIT( mcps ) ;

    while( auto mcp = mcpIT.next() ){

      // count only stable Particles
      if( mcp->getGeneratorStatus() != 1 ) continue ;

      emcpTot += mcp->getEnergy() ;
      mcppx   += mcp->getMomentum()[0] ;
      mcppy   += mcp->getMomentum()[1] ;
      mcppz   += mcp->getMomentum()[2] ;

      int pdg = abs( mcp->getPDG() ) ;

      // exclude neutrinos
      if( pdg == 12 ) continue ;
      if( pdg == 14 ) continue ;
      if( pdg == 16 ) continue ;

      emcpVis += mcp->getEnergy() ;
    }

    evts->setF( ESF::emcptot, emcpVis ) ;

#if LCIO_VERSION_GE( 2 , 15 )

    const int maxParticles = 10 ; // same as default, defined in ProcessFlag.h

    int nMCPCheck = maxParticles > mcps->getNumberOfElements() ? mcps->getNumberOfElements()  : maxParticles ;
    ProcessFlag pFlag = decodeMCTruthProcess( mcps, nMCPCheck ) ;

    evts->setI( ESI::mcproc,  pFlag ) ;

    streamlog_out( DEBUG2 ) << " ---- mc truth process : " << pFlag << std::endl ;
#endif

  }

  if( pfos && mcps ){
    // missing 4-momentum
    evts->setF( ESF::emiss,  emcpTot - epfoTot ) ;
    evts->setF( ESF::pxmiss, mcppx - pfopx ) ;
    evts->setF( ESF::pymiss, mcppy - pfopy ) ;
    evts->setF( ESF::pzmiss, mcppz - pfopz ) ;
  }

    
  //-------------------------------
  streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber() 
		       << "   in run:  " << evt->getRunNumber() << std::endl ;

  _nEvt ++ ;
}



void EventSummaryProcessor::check( LCEvent*  ) { 
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void EventSummaryProcessor::end(){ 



  auto* writer = lcio::LCFactory::getInstance()->createLCWriter() ;

  if( _useAppend )
    writer->open( _outFile , lcio::LCIO::WRITE_APPEND ) ; // append to existing file
  else
    writer->open( _outFile ) ; // create new file - throws if file allready exists
    

  writer->writeEvent( _sumEvt ) ;

  writer->close() ;

  delete _sumEvt ;

  delete writer ;


  streamlog_out(DEBUG) << "EventSummaryProcessor::end()  " << name() 
		       << " processed " << _nEvt << " events in " << _nRun << " runs "
		       << " - summary written to " << _outFile  
		       << std::endl ;
}

