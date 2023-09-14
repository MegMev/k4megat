R__LOAD_LIBRARY( libDDCore )
void dump() {
  auto& desc = dd4hep::Detector::getInstance();
  desc.fromXML( "AlphaBeta.xml" );
  desc.dump();
}
