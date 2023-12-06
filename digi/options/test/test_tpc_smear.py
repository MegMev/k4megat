from Gaudi.Configuration import *

# Data service
from Configurables import k4LegacyDataSvc
datasvc = k4LegacyDataSvc("EventDataSvc")
datasvc.input = 'tpcseg_megat.root'
# datasvc.ForceLeaves= True

# Fetch the collection into TES
from Configurables import PodioInput
inp = PodioInput()
inp.collections = ["TpcSegPixelHits", "TpcSegStripHits"]

# Add algorithm
from Configurables import TpcSimpleSmearAlg
tpcpixelsmeared = TpcSimpleSmearAlg("TpcPixelSmear")
tpcpixelsmeared.inHits.Path = "TpcSegPixelHits"
tpcpixelsmeared.outHits.Path = "TpcSmearPixelHits"
tpcpixelsmeared.energy_sigma = 10 # eV
tpcpixelsmeared.time_sigma = 100 # ps


tpcstripsmeared = TpcSimpleSmearAlg("TpcStripSmear")
tpcstripsmeared.inHits.Path = "TpcSegStripHits"
tpcstripsmeared.outHits.Path = "TpcSmearStripHits"

# Select & Write the collections to disk ROOT file
from Configurables import PodioOutput
out = PodioOutput()
out.filename = 'tpcsmear_megat.root'
out.outputCommands = ['keep *']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [inp, tpcstripsmeared,tpcpixelsmeared, out],
                EvtSel = 'NONE',
                EvtMax   = -1,
                ExtSvc = [datasvc],
                OutputLevel=INFO
 )

