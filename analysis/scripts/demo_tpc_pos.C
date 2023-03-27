void demo_tpc_pos( string infile = "./data/tpcseg_megat.root.bak", string outfile = "test.root", int idx = -1,
                   string br_name = "TpcSegStripHits", string ro_name = "TpcStripHits" ) {

  // activate megat library
  LoadMegat();

  // load geometry
  using namespace megat::utility;

  string xmlGeometryPath = fmt::format( "{}/geometry/compact/Megat.xml", std::getenv( "MEGAT_ROOT" ) );
  string xmlTpc          = fmt::format( "{}/geometry/compact/TPC_readout.xml", std::getenv( "MEGAT_ROOT" ) );
  loadGeometry( { xmlGeometryPath, xmlTpc }, ro_name );

  // get converter
  IdConverter idConv( ro_name );
  auto        decoder  = idConv.decoder( "TPC" );
  bool        is_strip = idConv.isStrip( "TPC" );

  // prepare some processors
  using ROOT::RVecD;
  using namespace ROOT::VecOps;
  ROOT::EnableImplicitMT();

  // algorithm to get position of hited cell
  CellPosition<edm4hep::TrackerHitData> cell_pos( ro_name );

  // algorithm to normalize u,v planes in strip readout
  auto norm_pos = [=]( const RVec<RVecD>& poss, const RVec<CellId>& ids ) {
    auto mapper = [=]( const RVecD& pos, const CellID& id ) {
      return ( is_strip && decoder->get( id, "layer" ) == 1 ) ? Take( pos, { 1, 0, 2 } ) : pos;
    };
    return Map( poss, ids, mapper );
  };

  // real job
  ROOT::RDataFrame df( "events", infile );
  auto             dfout = df.Define( "id", megat::Tracker::hit_id, { br_name } )
                   .Define( "cell_pos", cell_pos, { br_name } )
                   .Define( "sim_pos", megat::Tracker::hit_pos, { br_name } )
                   // .Define( "delta_pos_original", "cell_pos-sim_pos" )
                   // .Define( "delta_pos", norm_pos, { "delta_pos_original", "id" } )
                   .Define( "delta_pos", "cell_pos-sim_pos" )
                   .Redefine( "delta_pos", norm_pos, { "delta_pos", "id" } )
                   .Define( "dx", VecAt<double>( 0 ), { "delta_pos" } )
                   .Define( "dy", VecAt<double>( 1 ), { "delta_pos" } )
                   .Define( "dz", VecAt<double>( 2 ), { "delta_pos" } );

  auto hdx = dfout.Histo1D( { "hdx", "hdx", 100, -1, 1 }, "dx" );
  auto hdz = dfout.Histo1D( { "hdz", "hdz", 100, -5, 5 }, "dz" );
  auto hdy = dfout.Histo1D( { "hdy", "hdy", 100, -1, 1 }, "dy" );
  if ( is_strip ) hdy = dfout.Histo1D( { "hdy", "hdy", 500, -300, 300 }, "dy" );

  auto c3 = new TCanvas( "c3", "c3", 1200, 500 );
  c3->Divide( 3, 1 );
  c3->cd( 1 );
  hdx->DrawClone();
  c3->cd( 2 );
  hdy->DrawClone();
  c3->cd( 3 );
  hdz->DrawClone();

  auto hdxdy = dfout.Histo2D( { "hdxdy", "hdxdy", 100, -1, 1, 100, -1, 1 }, "dx", "dy" );
  if ( is_strip ) hdxdy = dfout.Histo2D( { "hdxdy", "hdxdy", 100, -1, 1, 600, -300, 300 }, "dx", "dy" );
  auto c2 = new TCanvas( "c2", "c2", 500, 500 );
  hdxdy->DrawClone( "colz" );

  // ROOT::RDF::SaveGraph( dfout, "rdf_test.dot" );
  // gSystem->Exec( "dot -Tpng rdf_test.dot -o rdf_test.png" );
}
