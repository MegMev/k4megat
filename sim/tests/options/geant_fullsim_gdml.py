

### \file
### \ingroup SimulationTests
### | **input (alg)**                 | other algorithms                   |                                                           |                          |                                    | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | --------------------------------------------------------- | ------------------------ | ---------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry taken from GDML (no sensitive detectors!) | FTFP_BERT physics list   | no action initialization list      | write the EDM output to ROOT file using PODIO   |


from Gaudi.Configuration import *

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")

# Get lists of algorithms and services that provide input to the simulation
from common_config import mc_particle_algs, mc_particle_svcs

from Configurables import SimSvc, SimGdmlDetector
## Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
# first create tool that builds geometry from GDML
det = SimGdmlDetector("SimGdmlDetector", gdml = "Sim/SimKernel/gdml/example.xml")
geantservice = SimSvc("SimSvc", detector=det, physicslist="SimFtfpBert", actions="SimFullSimActions")

from Configurables import SimAlg, SimPrimariesFromEdmTool
## Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4
particle_converter = SimPrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimAlg("SimAlg",
                    eventProvider=particle_converter)

from Configurables import PodioLegacyOutput
out = PodioLegacyOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=mc_particle_algs + [geantsim, out],
                EvtSel='NONE',
                EvtMax=1,
                ## order is important, as GeoSvc is needed by SimSvc
                ExtSvc=mc_particle_svcs + [podioevent, geantservice],
                OutputLevel=DEBUG)
