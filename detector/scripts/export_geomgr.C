// #include <DD4hep/Detector.h>
#include <TGeoManager.h>
#include <string>
#include <string_view>
R__LOAD_LIBRARY( DDCore )

void export_geomgr( std::string compact_file = "../geometry/compact/Megat.xml",
                    std::string out_file     = "default.root" ) {
  auto& desc = dd4hep::Detector::getInstance();
  desc.fromXML( compact_file );
  auto& geoMgr = desc.manager();
  geoMgr.SetMaxVisNodes( 10000 );
  geoMgr.SetVisLevel( 6 );
  geoMgr.SetVisOption( 1 );
  auto vol = geoMgr.GetTopVolume();
  vol->Draw( "ogl" );
  geoMgr.Export( out_file.data(), "Megat", "ogl" );
  // desc.dump();
}
