from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import GenAlg, MomentumRangeParticleGun
pgun = MomentumRangeParticleGun("PGun",
                                PdgCodes=[11], # electron
                                MomentumMin = 10, # GeV
                                MomentumMax = 10, # GeV
                                ThetaMin = 1.57, # rad
                                ThetaMax = 1.57, # rad
                                PhiMin = 0, # rad
                                PhiMax = 3.14) # rad
gen = GenAlg("ParticleGun", SignalProvider=pgun, VertexSmearingTool="FlatSmearVertex")
gen.hepmc.Path = "hepmc"

# reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:../../../Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         'file:../../../Detector/DetFCChhBaseline1/compact/FCChh_TrackerAir.xml'])

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimSvc, SimFastSimPhysicsList, SimFastSimTrackerRegion
## create region and a parametrisation model with a default smearing (sigma=const=0.01)
regiontool = SimFastSimTrackerRegion("model", volumeNames=["TrackerEnvelopeBarrel"])
# create overlay on top of FTFP_BERT physics list, attaching fast sim/parametrization process
physicslisttool = SimFastSimPhysicsList("Physics", fullphysics="SimFtfpBert")
# attach those tools to the G4 service
geantservice = SimSvc("SimSvc", physicslist=physicslisttool, regions=["SimFastSimTrackerRegion/model"])

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimAlg, SimSaveSmearedParticles, SimPrimariesFromEdmTool
# first, create a tool that saves the smeared particles
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimSaveSmearedParticles")
# and YY is the given name ("saveSmearedParticles")
saveparticlestool = SimSaveSmearedParticles("saveSmearedParticles")
saveparticlestool.particlesMCparticles.Path = "particleMCparticleAssociation"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimPrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimAlg("SimAlg",
                    outputs = ["SimSaveSmearedParticles/saveSmearedParticles"],
                    eventProvider=particle_converter)

from Configurables import SimFastSimHistograms
hist = SimFastSimHistograms("fastHist")
hist.particlesMCparticles.Path = "particleMCparticleAssociation"
THistSvc().Output = ["rec DATAFILE='histSimple.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out", filename = "out_fast_simple.root")
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, geantsim, hist, out],
                EvtSel = 'NONE',
                EvtMax   = 1000,
                # order is important, as GeoSvc is needed by SimSvc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=INFO
 )
