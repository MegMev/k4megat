#include "TpcSamplingAlg.h"

// edm4hep
#include "SimKernel/Units.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/TPCHitCollection.h"
#include "edm4hep/TrackerHitCollection.h"

#include "boost/numeric/conversion/cast.hpp"
#include <CLHEP/Units/SystemOfUnits.h>
#include <GaudiKernel/Kernel.h>
#include <GaudiKernel/MsgStream.h>
#include <algorithm>
#include <boost/numeric/conversion/converter_policies.hpp>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <vector>

using namespace megat;

using boost::numeric_cast;
using boost::numeric::bad_numeric_cast;
using boost::numeric::negative_overflow;
using boost::numeric::positive_overflow;

DECLARE_COMPONENT( TpcSamplingAlg )

static constexpr double max_boundary = std::numeric_limits<double>::max();

TpcSamplingAlg::TpcSamplingAlg( const std::string& aName, ISvcLocator* aSvcLoc ) : GaudiAlgorithm( aName, aSvcLoc ) {
  declareProperty( "inHits", m_inHits, "Segmented Hit Collection of Tpc" );
  declareProperty( "simHits", m_simHits, "Component collection of the segmented hit" );
  declareProperty( "outHits", m_outHits, "Raw TPC Hit Collection with ADC words" );
}
TpcSamplingAlg::~TpcSamplingAlg() {}

StatusCode TpcSamplingAlg::initialize() {
  /// mandatory
  if ( GaudiAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

  /// unit conversion
  m_sampleInterval = m_sampleInterval * CLHEP::ns / edmdefault::time;
  m_shapeTime      = m_shapeTime * CLHEP::ns / edmdefault::time;
  m_gain           = m_gain * edmdefault::energy / CLHEP::keV;

  return StatusCode::SUCCESS;
}

StatusCode TpcSamplingAlg::execute() {
  /// input
  const auto in_hits  = m_inHits.get();
  const auto sim_hits = m_simHits.get();

  /// output
  auto out_hits = m_outHits.createAndPut();

  //  loop input hits
  for ( const auto& hit : *in_hits ) {

    // 0. get bined raw energy array of this cell
    auto _get_seeds = [&]() -> std::map<int, double> {
      // bin -> ADC counts
      std::map<int, double> _seeds;

      auto _hits = hit.getRawHits();
      for ( const auto _hit : _hits ) {
        if ( LIKELY( _hit.collectionID == sim_hits->getID() ) ) {
          auto sim_hit = sim_hits->at( _hit.index );
          int  bin     = std::round( sim_hit.getTime() / m_sampleInterval );
          _seeds[bin] += sim_hit.getEDep() * m_gain;
        }
      }

      return _seeds;
    };
    auto seeds = _get_seeds();

    // transfer function (from REST)
    auto _shaper = []( double t ) -> double {
      if ( t <= 0 ) return 0;
      // normalized that max output is 1.0 (t_max = 1.1664004483744728)
      return std::exp( -3. * t ) * std::pow( t, 3. ) * std::sin( t ) * 22.68112123672292;
    };

    // 1. convolution of response of all seeds
    std::vector<double> adc_counts( m_nrPts, 0 );
    for ( auto [key, seed] : seeds ) {
      // only have effect on future counts
      for ( int bin = key; bin < m_nrPts; bin++ ) {
        adc_counts[bin] += seed * _shaper( ( bin - key ) * m_sampleInterval / m_shapeTime );
      }
    }

    // 2. create output hit
    // [improve:] save seed size in quality member (temporary?)
    auto new_hit = out_hits->create( hit.getCellID(), seeds.size(), hit.getTime(), hit.getEDep() );
    std::for_each( adc_counts.begin(), adc_counts.end(), [&]( auto& count ) {
      try {
        new_hit.addToRawDataWords( numeric_cast<int16_t>( std::lround( count ) + m_ampOffset ) );

      } catch ( bad_numeric_cast& e ) {
        warning() << "ADC count out of range (16-bit), try adjusting the gain factor!" << endmsg;
        fatal() << e.what() << endmsg;
      }
    } );
  }

  return StatusCode::SUCCESS;
}

StatusCode TpcSamplingAlg::finalize() { return GaudiAlgorithm::finalize(); }
