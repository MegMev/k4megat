

### \file
### \ingroup SimulationTests
### | **input (alg)**                 | other algorithms                   |                                                           |                          |                                    | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | --------------------------------------------------------- | ------------------------ | ---------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry taken from XML            | FTFP_BERT physics list   | save HCAL hits              | write the EDM output to ROOT file using PODIO   |

from Gaudi.Configuration import *

from Configurables import FCCDataSvc
## Data service
podioevent = FCCDataSvc("EventDataSvc")

# Get lists of algorithms and services that provide input to the simulation
from common_config import mc_particle_algs, mc_particle_svcs

from Configurables import SimSvc
## Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
geantservice = SimSvc("SimSvc", detector="SimDD4hepDetector", physicslist="SimFtfpBert", actions="SimFullSimActions")

from Configurables import GeoSvc
## DD4hep geometry service
# Parses the given xml file
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml' ],
                    OutputLevel = DEBUG)

from Configurables import SimAlg, SimSaveCalHits, SimPrimariesFromEdmTool
## Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# first, create a tool that saves the calorimeter hits (of type "hcal")
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimSaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savehcaltool = SimSaveCalHits("saveHCalHits", readoutName = ["HCalBarrelReadout"])
savehcaltool.positionedCaloHits.Path = "positionedCaloHits"
savehcaltool.caloHits.Path = "caloHits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimPrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimAlg("SimAlg",
                    outputs = ["SimSaveCalHits/saveHCalHits"],
                    eventProvider=particle_converter)

from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=mc_particle_algs + [geantsim, out],
                EvtSel='NONE',
                EvtMax=1,
                # order is important, as GeoSvc is needed by SimSvc
                ExtSvc=mc_particle_svcs + [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG)
