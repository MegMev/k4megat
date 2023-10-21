//
//  IGeoSvc.h
//
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#ifndef IGEOSVC_H
#define IGEOSVC_H

#include "DD4hep/DetElement.h"
#include "DD4hep/VolumeManager.h"
#include "DD4hep/Volumes.h"
#include "DDRec/Surface.h"
#include "GaudiKernel/IService.h"
#include "Parsers/Primitives.h"

namespace dd4hep {
  class Detector;
  class DetElement;
} // namespace dd4hep

class G4VUserDetectorConstruction;

class GAUDI_API IGeoSvc : virtual public IService {

public:
  /// InterfaceID
  DeclareInterfaceID( IGeoSvc, 1, 0 );

  // receive DD4hep Geometry
  virtual dd4hep::DetElement    getDD4HepGeo()     = 0;
  virtual dd4hep::Detector*     lcdd()             = 0;
  virtual dd4hep::VolumeManager getVolumeManager() = 0;

  // receive Geant4 Geometry
  virtual G4VUserDetectorConstruction* getGeant4Geo() = 0;

  // receive list of attached surfaces of a subdetector based on component's volumeID
  virtual dd4hep::rec::SurfaceList getSensitiveSurfList( dd4hep::VolumeID volumeID ) = 0;
  virtual dd4hep::rec::SurfaceList getHelperSurfList( dd4hep::VolumeID volumeID )    = 0;

  // find a placed node, path can be de tree or pv tree
  virtual dd4hep::PlacedVolume getPlacedVolume( std::string path ) = 0;
  virtual dd4hep::DetElement   getDetElement( std::string path )   = 0;

  virtual ~IGeoSvc() {}
};

#endif // IGEOSVC_H
