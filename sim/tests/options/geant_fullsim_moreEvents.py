

### \file
### \ingroup SimulationTests
### | **input (alg)**                 | other algorithms                   |                                     |                          |                                    | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | ----------------------------------- | ------------------------ | ---------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry taken from TestTracker.xml | FTFP_BERT physics list   | save Tracker and HCAL hits         | write the EDM output to ROOT file using PODIO   |




from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCDumper, FCCDataSvc

podioevent = FCCDataSvc("EventDataSvc")

from Configurables import MomentumRangeParticleGun
from GaudiKernel import PhysicalConstants as constants
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 0 
guntool.ThetaMax = 2 * constants.pi 
guntool.PdgCodes = [11]
from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider=guntool
gen.hepmc.Path = "hepmc"


from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:../../../Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:../../../Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'],
                    OutputLevel = DEBUG)

from Configurables import SimSvc
geantservice = SimSvc("SimSvc", detector='SimDD4hepDetector', physicslist="SimFtfpBert", actions="SimFullSimActions", )

from Configurables import SimAlg, SimSaveTrackerHits, SimPrimariesFromEdmTool
savetrackertool = SimSaveTrackerHits("SimSaveTrackerHits", readoutNames=["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"

particle_converter = SimPrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimAlg("SimAlg",
                    outputs = ["SimSaveTrackerHits/SimSaveTrackerHits"],
                    eventProvider=particle_converter)

from Configurables import PodioOutput
out = PodioOutput("out",
                  filename = "test_geant_fullsim_moreEvents.root",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr( TopAlg=[gen, hepmc_converter, geantsim, out],
                EvtSel='NONE',
                EvtMax=10,
                ## order! geo needed by geant
                ExtSvc=[podioevent, geoservice, geantservice],
                OutputLevel=DEBUG)
