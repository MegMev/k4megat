from Gaudi.Configuration import *
import GaudiKernel.SystemOfUnits as units

# TES
from Configurables import k4LegacyDataSvc
podioevent = k4LegacyDataSvc("EventDataSvc")

# geometry service
### Configures the detector construction: geometry and sd
from Configurables import GeoSvc
from os import environ, path
detector_path = environ.get("MEGAT_ROOT", "")
geoservice = GeoSvc("GeoSvc", detectors=[path.join(detector_path, 'geometry/compact/Megat.xml')],
                    sensitiveTypes={'tracker':'MegatSimpleTrackerSD','calorimeter':'MegatAggregateCalorimeterSD'},
                    OutputLevel = WARNING)

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
actions.energyCut= 0.1 # min kinetic e the generated track to be saved (in MeV)

# detector
from Configurables import SimDD4hepDetector
detector = SimDD4hepDetector()
detector.GeoSvc = 'GeoSvc'

# g4 service
### Configures the Geant simulation: detector building, fields, regions, physics, actions
from Configurables import SimSvc
geantservice = SimSvc("SimSvc")
geantservice.detector=detector
geantservice.physicslist=physicslist
geantservice.actions= actions
geantservice.g4PostInitCommands += ["/run/setCut 0.1 mm"]
geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"] # if to store track points
### Take rndm seed under control to have reproducible output
geantservice.randomNumbersFromGaudi = False
geantservice.seedValue = 4242

# g4 algorithm
### Configures event provider (i.e. generator) and output collections via tools
##### generator (in g4 unit)
from Configurables import SimSingleParticleGeneratorTool
pgun=SimSingleParticleGeneratorTool('ParticleGun', saveEdm=True,
                                      particleName = "mu-", energyMin = 10000, energyMax = 10000,
                                      etaMin = -4, etaMax = 4, phiMin = -3.14, phiMax = 3.14,
                                      OutputLevel = DEBUG)

##### output collections
from Configurables import SimSaveCalHits
saveCalo = SimSaveCalHits('saveCalo',readoutNames = ['CztHits'])
saveCalo.CaloHits.Path = 'CztHits'

from Configurables import SimSaveTrackerHits
saveTpc = SimSaveTrackerHits('saveTpc',readoutNames = ['TpcHits'])
saveTpc.SimTrackHits.Path = 'TpcHits'

##### save trajectory and history
from Configurables import SimSaveTrajectory
savetrajectorytool = SimSaveTrajectory("saveTrajectory")
savetrajectorytool.TrajectoryPoints.Path = "trajectoryPoints"

from Configurables import SimSaveParticleHistory
savehisttool = SimSaveParticleHistory("saveHistory")
savehisttool.GenParticles.Path = "SimParticles"

##### finally the alg itself
from Configurables import SimAlg
geantsim = SimAlg('SimAlg',
                    # outputs= ['SimSaveCalHits/saveCalo',
                    #           'SimSaveTrackerHits/saveTpc',
                    #           'SimSaveParticleHistory/saveHistory',
                    #           'SimSaveTrajectory/saveTrajectory',
                    #           ],
                    eventProvider = pgun,
                    OutputLevel = DEBUG)

# output to root file
from Configurables import PodioLegacyOutput
out = PodioLegacyOutput('out')
out.filename = 'megat.gaudi.root'
out.outputCommands = ['keep *']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel = DEBUG)
