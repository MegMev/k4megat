from Gaudi.Configuration import *
import GaudiKernel.SystemOfUnits as units

# data store service
from Configurables import k4DataSvc
podioevent = k4DataSvc("EventDataSvc")

# geometry service
### Configures the detector construction: geometry and sd
from Configurables import MegatGeoSvc as GeoSvc
from os import environ, path
detector_path = environ.get("MEGAT_ROOT", "")
geoSvc = GeoSvc("GeoSvc", detectors=[path.join(detector_path, 'geometry/compact/Megat.xml')],
                    buildType="BUILD_SIMU",
                    # sensitiveTypes={'tracker':'MegatSimpleTrackerSD','calorimeter':'MegatAggregateCalorimeterSD'},
                    OutputLevel = INFO)

### Configures the Geant simulation: detector building, fields, regions, physics, actions
from Configurables import SimSvc
simSvc = SimSvc('SimSvc')
simSvc.g4VisInitCommands += ['/control/execute vis.mac']
simSvc.InteractiveMode = True

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( EvtSel = 'NONE',
                EvtMax = -1,
                ExtSvc = [ geoSvc, simSvc ],
                OutputLevel = WARNING)
