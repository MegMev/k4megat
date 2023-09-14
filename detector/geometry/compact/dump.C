R__LOAD_LIBRARY( libDDCore )
void dump() {
  auto& desc = dd4hep::Detector::getInstance();
  desc.fromXML( "Megat.xml" );
  desc.manager().SetVisLevel( 6 );
  desc.dump();
}
