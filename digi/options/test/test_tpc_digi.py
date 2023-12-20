from Gaudi.Configuration import *

# ApplicationMgr
from Configurables import ApplicationMgr
appMgr = ApplicationMgr(
                EvtSel = 'NONE',
                EvtMax   = -1,
                OutputLevel=INFO
               )

################################# Servicec ########################################

# Geometry service
from Configurables import MegatGeoSvc as GeoSvc
from os import environ, path
detector_path = environ.get("MEGAT_ROOT", "")
geoSvc = GeoSvc("GeoSvc",
                buildType="BUILD_SIMU",
                detectors=[path.join(detector_path, 'geometry/compact/Megat.xml'),
                           './tpc_seg_test.xml'
                           ],
                OutputLevel = WARNING)
appMgr.ExtSvc += [geoSvc]

# Data service
from Configurables import k4LegacyDataSvc
dataSvc = k4LegacyDataSvc("EventDataSvc")
dataSvc.input = "megat.gaudi.root"
appMgr.ExtSvc += [dataSvc]

# Rndm service (use G4 default engine)
from Configurables import HepRndm__Engine_CLHEP__HepJamesRandom_
rdmEngine = HepRndm__Engine_CLHEP__HepJamesRandom_("RndmGenSvc.Engine")
rdmEngine.SetSingleton = True
rdmEngine.Seeds = [5685]

from Configurables import RndmGenSvc
rdmSvc = RndmGenSvc("RndmGenSvc")
rdmSvc.Engine = rdmEngine.name()
appMgr.ExtSvc += [rdmEngine, rdmSvc]

################################# Algorithms ########################################

# Fetch the collection into TES
from Configurables import PodioInput
inputAlg = PodioInput()
inputAlg.collections = ["TpcSimHits"]
appMgr.TopAlg += [inputAlg]

# 1. Electron drift to anode surface
from Configurables import TpcDriftAlg
driftAlg = TpcDriftAlg("TpcDriftAlg")
driftAlg.inHits.Path = "TpcSimHits"
driftAlg.outHits.Path = "TpcDriftHits"
# driftAlg.maxHits = 500
# driftAlg.usePoisson= True
# driftAlg.wvalue= 25
# driftAlg.trans_diffusion_const = 200
# driftAlg.long_diffusion_const = 250
# driftAlg.drift_velocity = 6
# driftAlg.attach_factor = 0.1
appMgr.TopAlg += [driftAlg]

# 2. Readout segmentation (pixel & strip)
from Configurables import TpcSegmentAlg
pixelSegAlg = TpcSegmentAlg("TpcPixelSeg")
pixelSegAlg.inHits.Path = "TpcDriftHits"
pixelSegAlg.outHits.Path = "TpcSegPixelHits"
pixelSegAlg.readoutName = "TpcPixelHits"

stripSegAlg = TpcSegmentAlg("TpcStripSeg")
stripSegAlg.inHits.Path = "TpcDriftHits"
stripSegAlg.outHits.Path = "TpcSegStripHits"
stripSegAlg.readoutName = "TpcDiagonalStripHits"
appMgr.TopAlg += [pixelSegAlg, stripSegAlg]

# 3. Simple smear (add a fixed-width Gaussian noise)
from Configurables import TpcSimpleSmearAlg
pixelSmearAlg = TpcSimpleSmearAlg("TpcPixelSmear")
pixelSmearAlg.inHits.Path = "TpcSegPixelHits"
pixelSmearAlg.outHits.Path = "TpcSmearPixelHits"
pixelSmearAlg.energy_sigma = 10 # eV
pixelSmearAlg.time_sigma = 100 # ps

stripSmearAlg = TpcSimpleSmearAlg("TpcStripSmear")
stripSmearAlg.inHits.Path = "TpcSegStripHits"
stripSmearAlg.outHits.Path = "TpcSmearStripHits"
stripSmearAlg.energy_sigma = 10 # eV
stripSmearAlg.time_sigma = 100 # ps
appMgr.TopAlg += [pixelSmearAlg, stripSmearAlg]

# 4. Signal formation and sampling
from Configurables import TpcSamplingAlg
stripSamplingAlg = TpcSamplingAlg("TpcStripSampling")
stripSamplingAlg.inHits.Path = "TpcSmearStripHits"
stripSamplingAlg.simHits.Path = "TpcDriftHits"
stripSamplingAlg.outHits.Path = "TpcDiagonalStripHits"
# stripSamplingAlg.sample_interval = 5 # ns
# stripSamplingAlg.shape_time = 5 # us
# stripSamplingAlg.nr_points = 512
# stripSamplingAlg.gain = 1000
# stripSamplingAlg.amplitude_offset = 100

pixelSamplingAlg = TpcSamplingAlg("TpcPixelSampling")
pixelSamplingAlg.inHits.Path = "TpcSmearPixelHits"
pixelSamplingAlg.simHits.Path = "TpcDriftHits"
pixelSamplingAlg.outHits.Path = "TpcPixelHits"
appMgr.TopAlg += [pixelSamplingAlg, stripSamplingAlg]

################################# Output ########################################

# Select & Write the collections to disk ROOT file
from Configurables import PodioOutput
outAlg = PodioOutput('outAlg')
outAlg.filename = 'tpcdigi_megat.root'
outAlg.outputCommands = ['drop *', 'keep TpcPixelHits', 'keep TpcDiagonalStripHits']
appMgr.TopAlg += [outAlg]

