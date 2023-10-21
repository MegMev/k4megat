#pragma once

// Interface
#include "SimInterface/IGeoSvc.h"

// Gaudi
#include "Gaudi/Property.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"

// DD4Hep
#include "DD4hep/BuildType.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/Detector.h"
#include "DD4hep/VolumeManager.h"
#include "DD4hep/Volumes.h"
#include "DDRec/Surface.h"

// Geant4
#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "GaudiKernel/StatusCode.h"
#include "Parsers/Primitives.h"
#include <string>

namespace megat {

  class GeoSvc : public extends<Service, IGeoSvc> {

  public:
    /// Default constructor
    GeoSvc( const std::string& name, ISvcLocator* svc );

    /// Destructor
    virtual ~GeoSvc();
    /// Initialize function
    virtual StatusCode initialize() final;
    /// Finalize function
    virtual StatusCode finalize() final;
    /// This function generates the DD4hep geometry
    StatusCode buildDD4HepGeo();
    /// This function generates the Geant4 geometry
    StatusCode buildGeant4Geo();
    // receive DD4hep Geometry
    virtual dd4hep::DetElement    getDD4HepGeo() override;
    virtual dd4hep::Detector*     lcdd() override;
    virtual dd4hep::VolumeManager getVolumeManager() final { return m_volMgr; };

    // receive Geant4 Geometry
    virtual G4VUserDetectorConstruction* getGeant4Geo() override;

    virtual dd4hep::rec::SurfaceList getSensitiveSurfList( dd4hep::VolumeID volumeID ) override;
    virtual dd4hep::rec::SurfaceList getHelperSurfList( dd4hep::VolumeID volumeID ) override;

    virtual dd4hep::PlacedVolume getPlacedVolume( std::string path );
    virtual dd4hep::DetElement   getDetElement( std::string path );

  private:
    /// Pointer to the interface to the DD4hep geometry
    dd4hep::Detector* m_dd4hepgeo;
    /// volume manager
    dd4hep::VolumeManager m_volMgr;

    /// Pointer to the detector construction of DDG4
    std::shared_ptr<G4VUserDetectorConstruction> m_geant4geo;
    /// caching of the surface list
    std::map<dd4hep::VolumeID, dd4hep::rec::SurfaceList> m_sensitiveSurfList;
    std::map<dd4hep::VolumeID, dd4hep::rec::SurfaceList> m_helperSurfList;

    /// geometry build type: sim, rec, env, dis
    Gaudi::Property<std::string> m_build_typeString{
        this, "buildType", "BUILD_RECO", "Geometry build type: BUILD_SIMU, BUILD_RECO, BUILD_DISPLAY, BUILD_ENVELOPE" };

    /// XML-files with the detector description
    Gaudi::Property<std::vector<std::string>> m_xmlFileNames{
        this, "detectors", {}, "Detector descriptions XML-files" };
    /// mapping of sensitive detector names
    Gaudi::Property<std::map<std::string, std::string>> m_sensitive_types{
        this,
        "sensitiveTypes",
        { { "tracker", "MegatSimpleTrackerSD" }, { "calorimeter", "MegatAggregateCalorimeterSD" } } };
  };

} // namespace megat
