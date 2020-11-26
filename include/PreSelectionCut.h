#ifndef PresSelectionCut_h
#define PresSelectionCut_h

#include "UTIL/EventSummary.h"


/** User defined pre-selection cut based 
 *  on lcio::EventSummary used in PreSelectReader.
 *  Modify as needed...
 * 
 * @author F. Gaede, DESY
 * @date Nov 2020
 */

inline bool preselection_cut( lcio::EventSummary& es ){

  using namespace UTIL ;

  // --- uncomment as needed  ------------------------------------------------
  //
  // int runnum     = es.getI( ESI::runnum    );  ///< run   number
  // int evtnum     = es.getI( ESI::evtnum    );  ///< event number
  // int chpfonum   = es.getI( ESI::chpfonum  );  ///< number of charged PFOs
  // int neupfonum  = es.getI( ESI::neupfonum );  ///< number of neutral PFOs
  // int elnum      = es.getI( ESI::elnum     );  ///< number of isloted electrons
  // int munum      = es.getI( ESI::munum     );  ///< number of isloted muons
  // int taunum     = es.getI( ESI::taunum    );  ///< number of isloted taus
  // int phonum     = es.getI( ESI::phonum    );  ///< number of isloted photons
  // int jetnum     = es.getI( ESI::jetnum    );  ///< number of jets - if only one jet collection, -1 else
  // int bcpfonum   = es.getI( ESI::bcpfonum  );  ///< number of PFOs in the BeamCal
  // int trknum     = es.getI( ESI::trknum    );  ///< number of tracks - if present
  // int clunum     = es.getI( ESI::clunum    );  ///< number of clusters - if present
  int mcproc     = es.getI( ESI::mcproc    );  ///< generated Monte Carlo ProcessFlag

  // float epfotot   = es.getF( ESF::epfotot ); ///< total reconstructed energy
  // float emcptot   = es.getF( ESF::emcptot ); ///< total visible true energy (exluding neutrinos)
  // float thrust    = es.getF( ESF::thrust  ); ///< thrust value of the event
  // float spher     = es.getF( ESF::spher   ); ///< spericity value of the event
  // float emiss     = es.getF( ESF::emiss   ); ///< missing energy
  // float pxmiss    = es.getF( ESF::pxmiss  ); ///< missing momentum in x
  // float pymiss    = es.getF( ESF::pymiss  ); ///< missing momentum in y
  // float pzmiss    = es.getF( ESF::pzmiss  ); ///< missing momentum in z
  // float y12       = es.getF( ESF::y12     ); ///< y12 flip value for exclusive jet clustering
  // float y23       = es.getF( ESF::y23     ); ///< y23 flip value for exclusive jet clustering
  // float y34       = es.getF( ESF::y34     ); ///< y34 flip value for exclusive jet clustering
  // float y45       = es.getF( ESF::y45     ); ///< y45 flip value for exclusive jet clustering
  // float y56       = es.getF( ESF::y56     ); ///< y56 flip value for exclusive jet clustering
  // float y67       = es.getF( ESF::y67     ); ///< y67 flip value for exclusive jet clustering
  // float y78       = es.getF( ESF::y78     ); ///< y78 flip value for exclusive jet clustering

// *******************************************************************************************
  
//  bool cut = ( evtnum % 2 ) ; // only read every other event
//  bool cut = ( munum == 2  ) ; // only di-muon events

// only events with u,d, and s quarks in the Monte Carlo final state
  ProcessFlag mcf( mcproc ) ;
  bool cut = ( mcf.contains( { PF::dquarks, PF::uquarks, PF::squarks }  )  ) ; 


// *******************************************************************************************

  return cut ;
}



// ====== bits in Monte Carlo ProcessFlag  =================
//    if( flag.has( PF::electrons ) ) os << "electrons|" ;
//    if( flag.has( PF::muons     ) ) os << "muons|" ;
//    if( flag.has( PF::taus      ) ) os << "taus|" ;
//    if( flag.has( PF::neutrinos ) ) os << "neutrinos|" ;
//    if( flag.has( PF::photons   ) ) os << "photons|" ;
//    if( flag.has( PF::dquarks   ) ) os << "dquarks|" ;
//    if( flag.has( PF::uquarks   ) ) os << "uquarks|" ;
//    if( flag.has( PF::squarks   ) ) os << "squarks|" ;
//    if( flag.has( PF::cquarks   ) ) os << "cquarks|" ;
//    if( flag.has( PF::bquarks   ) ) os << "bquarks|" ;
//    if( flag.has( PF::higgsbb     ) ) os << "higgsbb|" ;
//    if( flag.has( PF::higgsdu     ) ) os << "higgsdu|" ;
//    if( flag.has( PF::higgsss     ) ) os << "higgsss|" ;
//    if( flag.has( PF::higgscc     ) ) os << "higgscc|" ;
//    if( flag.has( PF::higgstautau ) ) os << "higgstautau|" ;
//    if( flag.has( PF::higgsmumu   ) ) os << "higgsmumu|" ;
//    if( flag.has( PF::higgsWW     ) ) os << "higgsWW|" ;
//    if( flag.has( PF::higgsZZ     ) ) os << "higgsZZ|" ;
//    if( flag.has( PF::higgsgaga   ) ) os << "higgsgaga|" ;
//    if( flag.has( PF::higgsgaZ    ) ) os << "higgsgaZ|" ;
//    if( flag.has( PF::higgsinv    ) ) os << "higgsinv|" ;
//    if( flag.has( PF::higgsother  ) ) os << "higgsother|" ;
//    if( flag.has( PF::exotic      ) ) os << "exotic|" ;
//    if( flag.has( PF::unknown     ) ) os << "unknown|" ;

#endif
