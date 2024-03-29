void demo_fillx() {
  // trigger the library auto-loading
  LoadMegat();

  // we need MT for long - time job, here is for demo
  ROOT::EnableImplicitMT();

  // real job
  ROOT::RDataFrame df( "events", "./data/megat.edm4hep.root" );

  // method 1
  gInterpreter->Declare( " using namespace megat::utility;" );

  auto df1 = df.Define( "x", "get_x<edm4hep::SimCalorimeterHitData>(CztHits)" )
                 .Define( "y", "get_y<edm4hep::SimCalorimeterHitData>(CztHits)" )
                 .Define( "z", "get_z<edm4hep::SimCalorimeterHitData>(CztHits)" )
                 .Define( "e", "get_energy<edm4hep::SimCalorimeterHitData>(CztHits)" );

  // method 2
  gInterpreter->Declare( "using namespace megat::SimCalo; " );

  auto df2 = df.Define( "x", "hit_x(CztHits)" )
                 .Define( "y", "hit_y(CztHits)" )
                 .Define( "z", "hit_z(CztHits)" )
                 .Define( "e", "hit_e(CztHits)" );

  // method 3
  using namespace megat::SimCalo;

  auto df3 = df.Define( "x", "CztHits.position.x" )
                 .Define( "y", "CztHits.position.y" )
                 .Define( "z", "CztHits.position.z" )
                 .Define( "e", "CztHits.energy" );

  // draw & print
  auto h1 = df3.Histo1D( "x" );
  auto h2 = df3.Histo1D( "y" );
  auto h3 = df3.Histo1D( "e" );

  auto c = new TCanvas( "c", "c", 1200, 500 );
  c->Divide( 3, 1 );
  c->cd( 1 );
  h1->DrawClone();
  c->cd( 2 );
  h2->DrawClone();
  c->cd( 3 );
  h3->DrawClone();
}
