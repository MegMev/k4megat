from Gaudi.Configuration import *

# geometry service
from Configurables import MegatGeoSvc as GeoSvc
from os import environ, path
detector_path = environ.get("MEGAT_ROOT", "")
geoservice = GeoSvc("MegatGeoSvc",
                    buildType="BUILD_SIMU",
                    detectors=[path.join(detector_path, 'geometry/compact/Megat.xml'),
                               ],
                    OutputLevel = WARNING)

# Data service
from Configurables import k4DataSvc
datasvc = k4DataSvc("EventDataSvc")
datasvc.input = "megat_tut.root"
# datasvc.ForceLeaves= True

# Fetch the collection into TES
from Configurables import PodioInput
inp = PodioInput()
inp.collections = ["TpcHits"]

# Add algorithm
from Configurables import TpcDriftAlg
tpcdriftalg = TpcDriftAlg("TpcDriftAlg")
tpcdriftalg.maxHits = 500
tpcdriftalg.usePoisson= True
tpcdriftalg.wvalue= 25
tpcdriftalg.trans_diffusion_const = 200
tpcdriftalg.long_diffusion_const = 250
tpcdriftalg.drift_velocity = 6
# tpcdriftalg.attach_factor = 0.1
tpcdriftalg.inHits.Path = "TpcHits"
tpcdriftalg.outHits.Path = "TpcDriftHits"

# Select & Write the collections to disk ROOT file
from Configurables import PodioOutput
out = PodioOutput('out')
out.filename = 'tpcdrift_megat_tut.root'
out.outputCommands = ['keep *']

# from Configurables import HepRndm__Engine_CLHEP__RanluxEngine_
# rdmEngine = HepRndm__Engine_CLHEP__RanluxEngine_("RndmGenSvc.Engine") # The default engine in Gaudi
from Configurables import HepRndm__Engine_CLHEP__HepJamesRandom_
rdmEngine = HepRndm__Engine_CLHEP__HepJamesRandom_("RndmGenSvc.Engine") # The default engine in Geant4
rdmEngine.SetSingleton = True
rdmEngine.Seeds = [5685]

from Configurables import RndmGenSvc
rdmSvc = RndmGenSvc("RndmGenSvc")
rdmSvc.Engine = rdmEngine.name()

# or just change the engine type, but not other params like seeds
# rdmSvc.Engine = "HepRndm::Engine<CLHEP::HepJamesRandom>"

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [inp, tpcdriftalg, out],
                EvtSel = 'NONE',
                EvtMax   = -1,
                ExtSvc = [datasvc, rdmEngine, rdmSvc],
                OutputLevel=INFO
               )


