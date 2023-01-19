from Gaudi.Configuration import *
import GaudiKernel.SystemOfUnits as units

# TES
from Configurables import k4DataSvc
podioevent = k4DataSvc("EventDataSvc")

# geometry service
### Configures the detector construction: geometry and sd
from Configurables import GeoSvc
from os import environ, path
detector_path = environ.get("MEGAT_ROOT", "")
geoservice = GeoSvc("GeoSvc", detectors=[path.join(detector_path, 'geometry/compact/Megat.xml')],
                    sensitiveTypes={'tracker':'SimpleTpcSD','calorimeter':'AggregateCalorimeterSD'},
                    OutputLevel = WARNING)

# region & limits
# from Configurables import SimG4UserLimitRegion
# regionTpc = SimG4UserLimitRegion("TpcLimits")
# regionTpc.volumeNames = ["Tracker"]
# regionTpc.maxStep = 2*units.mm
### need to append step limiter to the main physics list
from Configurables import SimG4UserLimitPhysicsList
physicslist = SimG4UserLimitPhysicsList("Physics")
physicslist.fullphysics = "SimG4FtfpBert"

# user actions
from Configurables import SimG4FullSimActions
actions = SimG4FullSimActions()
actions.enableHistory=True # toggle, default: false and no action is binded
actions.energyCut= 0.1 # min kinetic e the generated track to be saved (in MeV)

# g4 service
### Configures the Geant simulation: detector building, fields, regions, physics, actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")
geantservice.detector='SimG4DD4hepDetector'
geantservice.physicslist=physicslist
# geantservice.regions=[regionTpc]
geantservice.actions= actions
geantservice.g4PostInitCommands += ["/run/setCut 0.1 mm"]
# geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]
### Take rndm seed under control to have reproducible output
geantservice.randomNumbersFromGaudi = False
geantservice.seedValue = 4242

# g4 algorithm
### Configures event provider (i.e. generator) and output collections via tools
##### generator (in g4 unit)
from Configurables import SimG4SingleParticleGeneratorTool
pgun=SimG4SingleParticleGeneratorTool('ParticleGun', saveEdm=True,
                                      particleName = "mu-", energyMin = 10000, energyMax = 10000,
                                      etaMin = -4, etaMax = 4, phiMin = -3.14, phiMax = 3.14,
                                      OutputLevel = DEBUG)

##### output collections
from Configurables import SimG4SaveCalHits
saveCalo = SimG4SaveCalHits('saveCalo',readoutNames = ['CztHits'])
saveCalo.CaloHits.Path = 'CztHits'

from Configurables import SimG4SaveTrackerHits
saveTpc = SimG4SaveTrackerHits('saveTpc',readoutNames = ['TpcHits'])
saveTpc.SimTrackHits.Path = 'TpcHits'

##### save trajectory and history
# from Configurables import SimG4SaveTrajectory
# savetrajectorytool = SimG4SaveTrajectory("saveTrajectory")
# savetrajectorytool.TrajectoryPoints.Path = "trajectoryPoints"

from Configurables import SimG4SaveParticleHistory
savehisttool = SimG4SaveParticleHistory("saveHistory")
savehisttool.GenParticles.Path = "SimParticles"

##### finally the alg itself
from Configurables import SimG4Alg
geantsim = SimG4Alg('SimG4Alg',
                    outputs= ['SimG4SaveCalHits/saveCalo',
                              'SimG4SaveTrackerHits/saveTpc',
                              'SimG4SaveParticleHistory/saveHistory'
                              ],
                    eventProvider = pgun,
                    OutputLevel = DEBUG)

# output to root file
from Configurables import PodioOutput
out = PodioOutput('out')
out.filename = 'megat.gaudi.root'
out.outputCommands = ['keep *']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax = 10,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel = DEBUG)
