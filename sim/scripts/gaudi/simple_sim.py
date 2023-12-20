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
geoservice = GeoSvc("GeoSvc", detectors=[path.join(detector_path, 'geometry/compact/Megat.xml')],
                    buildType="BUILD_SIMU",
                    # sensitiveTypes={'tracker':'MegatSimpleTrackerSD','calorimeter':'MegatAggregateCalorimeterSD'},
                    OutputLevel = INFO)

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
geantservice = SimSvc("SimSvc")
geantservice.detector='SimDD4hepDetector'
geantservice.physicslist=physicslist
geantservice.actions= actions
geantservice.g4PostInitCommands += ["/run/setCut 0.1 mm"]
geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"] # if to store track points
geantservice.randomNumbersFromGaudi = True
# geantservice.seedValue = 4242

# g4 algorithm
### Configures event generator and output collections via tools
##### generator (in g4 unit)
from Configurables import SimSingleParticleGenerator
pgun = SimSingleParticleGenerator('particleGun',
                                  particleName = "mu-", energyMin = 3000, energyMax = 10000,
                                  thetaMin = 0, thetaMax = 180, phiMin = 0, phiMax = 360,
                                  # vertexX=0, vertexZ=0, vertexY = 0,
                                  OutputLevel = WARNING)

##### vertex smearing tool
from Configurables import SimVertexSmearVolumeTool
vxSmearer = SimVertexSmearVolumeTool('vertexSmear')
vxSmearer.volumePath = '/world/TPC/Gas'

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

#### save trajectory and history
from Configurables import SimSaveTrajectory
saveTrajectory = SimSaveTrajectory("saveTrajectory")
saveTrajectory.Hits.Path = "TrajectoryPoints"

from Configurables import SimSaveHistory
saveHistory = SimSaveHistory("saveHistory")
saveHistory.Hits.Path = "SimParticles"

#### finally the alg itself
from Configurables import SimAlg
geantsim = SimAlg('SimAlg',
                  saveTools = [savePrimaries, saveCalo, saveTpc, saveTrajectory, saveHistory],
                  eventProvider = pgun,
                  vertexSmearer = vxSmearer,
                  OutputLevel = WARNING)

# output to root file
from Configurables import PodioOutput
out = PodioOutput()
out.filename = 'simple_sim.root'
out.outputCommands = ['keep *']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax = 1000,
                # order is important, as GeoSvc, RndmSvc are needed by G4SimSvc
                ExtSvc = [ rdmEngine, rdmSvc, podioevent, geoservice, geantservice],
                OutputLevel = WARNING)
