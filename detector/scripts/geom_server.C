R__LOAD_LIBRARY( libDDCore )

void geom_server() {
  auto& desc = dd4hep::Detector::getInstance();
  desc.fromXML( "../geometry/compact/Megat.xml" );
  desc.dump();
}
