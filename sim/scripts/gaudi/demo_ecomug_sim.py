from Gaudi.Configuration import *
import GaudiKernel.SystemOfUnits as units

# data store service
from Configurables import k4DataSvc
dataSvc = k4DataSvc("EventDataSvc")

# geometry service
### Configures the detector construction: geometry and sd
from Configurables import MegatGeoSvc as GeoSvc
from os import environ, path
detector_path = environ.get("MEGAT_ROOT", "")
geoSvc = GeoSvc("GeoSvc", detectors=[path.join(detector_path, 'geometry/compact/Megat.xml')],
                    buildType="BUILD_SIMU",
                    # sensitiveTypes={'tracker':'MegatSimpleTrackerSD','calorimeter':'MegatAggregateCalorimeterSD'},
                    OutputLevel = WARNING)

# Rndm service (use G4 default engine)
from Configurables import HepRndm__Engine_CLHEP__HepJamesRandom_
rdmEngine = HepRndm__Engine_CLHEP__HepJamesRandom_("RndmGenSvc.Engine")
rdmEngine.SetSingleton = True
rdmEngine.Seeds = [5685]

from Configurables import RndmGenSvc
rdmSvc = RndmGenSvc("RndmGenSvc")
rdmSvc.Engine = rdmEngine.name()

# region & limits
### definition is in dd4hep compact file
### here, only need to append step limiter to the main physics list
from Configurables import SimUserLimitPhysicsList
physicslist = SimUserLimitPhysicsList("Physics")
physicslist.fullphysics = "SimFtfpBert"

# user actions
from Configurables import SimFullSimActions
actions = SimFullSimActions()
actions.enableHistory=True # toggle, default: false and no action is binded
actions.energyCut= 0.1 # min kinetic energy (MeV) for the generated track to be saved

# g4 service
### Configures the Geant simulation: detector building, fields, regions, physics, actions
from Configurables import SimSvc
simSvc = SimSvc("SimSvc")
simSvc.detector='SimDD4hepDetector'
simSvc.physicslist=physicslist
simSvc.actions= actions
simSvc.g4PostInitCommands += ["/run/setCut 0.1 mm"]
simSvc.randomNumbersFromGaudi = True
#simSvc.OutputLevel = DEBUG

# g4 algorithm
### Configures event generator and output collections via tools
##### generator (in g4 unit)
from Configurables import SimEcoMugGenerator
ecoMug = SimEcoMugGenerator('muonSource')
ecoMug.Position = [0, 0, 800] # mm
ecoMug.Geometry = "Sky";
ecoMug.SizeXY = [400, 400] # mm
ecoMug.ThetaRangeDirection = [0, 45] # degree
# ecoMug.MomentumRange = [0, 100] # GeV
ecoMug.OutputLevel = INFO
ecoMug.Duration = 5 # min

##### output collections
from Configurables import SimSavePrimaries
savePrimaries = SimSavePrimaries('savePrimaries')
savePrimaries.Particles.Path = 'PrimaryParticles'

from Configurables import SimSaveCalHits
saveCalo = SimSaveCalHits('saveCalo',readoutName = 'CztHits')
saveCalo.Hits.Path = 'CaloSimHits'

from Configurables import SimSaveTrackerHits
saveTpc = SimSaveTrackerHits('saveTpc',readoutName = 'TpcHits')
saveTpc.Hits.Path = 'TpcSimHits'

#### finally the alg itself
from Configurables import SimAlg
simAlg = SimAlg('SimAlg',
                  saveTools = [savePrimaries, saveCalo, saveTpc],
                  eventProvider = ecoMug,
                  )
#simAlg.OutputLevel = DEBUG

# output to root file
from Configurables import PodioOutput
outAlg = PodioOutput()
outAlg.filename = 'demo_ecomug_sim.root'
outAlg.outputCommands = ['keep *']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [simAlg, outAlg],
                EvtSel = 'NONE',
                EvtMax = 1000,
                # order is important, as GeoSvc, RndmSvc are needed by G4SimSvc
                ExtSvc = [ rdmEngine, rdmSvc, dataSvc, geoSvc, simSvc],
                OutputLevel = WARNING)
