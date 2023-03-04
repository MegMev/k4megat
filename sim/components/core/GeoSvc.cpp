
#include "GeoSvc.h"
#include "GaudiKernel/Service.h"
#include "SimKernel/GeoConstruction.h"
#include "TGeoManager.h"

#include "DD4hep/Printout.h"
#include "DDRec/SurfaceHelper.h"
#include <DD4hep/BuildType.h>
#include <DDRec/Surface.h>

namespace megat {
  using namespace Gaudi;

  DECLARE_COMPONENT_WITH_ID( GeoSvc, "MegatGeoSvc" )

  GeoSvc::GeoSvc( const std::string& name, ISvcLocator* svc )
      : base_class( name, svc ), m_dd4hepgeo( 0 ), m_geant4geo( 0 ) {}

  GeoSvc::~GeoSvc() {
    if ( m_dd4hepgeo ) {
      // m_dd4hepgeo->destroyInstance();
    }
  }

  StatusCode GeoSvc::initialize() {
    StatusCode sc = Service::initialize();
    if ( !sc.isSuccess() ) return sc;

    // Turn off TGeo printouts if appropriate for the msg level
    if ( msgLevel() >= MSG::INFO ) { TGeoManager::SetVerboseLevel( 0 ); }

    uint printoutLevel = msgLevel();
    dd4hep::setPrintLevel( dd4hep::PrintLevel( printoutLevel ) );

    if ( buildDD4HepGeo().isFailure() )
      error() << "Could not build DD4Hep geometry" << endmsg;
    else
      info() << "DD4Hep geometry SUCCESSFULLY built" << endmsg;

    if ( buildGeant4Geo().isFailure() )
      error() << "Could not build Geant4 geometry" << endmsg;
    else
      info() << "Geant4 geometry SUCCESSFULLY built" << endmsg;

    // TODO: return failure
    return StatusCode::SUCCESS;
  }

  StatusCode GeoSvc::finalize() { return StatusCode::SUCCESS; }

  StatusCode GeoSvc::buildDD4HepGeo() {
    // we retrieve the the static instance of the DD4HEP::Geometry
    m_dd4hepgeo = &( dd4hep::Detector::getInstance() );
    m_dd4hepgeo->addExtension<IGeoSvc>( this );

    // determine build type
    auto type = dd4hep::buildType( m_build_typeString );

    // load geometry
    for ( auto& filename : m_xmlFileNames ) {
      info() << "loading geometry from file:  '" << filename << "'" << endmsg;
      m_dd4hepgeo->fromCompact( filename, type );
    }
    if ( not m_dd4hepgeo->volumeManager().isValid() ) { m_dd4hepgeo->apply( "DD4hepVolumeManager", 0, 0 ); }
    m_volMgr = m_dd4hepgeo->volumeManager();

    return StatusCode::SUCCESS;
  }

  dd4hep::Detector* GeoSvc::lcdd() { return ( m_dd4hepgeo ); }

  dd4hep::DetElement GeoSvc::getDD4HepGeo() { return ( lcdd()->world() ); }

  StatusCode GeoSvc::buildGeant4Geo() {
    std::shared_ptr<G4VUserDetectorConstruction> detector( new sim::GeoConstruction( *lcdd(), m_sensitive_types ) );
    m_geant4geo = detector;
    if ( nullptr != m_geant4geo ) {
      return StatusCode::SUCCESS;
    } else
      return StatusCode::FAILURE;
  }

  G4VUserDetectorConstruction* GeoSvc::getGeant4Geo() { return ( m_geant4geo.get() ); }

  dd4hep::rec::SurfaceList GeoSvc::getSensitiveSurfList( dd4hep::VolumeID volumeID ) {
    auto it = m_sensitiveSurfList.find( volumeID );
    if ( it != m_sensitiveSurfList.end() ) { return m_sensitiveSurfList[volumeID]; }

    auto sub_volmgr = m_volMgr.subdetector( volumeID );
    auto sub_de     = sub_volmgr.detector();
    if ( sub_de.isValid() ) {
      dd4hep::rec::SurfaceHelper surfMan( sub_de );
      const auto&                sL = surfMan.surfaceList();
      for ( auto& surf : sL ) {
        auto type = surf->type();
        if ( type.isSensitive() ) m_sensitiveSurfList[volumeID].push_back( surf );
      }
    }
    return m_sensitiveSurfList[volumeID];
  }

  dd4hep::rec::SurfaceList GeoSvc::getHelperSurfList( dd4hep::VolumeID volumeID ) {
    auto it = m_helperSurfList.find( volumeID );
    if ( it != m_helperSurfList.end() ) { return m_helperSurfList[volumeID]; }

    auto sub_volmgr = m_volMgr.subdetector( volumeID );
    auto sub_de     = sub_volmgr.detector();
    if ( sub_de.isValid() ) {
      dd4hep::rec::SurfaceHelper surfMan( sub_de );
      const auto&                sL = surfMan.surfaceList();
      for ( auto& surf : sL ) {
        auto type = surf->type();
        if ( type.isHelper() ) m_helperSurfList[volumeID].push_back( surf );
      }
    }
    return m_helperSurfList[volumeID];
  }
} // namespace megat
