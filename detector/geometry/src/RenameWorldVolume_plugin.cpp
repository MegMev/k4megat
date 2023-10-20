#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "XML/XML.h"

namespace {
  using namespace dd4hep;

  long configure_worldname( Detector& detector, xml_h e ) {
    xml_comp_t  c        = e;
    std::string new_name = c.attr<std::string>( _Unicode( new_name ), "World" );
    DetElement  world_de = detector.world();
    auto        pv       = world_de.placement();
    pv->SetName( new_name.c_str() );
    // [todo]: no effect changing de's name of de tree
    // world_de->SetName(new_name.c_str());
    // world_de.volume()->SetName(new_name.c_str());
    return 1;
  }
} // namespace

DECLARE_XML_PLUGIN( Megat_RenameWorldVolume, configure_worldname )
