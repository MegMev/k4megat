from Gaudi.Configuration import *

# geometry service
from Configurables import MegatGeoSvc as GeoSvc
from os import environ, path
detector_path = environ.get("MEGAT_ROOT", "")
geoservice = GeoSvc("MegatGeoSvc", detectors=[path.join(detector_path, 'geometry/compact/Megat.xml'),
                                              './tpc_seg_test.xml'],
                    OutputLevel = WARNING)

# Data service
from Configurables import k4DataSvc
datasvc = k4DataSvc("EventDataSvc")
datasvc.input = "megat_tut.root"
# datasvc.RootName = "/sim"
# datasvc.ForceLeaves= True

# Fetch the collection into TES
from Configurables import PodioInput
inp = PodioInput()
inp.collections = ["TpcHits"]

# Add algorithm
from Configurables import TpcTestDigi
tpcpixelseg = TpcTestDigi("TpcPixelSeg")
# tpcpixelseg.inHits.Path = "TpcHits"
# tpcpixelseg.outHits.Path = "TpcPixelHitsTest"
# tpcpixelseg.readoutName = "TpcStripHits"
# THistSvc().Output = ["tutorial DATAFILE='test_tpcseg.root' TYP='ROOT' OPT='RECREATE'"]
# THistSvc().PrintAll=True
# THistSvc().AutoSave=True
# THistSvc().AutoFlush=True
# THistSvc().OutputLevel=INFO

# Select & Write the collections to disk ROOT file
from Configurables import PodioOutput
out = PodioOutput('out')
#### caveat: the filename must be different from HistSvc output file
out.filename = 'tpcdigi_megat_tut.root'
#### drop all but keep the offset collection
out.outputCommands = ['keep *']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [inp, tpcpixelseg, out],
                EvtSel = 'NONE',
                EvtMax   = -1,
                ExtSvc = [datasvc],
                OutputLevel=INFO
 )

