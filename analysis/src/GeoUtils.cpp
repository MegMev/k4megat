#include "Analysis/GeoUtils.h"
#include "Analysis/SimpleLogger.h"
#include <stdexcept>

namespace megat {
  namespace utility {

    /**
     * @brief load and register geometry under different tags
     * @details different tags match different segmentation of TPC [todo: better solution]
     * @param[in] xmlList list of compact files
     * @param[in] tpc_readout_name specifiy readout name of TPC
     * @param[in] tag tag name of the loaded geometry
     * @param[in] tpc_name TPC detector name
     * @return the loaded detector description
     */
    dd4hep::Detector& loadGeometry( std::vector<std::string> xmlList, std::string tpc_readout_name, std::string tag,
                                    std::string tpc_name ) {
      using namespace dd4hep;

      if ( tpc_readout_name.empty() ) {
        if ( !tag.empty() ) LOG_WARN( "No tag for geometry when using default TPC readout" );

        tag = "default";
      } else if ( tag.empty() ) {
        tag = tpc_readout_name;
      }

      auto& detector = Detector::getInstance( tag );
      if ( detector.state() != Detector::State::NOT_READY ) {
        throw std::runtime_error( "Do not load the detector geometry repeatedly (or with same tag)" );
      }

      for ( auto xml : xmlList ) { detector.fromCompact( xml, DetectorBuildType::BUILD_RECO ); }
      if ( !tpc_readout_name.empty() ) {
        auto tpc_sd      = detector.sensitiveDetector( tpc_name );
        auto tpc_readout = detector.readout( tpc_readout_name );
        tpc_sd.setReadout( tpc_readout );
      }
      return detector;
    }

  } // namespace utility
} // namespace megat
