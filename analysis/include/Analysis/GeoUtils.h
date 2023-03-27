#ifndef MEGAT_GeoUitls_H
#define MEGAT_GeoUitls_H

#include "Analysis/IdConverter.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "TGeoManager.h"
#include <ROOT/RVec.hxx>
#include <string>

namespace megat {
  namespace utility {

    // customize log level of dd4hep and TGeo
    inline void setGeomLogLevel( dd4hep::PrintLevel level ) {
      dd4hep::setPrintLevel( level );
      if ( level == dd4hep::NOLOG || level == dd4hep::WARNING || level == dd4hep::ERROR || level == dd4hep::FATAL ) {
        TGeoManager::SetVerboseLevel( 0 );
      }
    }

    // geometry loader
    dd4hep::Detector& loadGeometry( std::vector<std::string> xmlList, std::string tpc_readout_name = "",
                                    std::string tag = "", std::string tpc_name = "TPC",
                                    dd4hep::PrintLevel level = dd4hep::ERROR );

    // return cell positions as RVec<RVecD>
    template <typename HitData>
    class CellPosition {
    public:
      CellPosition( std::string tag_name = "default" ) : idConv( tag_name ) {}

      auto operator()( const ROOT::VecOps::RVec<HitData>& in ) {
        auto mapper = [this]( const HitData& hit ) {
          auto pos = idConv.position( hit.cellID );
          return ROOT::RVecD{ pos.x, pos.y, pos.z };
        };
        return Map( in, mapper );
      }

    private:
      megat::utility::IdConverter idConv;
    };
  } // namespace utility
} // namespace megat
#endif
