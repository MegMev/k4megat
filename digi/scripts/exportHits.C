/*
 * Description:
 *
 * Convert edm4hep collection branches into flat-structure, with each branch of
 * std::vector of primitive types.
 * The global position of each readout cells, strip in TPC or crystal in CZT are
 * also extracted.
 * So the exported tree is self-standing.
 *
 * Usage:
 * 1. use test_sim.py as demo job option to generate the simulated hit collection and primary particle collection
 *    the following configuration can be changed as your needs:
 *    - primary generator and its setting
 *    - the random generator and its setting
 *    - the properties of the algorithms
 *    - the output root file name
 *    - the event number
 * 2. use simple_digi.py as the demo job option to generate the digi collections from the output root file in step 1
 *    the following configuration can be changed as your needs:
 *    - the random generator and its setting
 *    - the properties of the algorithms
 *    - the output root file name
 * 3. convert edm4hep collections into flat structure using the output root file from step 2
 *    in root shell:
 *    > .L exportHits.C
 *    > exportHits("input.root", "output.root")
 *    > .q
 *
 * Output branches:
 *
 *
 */

void exportHits( string infile = "../options/simple_digi.root", string outfile = "flat_digi.root" ) {

  // activate megat library
  LoadMegat();

  // load geometry (use same set of compact files as in simple_digi.py)
  using namespace megat::utility;

  string xmlGeometryPath = std::getenv( "MEGAT_ROOT" ) + std::string( "/geometry/compact/Megat.xml" );
  string xmlTpc          = std::getenv( "MEGAT_ROOT" ) + std::string( "/geometry/compact/TPC_readout.xml" );
  string ro_name         = "TpcStripHits";
  loadGeometry( { xmlGeometryPath, xmlTpc }, ro_name );

  // get converter
  IdConverter idConv( ro_name );
  auto        tpc_decoder  = idConv.decoder( "TPC" );
  auto        calo_decoder = idConv.decoder( "Calorimeter" );

  // prepare some processors
  using ROOT::RVecD;
  using namespace ROOT::VecOps;

  // real job
  ROOT::RDF::RSnapshotOptions opts;
  opts.fMode = "update";
  ROOT::RDataFrame df( "events", infile );

  // tpc
  // algorithm to get position of hited cell
  megat::Tracker::cell_pos tpc_cell_pos( ro_name );

  // algorithm to normalize u,v planes in strip readout
  auto tpc_norm_pos = [&]( const RVec<RVecD>& poss, const RVec<CellID>& ids ) {
    auto mapper = [&]( const RVecD& pos, const CellID& id ) {
      return ( tpc_decoder->get( id, "layer" ) == 1 ) ? Take( pos, { 1, 0, 2 } ) : pos;
    };
    return Map( poss, ids, mapper );
  };

  auto tpc_get_layer = [&]( const RVec<CellID>& ids ) {
    return Map( ids, [&]( const CellID& id ) { return tpc_decoder->get( id, "layer" ); } );
  };

  string tpc_br    = "TpcHits";
  auto   tpc_dfout = df.Define( "id", megat::Tracker::hit_id, { tpc_br } )
                       .Define( "energy", megat::Tracker::hit_e, { tpc_br } )
                       .Define( "time", megat::Tracker::hit_t, { tpc_br } )
                       .Define( "layer", tpc_get_layer, { "id" } )
                       .Define( "cell_pos", tpc_cell_pos, { tpc_br } )
                       .Define( "norm_pos", tpc_norm_pos, { "cell_pos", "id" } )
                       .Define( "position", VecAt<double>( 0 ), { "norm_pos" } )
                       .Snapshot( "tpc", outfile, { "layer", "position", "energy", "time" } );

  // calorimeter
  megat::SimCalo::cell_pos calo_cell_pos( ro_name );

  string calo_br    = "CaloHits";
  auto   calo_dfout = df.Define( "energy", megat::SimCalo::hit_e, { calo_br } )
                        .Define( "cell_pos", calo_cell_pos, { calo_br } )
                        .Define( "x", VecAt<double>( 0 ), { "cell_pos" } )
                        .Define( "y", VecAt<double>( 1 ), { "cell_pos" } )
                        .Define( "z", VecAt<double>( 2 ), { "cell_pos" } )
                        .Snapshot( "calo", outfile, { "x", "y", "z", "energy" }, opts );

  // primary particles and vertex
  string primary_br = "GenParticles";
  auto   primary_dfout =
      df.Define( "pdg", megat::MCParticle::pdg, { primary_br } )
          .Define( "mass", megat::MCParticle::mass, { primary_br } )
          .Define( "charge", megat::MCParticle::charge, { primary_br } )
          .Define( "vertex_x", megat::MCParticle::vertex_x, { primary_br } )
          .Define( "vertex_y", megat::MCParticle::vertex_y, { primary_br } )
          .Define( "vertex_z", megat::MCParticle::vertex_z, { primary_br } )
          .Define( "mom_x", megat::MCParticle::mom_x, { primary_br } )
          .Define( "mom_y", megat::MCParticle::mom_y, { primary_br } )
          .Define( "mom_z", megat::MCParticle::mom_z, { primary_br } )
          .Snapshot( "primaries", outfile,
                     { "pdg", "mass", "charge", "vertex_x", "vertex_y", "vertex_z", "mom_x", "mom_y", "mom_z" }, opts );
}
