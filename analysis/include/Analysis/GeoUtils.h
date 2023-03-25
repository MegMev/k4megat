#ifndef MEGAT_GeoUitls_H
#define MEGAT_GeoUitls_H

#include "Analysis/IdConverter.h"
#include "DD4hep/Detector.h"
#include <ROOT/RVec.hxx>
#include <string>

namespace megat {
  namespace utility {

    // geometry loader
    dd4hep::Detector& loadGeometry( std::vector<std::string> xmlList, std::string tpc_readout_name = "",
                                    std::string tag = "", std::string tpc_name = "TPC" );

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
