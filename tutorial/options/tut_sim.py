from Gaudi.Configuration import *
import GaudiKernel.SystemOfUnits as units

# TES data service
from Configurables import k4DataSvc
datasvc = k4DataSvc("EventDataSvc")

# Geometry service
### Configures the detector construction: geometry and sd
from Configurables import MegatGeoSvc as GeoSvc
from os import environ, path
detector_path = environ.get("MEGAT_ROOT", "")
geoservice = GeoSvc("MegatGeoSvc", detectors=[path.join(detector_path, 'geometry/compact/Megat.xml')],
                    # sensitiveTypes={'tracker':'MegatSimpleTrackerSD','calorimeter':'MegatAggregateCalorimeterSD'},
                    OutputLevel = ERROR)

# Physics
### limit definition is in defined in dd4hep compact file
### here, only need to append step limiter to the main physics list
from Configurables import SimUserLimitPhysicsList
physicslist = SimUserLimitPhysicsList("Physics")
physicslist.fullphysics = "SimFtfpBert"

# User actions
from Configurables import SimFullSimActions
actions = SimFullSimActions()
# toggle, default: false and no action is binded
actions.enableHistory=True
# min kinetic energy of the generated track to be saved (in MeV)
actions.energyCut= 0.1

# Geant4 service
### Configures the Geant simulation: detector building, fields, regions, physics, actions
from Configurables import SimSvc
geantservice = SimSvc("SimSvc")
geantservice.detector='SimDD4hepDetector'
geantservice.physicslist=physicslist
geantservice.actions= actions
geantservice.g4PostInitCommands += ["/run/setCut 0.1 mm"]
geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"] # if to store track points
### The two options following are needed for reproducible output
geantservice.randomNumbersFromGaudi = False
geantservice.seedValue = 4242

# Main Geant4 algorithm
### Configures event provider (i.e. generator) and output collections via tools
##### 1. particle gun generator (in g4 unit)
from Configurables import SimSingleParticleGeneratorTool
pgun=SimSingleParticleGeneratorTool('ParticleGun', saveEdm=True,
                                      particleName = "mu-", energyMin = 10000, energyMax = 10000,
                                      etaMin = -4, etaMax = 4, phiMin = -3.14, phiMax = 3.14,
                                      OutputLevel = ERROR)

##### 2. configure collections to be recorded in TES
from Configurables import SimSaveCalHits
saveCalo = SimSaveCalHits('saveCalo',readoutNames = ['CztHits'])
saveCalo.CaloHits.Path = 'CztHits'

from Configurables import SimSaveTrackerHits
saveTpc = SimSaveTrackerHits('saveTpc',readoutNames = ['TpcHits'])
saveTpc.SimTrackHits.Path = 'TpcHits'

##### 3. record trajectory in TES
from Configurables import SimSaveTrajectory
savetrajectorytool = SimSaveTrajectory("saveTrajectory")
savetrajectorytool.TrajectoryPoints.Path = "trajectoryPoints"

##### 4. record particle history, i.e. MCParticle in TES
from Configurables import SimSaveParticleHistory
savehisttool = SimSaveParticleHistory("saveHistory")
savehisttool.GenParticles.Path = "SimParticles"

##### 5. finally configure the alg itself
from Configurables import SimAlg
geantsim = SimAlg('SimAlg',
                    outputs= ['SimSaveCalHits/saveCalo',
                              'SimSaveTrackerHits/saveTpc',
                              'SimSaveParticleHistory/saveHistory',
                              'SimSaveTrajectory/saveTrajectory',
                              ],
                    eventProvider = pgun,
                    OutputLevel = WARNING)

# Select & Write the collections to disk ROOT file
from Configurables import PodioOutput
out = PodioOutput('out')
out.filename = 'megat_tut.root'
#### write out all objects in TES (or you can select specific collections)
out.outputCommands = ['keep *']

# Add algorithms and services to ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [datasvc, geoservice, geantservice],
                OutputLevel = ERROR)
