#include "test_functor.h"
#include <fmt/core.h>

void demo_tpc_pos( string infile = "./data/tpcseg_megat.root.bak", string outfile = "test.root", int idx = -1,
                   string br_name = "TpcSegStripHits", string ro_name = "TpcStripHits" ) {

  LoadMegat();

  string xmlGeometryPath = fmt::format( "{}/geometry/compact/Megat.xml", std::getenv( "MEGAT_ROOT" ) );
  string xmlTpc          = fmt::format( "{}/geometry/compact/TPC_readout.xml", std::getenv( "MEGAT_ROOT" ) );

  using namespace megat::utility;
  loadGeometry( { xmlGeometryPath, xmlTpc }, ro_name );
  RealPos<edm4hep::TrackerHitData> getPos( ro_name );

  using ROOT::RVecD;
  using ROOT::RVecF;
  ROOT::EnableImplicitMT();
  ROOT::RDataFrame df( "events", infile );
  auto             dfout = df.Define( "real_pos", getPos, { br_name } )
                   .Define( "sim_pos", get_pos<edm4hep::TrackerHitData>, { br_name } )
                   .Define( "delta_pos", "real_pos-sim_pos" )
                   .Define( "dx", VecAt<RVecD>( 0 ), { "delta_pos" } )
                   .Define( "dy", VecAt<RVecD>( 1 ), { "delta_pos" } )
                   .Define( "dz", VecAt<RVecD>( 2 ), { "delta_pos" } );

  auto hdx = dfout.Histo1D( { "hdx", "hdx", 100, -1, 1 }, "dx" );
  auto hdz = dfout.Histo1D( { "hdz", "hdz", 100, -5, 5 }, "dz" );
  auto hdy = dfout.Histo1D( { "hdy", "hdy", 100, -1, 1 }, "dy" );

  auto c3 = new TCanvas( "c3", "c3", 1200, 500 );
  c3->Divide( 3, 1 );
  c3->cd( 1 );
  hdx->DrawClone();
  c3->cd( 2 );
  hdy->DrawClone();
  c3->cd( 3 );
  hdz->DrawClone();

  // ROOT::RDF::SaveGraph( dfout, "rdf_test.dot" );
  // gSystem->Exec( "dot -Tpng rdf_test.dot -o rdf_test.png" );
}
