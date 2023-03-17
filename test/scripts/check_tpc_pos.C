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

void check_tpc_pos( string infile = "./data/tpcseg_megat.root.bak", string outfile = "test.root", int idx = -1,
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
    // vector<dd4hep::Position> result;
    RVec<dd4hep::Position> result;
    // using RPos = RVec<array<float, 3>>;
    // RPos result;
    // RVec<pos> result;
    // RVec<double> result;
    for ( auto& hit : in ) {
      auto             cell_pos = idCov.position( hit.cellID );
      dd4hep::Position real_pos{ hit.position.x, hit.position.y, hit.position.z };
      auto             delta = real_pos - cell_pos;

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

  auto get_dPos_alt = [&]( const RVec<edm4hep::TrackerHitData>& in ) {
    using ROOT::VecOps::Filter;
    using PosVec   = ROOT::RVec<dd4hep::Position>;
    using TrackVec = ROOT::RVec<edm4hep::TrackerHitData>;
    TrackVec input;
    if ( idx >= 0 ) {
      input = Filter( in, [&]( const edm4hep::TrackerHitData& hit ) {
        auto x_id = decoder->get( hit.cellID, "x" );
        return x_id == idx;
      } );
    } else {
      input = in;
    }

    return Map( input, [&]( const edm4hep::TrackerHitData hit ) {
      auto             cell_pos = idCov.position( hit.cellID );
      dd4hep::Position real_pos{ hit.position.x, hit.position.y, hit.position.z };
      auto             delta = real_pos - cell_pos;

      if ( is_strip ) {
        auto layer = decoder->get( hit.cellID, "layer" );

        if ( layer == 1 ) {
          auto temp = delta.y();
          delta.SetY( delta.x() );
          delta.SetX( temp );
        }
      }

      return delta;
    } );
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
                             { "tpc_dPos" } )
                    .Define( "tpc_dPos_alt", get_dPos_alt )
                    .Define( "dx_alt",
                             []( const RVec<dd4hep::Position>& pos ) {
                               return Map( pos, []( const dd4hep::Position& hit ) { return hit.x(); } );
                             },
                             { "tpc_dPos_alt" } )
                    .Define( "dy_alt",
                             []( const RVec<dd4hep::Position>& pos ) {
                               return Map( pos, []( const dd4hep::Position& hit ) { return hit.y(); } );
                             },
                             { "tpc_dPos_alt" } )
                    .Define( "dz_alt",
                             []( const RVec<dd4hep::Position>& pos ) {
                               return Map( pos, []( const dd4hep::Position& hit ) { return hit.z(); } );
                             },
                             { "tpc_dPos_alt" } );

  //
  auto hdx = df_out.Histo1D( { "hdx", "hdx", 100, -1, 1 }, "dx" );
  auto hdz = df_out.Histo1D( { "hdz", "hdz", 100, -5, 5 }, "dz" );
  auto hdy = df_out.Histo1D( { "hdy", "hdy", 100, -1, 1 }, "dy" );
  if ( is_strip ) hdy = df_out.Histo1D( { "hdy", "hdy", 500, -300, 300 }, "dy" );

  auto c = new TCanvas( "c", "c", 900, 300 );
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

  auto hdx_alt = df_out.Define( "delta_x", "dx - dx_alt" ).Histo1D( { "hdx_alt", "hdx_alt", 100, -1, 1 }, "delta_x" );
  auto hdy_alt = df_out.Define( "delta_y", "dy - dy_alt" ).Histo1D( { "hdy_alt", "hdy_alt", 100, -1, 1 }, "delta_y" );
  auto hdz_alt = df_out.Define( "delta_z", "dz - dz_alt" ).Histo1D( { "hdz_alt", "hdz_alt", 100, -1, 1 }, "delta_z" );

  // auto hdz_alt= df_out.Histo1D( { "hdz", "hdz", 100, -5, 5 }, "dz" );
  // auto hdy_alt = df_out.Histo1D( { "hdy", "hdy", 100, -1, 1 }, "dy" );
  // if ( is_strip ) hdy = df_out.Histo1D( { "hdy", "hdy", 500, -300, 300 }, "dy" );
  auto c3 = new TCanvas( "c3", "c3", 900, 300 );
  c3->Divide( 3, 1 );
  c3->cd( 1 );
  hdx_alt->DrawClone();
  c3->cd( 2 );
  hdy_alt->DrawClone();
  c3->cd( 3 );
  hdz_alt->DrawClone();

  // df_out.Describe();
  // ROOT::RDF::SaveGraph( df, "rdf_test.dot" );
  // gSystem->Exec( "dot -Tpng rdf_test.dot -o rdf_test.png" );

  // df_out.Describe().Print();
}
