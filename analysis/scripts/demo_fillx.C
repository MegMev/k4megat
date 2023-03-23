void demo_fillx() {
  // trigger the library auto-loading
  LoadMegat();

  // we need MT for long - time job, here is for demo
  ROOT::EnableImplicitMT();

  // real job
  ROOT::RDataFrame df( "events", "./data/megat.edm4hep.root" );
  auto             df1 = df.Define( "x", "megat::utility::get_x<edm4hep::SimCalorimeterHitData>(CztHits)" )
                 .Define( "y", "megat::utility::get_y<edm4hep::SimCalorimeterHitData>(CztHits)" )
                 .Define( "z", "megat::utility::get_z<edm4hep::SimCalorimeterHitData>(CztHits)" );

  auto df2 = df.Define( "x", "megat::SimCalo::hit_x(CztHits)" )
                 .Define( "y", "megat::SimCalo::hit_y(CztHits)" )
                 .Define( "z", "megat::SimCalo::hit_z(CztHits)" );

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
