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
                 .Define( "z", "get_z<edm4hep::SimCalorimeterHitData>(CztHits)" );

  // method 2
  gInterpreter->Declare( "using namespace megat::SimCalo; " );

  auto df2 = df.Define( "x", "hit_x(CztHits)" ).Define( "y", "hit_y(CztHits)" ).Define( "z", "hit_z(CztHits)" );

  // method 3
  using namespace megat::SimCalo;

  auto df3 =
      df.Define( "x", hit_x, { "CztHits" } ).Define( "y", hit_y, { "CztHits" } ).Define( "z", hit_z, { "CztHits" } );

  // draw & print
  auto h1 = df2.Histo1D( "x" );
  auto h2 = df2.Histo1D( "y" );
  auto h3 = df2.Histo1D( "z" );

  auto c = new TCanvas( "c", "c", 1200, 500 );
  c->Divide( 3, 1 );
  c->cd( 1 );
  h1->DrawClone();
  c->cd( 2 );
  h2->DrawClone();
  c->cd( 3 );
  h3->DrawClone();
}
