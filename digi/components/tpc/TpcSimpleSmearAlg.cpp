#include "TpcSimpleSmearAlg.h"

// Gaudi
#include "GaudiKernel/IRndmEngine.h"
#include <cmath>

// edm4hep
#include "SimKernel/Units.h"
#include "edm4hep/TrackerHitCollection.h"

using namespace megat;

DECLARE_COMPONENT( TpcSimpleSmearAlg )

TpcSimpleSmearAlg::TpcSimpleSmearAlg( const std::string& aName, ISvcLocator* aSvcLoc )
    : GaudiAlgorithm( aName, aSvcLoc ) {
  declareProperty( "inHits", m_inHits, "Segmented Hit Collection of Tpc" );
  declareProperty( "outHits", m_outHits, "Smeared Electron Hit Collection" );
}
TpcSimpleSmearAlg::~TpcSimpleSmearAlg() {}

StatusCode TpcSimpleSmearAlg::initialize() {
  /// mandatory
  if ( GaudiAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

  /// unit conversion
  m_sigmaE = m_sigmaE * CLHEP::eV / edmdefault::energy;
  m_sigmaT = m_sigmaT * CLHEP::ps / edmdefault::time;

  /// Random number generators
  if ( m_sigmaT ) {
    auto sc = m_sigmaGenT.initialize( randSvc(), Rndm::Gauss( 0.0, m_sigmaT ) );
    if ( !sc.isSuccess() ) { return StatusCode::FAILURE; }
  }

  if ( !use_Fano && m_sigmaE ) {
    auto sc = m_sigmaGenE.initialize( randSvc(), Rndm::Gauss( 0.0, m_sigmaE ) );
    if ( !sc.isSuccess() ) { return StatusCode::FAILURE; }
  }

  return StatusCode::SUCCESS;
}

StatusCode TpcSimpleSmearAlg::execute() {
  /// input
  const auto in_hits = m_inHits.get();

  /// output
  auto out_hits = m_outHits.createAndPut();

  //  loop input hits
  for ( const auto& hit : *in_hits ) {
    auto new_hit = hit.clone();

    // smearing time
    if ( m_sigmaT ) { new_hit.setTime( m_sigmaGenT() + hit.getTime() ); }

    // smearing energy
    auto old_e = hit.getEDep();
    if ( use_Fano ) {
      // convert to eV for correct behaviour
      old_e = old_e * edmdefault::energy / CLHEP::eV;
      Rndm::Numbers _gaus_gen( randSvc(), Rndm::Gauss( 0., std::sqrt( m_fano * old_e ) ) );
      new_hit.setEDep( ( _gaus_gen() + old_e ) * CLHEP::eV / edmdefault::energy );
    } else if ( m_sigmaE ) {
      new_hit.setEDep( m_sigmaGenE() + old_e );
    }

    // add new hit to output collection
    out_hits->push_back( new_hit );
  }

  return StatusCode::SUCCESS;
}

StatusCode TpcSimpleSmearAlg::finalize() { return GaudiAlgorithm::finalize(); }
