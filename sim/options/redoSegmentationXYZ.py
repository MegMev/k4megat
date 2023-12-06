from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCDumper

from Configurables import HepMCFileReader, GenAlg
readertool = HepMCFileReader("ReaderTool", Filename="Test/TestGeometry/data/testHepMCrandom.dat")
reader = GenAlg("Reader", SignalProvider=readertool)
reader.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

dumper = HepMCDumper("Dumper")
dumper.hepmc.Path="hepmc"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Test/TestGeometry/data/TestBoxCaloSD_3readouts.xml'], OutputLevel = INFO)

from Configurables import SimSvc
geantservice = SimSvc("SimSvc", physicslist='SimTestPhysicsList')

from Configurables import SimAlg, SimSaveCalHits, InspectHitsCollectionsTool
inspecttool = InspectHitsCollectionsTool("inspect", readoutNames=["ECalHits"], OutputLevel = DEBUG)
savecaltool = SimSaveCalHits("saveECalHits", readoutName = ["ECalHits"], OutputLevel = DEBUG)
savecaltool.positionedCaloHits.Path = "positionedCaloHits"
savecaltool.caloHits.Path = "caloHits"
geantsim = SimAlg("SimAlg", outputs= ["SimSaveCalHits/saveECalHits","InspectHitsCollectionsTool/inspect"])

from Configurables import RedoSegmentation
resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
                             oldReadoutName = "ECalHits",
                             # specify which fields are going to be deleted
                             oldSegmentationIds = ["x","y","z"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName="ECalHitsReverseOrder",
                             OutputLevel = DEBUG)
# clusters are needed, with deposit position and cellID in bits
resegment.inhits.Path = "positionedCaloHits"
resegment.outhits.Path = "newCaloHits"

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="testResegmentationXYZ.root")
out.outputCommands = ["keep *"]

ApplicationMgr(EvtSel='NONE',
               EvtMax=30,
               TopAlg=[reader, hepmc_converter, geantsim, resegment,
                out],
               ExtSvc = [podiosvc, geoservice, geantservice],
               OutputLevel=DEBUG)
