#
# Summary:
# Digitize the simulation hits: TPC for strip-based readout, Calo for pixel-based readout
#
# Input:
# The output of simulation job like test_sim.py
#
# Output:
# Three collections are saved:
# - GenParticles: primary particles
# - TpcHits: TPC strip-based digis
# - CaloHits: Calo pixel-based digis

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
geoSvc = GeoSvc("MegatGeoSvc",
                buildType="BUILD_SIMU",
                detectors=[path.join(detector_path, 'geometry/compact/Megat.xml'),
                           path.join(detector_path, 'geometry/compact/TPC_readout.xml')],
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
from Configurables import PodioLegacyInput
inputAlg = PodioLegacyInput()
inputAlg.collections = ["GenParticles", "TpcSimHits", "CaloSimHits"]
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

# 2. Readout segmentation
from Configurables import TpcSegmentAlg
tpcSegAlg = TpcSegmentAlg("TpcSegAlg")
tpcSegAlg.inHits.Path = "TpcDriftHits"
tpcSegAlg.outHits.Path = "TpcSegHits"
tpcSegAlg.readoutName = "TpcStripHits"
appMgr.TopAlg += [tpcSegAlg]

# 3. Simple smear (add a fixed-width Gaussian noise)
from Configurables import TpcSimpleSmearAlg
tpcSmearAlg = TpcSimpleSmearAlg("TpcSmearAlg")
tpcSmearAlg.inHits.Path = "TpcSegHits"
tpcSmearAlg.outHits.Path = "TpcHits"
tpcSmearAlg.energy_sigma = 10 # eV
tpcSmearAlg.time_sigma = 100 # ps
appMgr.TopAlg += [tpcSmearAlg]

# 4. CZT calo smearing (fixed-with gassian to edep)
from Configurables import CaloSimpleSmearAlg
caloSmearAlg = CaloSimpleSmearAlg("CaloSmearAlg")
caloSmearAlg.inHits.Path = "CaloSimHits"
caloSmearAlg.outHits.Path = "CaloHits"
caloSmearAlg.energy_sigma = 60 # keV
appMgr.TopAlg += [caloSmearAlg]

################################# Output ########################################

# Select & Write the collections to disk ROOT file
from Configurables import PodioLegacyOutput
outAlg = PodioLegacyOutput('outAlg')
outAlg.filename = 'simple_digi.root'
outAlg.outputCommands = ['drop *',
                         'keep GenParticles',
                         'keep TpcHits',
                         'keep CaloHits'
                         ]
appMgr.TopAlg += [outAlg]

