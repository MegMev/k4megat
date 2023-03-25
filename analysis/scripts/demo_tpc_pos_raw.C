#include <TROOT.h>
#include <boost/type_traits/type_with_alignment.hpp>
#include <cstddef>
#include <iterator>

R__LOAD_LIBRARY( libpodioDict )
R__LOAD_LIBRARY( libedm4hepDict )

#include "DD4hep/Detector.h"
#include "DDRec/CellIDPositionConverter.h"
#include "edm4hep/TrackerHitData.h"
#include "fmt/format.h"
#include <cstdio>
#include <cstdlib>
#include <fmt/core.h>
#include <stdexcept>
#include <string>

R__LOAD_LIBRARY( libDDCore.so )
R__LOAD_LIBRARY( libDDRec.so )
R__LOAD_LIBRARY( fmt )

using namespace dd4hep;
using namespace dd4hep::rec;
using namespace std;
using ROOT::VecOps::RVec;

#ifdef __CLING__
#  pragma link C++ class RVec < Position> + ;
#endif

void demo_tpc_pos_raw( string infile = "./data/tpcseg_megat.root.bak", string outfile = "test.root", int idx = -1,
                       string br_name = "TpcSegStripHits", string extra_xml = "../../digi/options/tpc_seg_test.xml",
                       string ro_name = "TpcStripHits" ) {

  string xmlGeometryPath = fmt::format( "{}/geometry/compact/Megat.xml", std::getenv( "MEGAT_ROOT" ) );
  auto&  detdesc         = Detector::getInstance();
  detdesc.fromCompact( xmlGeometryPath, DetectorBuildType::BUILD_RECO );
  detdesc.fromCompact( extra_xml, DetectorBuildType::BUILD_RECO );

  //
  auto tpc_sd   = detdesc.sensitiveDetector( "TPC" );
  bool is_strip = false;
  try {
    auto tpc_ro = detdesc.readout( ro_name );
    tpc_sd.setReadout( tpc_ro );
    if ( tpc_ro.segmentation().type() == "MultiSegmentation" ) { is_strip = true; }
  } catch ( const runtime_error& e ) { fmt::print( stderr, "{}\n", e.what() ); }

  auto                    decoder = tpc_sd.idSpec().decoder();
  CellIDPositionConverter idCov{ detdesc };

  //
  auto get_dPos = [&]( const RVec<edm4hep::TrackerHitData>& in ) {
    RVec<dd4hep::Position> result;
    for ( auto& hit : in ) {
      auto             cell_pos = idCov.position( hit.cellID );
      dd4hep::Position real_pos{ hit.position.x, hit.position.y, hit.position.z };
      auto             delta = cell_pos - real_pos;

      if ( idx >= 0 ) {
        auto x_id = decoder->get( hit.cellID, "x" );
        if ( x_id != idx ) continue;
      }

      if ( is_strip ) {
        auto layer = decoder->get( hit.cellID, "layer" );

        if ( layer == 1 ) {
          auto temp = delta.y();
          delta.SetY( delta.x() );
          delta.SetX( temp );
        }
      }

      result.push_back( delta );
    }
    return result;
  };

  ROOT::EnableImplicitMT();
  ROOT::RDataFrame df( "events", infile, { br_name } );
  auto             df_out = df.Define( "tpc_dPos", get_dPos )
                    .Define( "dx",
                             []( const RVec<dd4hep::Position>& pos ) {
                               return Map( pos, []( const dd4hep::Position& hit ) { return hit.x(); } );
                             },
                             { "tpc_dPos" } )
                    .Define( "dy",
                             []( const RVec<dd4hep::Position>& pos ) {
                               return Map( pos, []( const dd4hep::Position& hit ) { return hit.y(); } );
                             },
                             { "tpc_dPos" } )
                    .Define( "dz",
                             []( const RVec<dd4hep::Position>& pos ) {
                               return Map( pos, []( const dd4hep::Position& hit ) { return hit.z(); } );
                             },
                             { "tpc_dPos" } );

  //
  auto hdx = df_out.Histo1D( { "hdx", "hdx", 100, -1, 1 }, "dx" );
  auto hdz = df_out.Histo1D( { "hdz", "hdz", 100, -5, 5 }, "dz" );
  auto hdy = df_out.Histo1D( { "hdy", "hdy", 100, -1, 1 }, "dy" );
  if ( is_strip ) hdy = df_out.Histo1D( { "hdy", "hdy", 500, -300, 300 }, "dy" );

  auto c = new TCanvas( "c", "c", 1200, 500 );
  c->Divide( 3, 1 );
  c->cd( 1 );
  hdx->DrawClone();
  c->cd( 2 );
  hdy->DrawClone();
  c->cd( 3 );
  hdz->DrawClone();

  auto hdxdy = df_out.Histo2D( { "hdxdy", "hdxdy", 100, -1, 1, 100, -1, 1 }, "dx", "dy" );
  if ( is_strip ) hdxdy = df_out.Histo2D( { "hdxdy", "hdxdy", 100, -1, 1, 600, -300, 300 }, "dx", "dy" );
  auto c2 = new TCanvas( "c2", "c2", 500, 500 );
  hdxdy->DrawClone( "colz" );

  // ROOT::RDF::SaveGraph( df, "rdf_test.dot" );
  // gSystem->Exec( "dot -Tpng rdf_test.dot -o rdf_test.png" );
}
