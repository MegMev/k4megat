void exportHits_tpc( string infile = "../options/simple_digi.root", string outfile = "flat_tpc_hits.root",
                     string br_name = "TpcHits", string ro_name = "TpcStripHits" ) {

  // activate megat library
  LoadMegat();

  // load geometry
  using namespace megat::utility;

  string xmlGeometryPath = std::getenv( "MEGAT_ROOT" ) + std::string( "/geometry/compact/Megat.xml" );
  string xmlTpc          = std::getenv( "MEGAT_ROOT" ) + std::string( "/geometry/compact/TPC_readout.xml" );
  loadGeometry( { xmlGeometryPath, xmlTpc }, ro_name );

  // get converter
  IdConverter idConv( ro_name );
  auto        decoder = idConv.decoder( "TPC" );

  // prepare some processors
  using ROOT::RVecD;
  using namespace ROOT::VecOps;
  ROOT::EnableImplicitMT();

  // algorithm to get position of hited cell
  megat::Tracker::cell_pos cell_pos( ro_name );

  // algorithm to normalize u,v planes in strip readout
  auto norm_pos = [&]( const RVec<RVecD>& poss, const RVec<CellID>& ids ) {
    auto mapper = [&]( const RVecD& pos, const CellID& id ) {
      return ( decoder->get( id, "layer" ) == 1 ) ? Take( pos, { 1, 0, 2 } ) : pos;
    };
    return Map( poss, ids, mapper );
  };

  auto get_layer = [&]( const RVec<CellID>& ids ) {
    return Map( ids, [&]( const CellID& id ) { return decoder->get( id, "layer" ); } );
  };

  // real job
  ROOT::RDataFrame df( "events", infile );
  auto             dfout = df.Define( "id", megat::Tracker::hit_id, { br_name } )
                   .Define( "energy", megat::Tracker::hit_e, { br_name } )
                   .Define( "time", megat::Tracker::hit_t, { br_name } )
                   .Define( "layer", get_layer, { "id" } )
                   .Define( "cell_pos", cell_pos, { br_name } )
                   .Define( "norm_pos", norm_pos, { "cell_pos", "id" } )
                   .Define( "position", VecAt<double>( 0 ), { "norm_pos" } )
                   .Snapshot( "flat_events", outfile, { "layer", "position", "energy", "time" } );
}
