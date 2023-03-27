void test_loadGeometry() {
  // activate megat library
  LoadMegat();

  // load geometry
  using namespace megat::utility;

  string xmlGeometryPath = fmt::format( "{}/geometry/compact/Megat.xml", std::getenv( "MEGAT_ROOT" ) );
  string xmlTpc          = fmt::format( "{}/geometry/compact/TPC_readout.xml", std::getenv( "MEGAT_ROOT" ) );
  loadGeometry( { xmlGeometryPath, xmlTpc }, "", "TestTag", "TPC", dd4hep::INFO );
}
