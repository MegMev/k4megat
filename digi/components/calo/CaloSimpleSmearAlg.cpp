#include "CaloSimpleSmearAlg.h"

// Gaudi
#include "GaudiKernel/IRndmEngine.h"
#include <GaudiKernel/MsgStream.h>
#include <cmath>

// edm4hep
#include "SimKernel/Units.h"
#include "edm4hep/SimCalorimeterHitCollection.h"

using namespace megat;

DECLARE_COMPONENT( CaloSimpleSmearAlg )

CaloSimpleSmearAlg::CaloSimpleSmearAlg( const std::string& aName, ISvcLocator* aSvcLoc )
    : GaudiAlgorithm( aName, aSvcLoc ) {
  declareProperty( "inHits", m_inHits, "Segmented Hit Collection of Calorimeter" );
  declareProperty( "outHits", m_outHits, "Smeared Hit Collection" );
}
CaloSimpleSmearAlg::~CaloSimpleSmearAlg() {}

StatusCode CaloSimpleSmearAlg::initialize() {
  /// mandatory
  if ( GaudiAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

  // unit conversion
  m_sigmaE = m_sigmaE * CLHEP::keV / edmdefault::energy;

  auto sc = m_sigmaGenE.initialize( randSvc(), Rndm::Gauss( 0.0, m_sigmaE ) );
  if ( !sc.isSuccess() ) { return StatusCode::FAILURE; }

  return StatusCode::SUCCESS;
}

StatusCode CaloSimpleSmearAlg::execute() {
  /// input
  const auto in_hits = m_inHits.get();

  /// output
  auto out_hits = m_outHits.createAndPut();

  //  loop input hits
  for ( const auto& hit : *in_hits ) {
    auto new_hit = hit.clone();

    // smearing energy
    auto old_e = hit.getEnergy();
    new_hit.setEnergy( m_sigmaGenE() + old_e );

    // add new hit to output collection
    out_hits->push_back( new_hit );
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloSimpleSmearAlg::finalize() { return GaudiAlgorithm::finalize(); }
