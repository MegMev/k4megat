
#include <DD4hep/Plugins.h>
#include <XML/Utilities.h>

namespace {
  using namespace dd4hep;

  long configure_worldname( Detector& detector, xml_h e ) {
    xml_comp_t  c        = e;
    std::string new_name = c.attr<std::string>( _U( name ), "world" );
    DetElement  world_de = detector.world();
    auto        pv       = world_de.placement();
    pv->SetName( new_name );
    return 1;
  }
} // namespace

DECLARE_XML_PLUGIN( Megat_RenameWorldVolume, configure_worldname )
