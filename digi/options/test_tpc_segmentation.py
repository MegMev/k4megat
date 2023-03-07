from Gaudi.Configuration import *

# geometry service
from Configurables import MegatGeoSvc as GeoSvc
from os import environ, path
detector_path = environ.get("MEGAT_ROOT", "")
geoservice = GeoSvc("MegatGeoSvc",
                    # buildType="BUILD_RECO",
                    buildType="BUILD_SIMU",
                    detectors=[path.join(detector_path, 'geometry/compact/Megat.xml'),
                                              './tpc_seg_test.xml'],
                    OutputLevel = WARNING)

# Data service
from Configurables import k4DataSvc
datasvc = k4DataSvc("EventDataSvc")
datasvc.input = 'tpcdrift_megat_tut.root'
# datasvc.ForceLeaves= True

# Fetch the collection into TES
from Configurables import PodioInput
inp = PodioInput()
inp.collections = ["TpcDriftHits"]

# Add algorithm
from Configurables import TpcSegmentAlg
tpcpixelseg = TpcSegmentAlg("TpcPixelSeg")
tpcpixelseg.inHits.Path = "TpcDriftHits"
# tpcpixelseg.outHits.Path = "TpcPixelHits"
tpcpixelseg.readoutName = "TpcPixelHits"

tpcstripseg = TpcSegmentAlg("TpcStripSeg")
tpcstripseg.inHits.Path = "TpcDriftHits"
# tpcstripseg.outHits.Path = "TpcStripHits"
tpcstripseg.readoutName = "TpcStripHits"

# Select & Write the collections to disk ROOT file
from Configurables import PodioOutput
out = PodioOutput('out')
out.filename = 'tpcseg_megat_tut.root'
out.outputCommands = ['keep *']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [inp, tpcstripseg,tpcpixelseg, out],
                EvtSel = 'NONE',
                EvtMax   = -1,
                ExtSvc = [datasvc],
                OutputLevel=INFO
 )

