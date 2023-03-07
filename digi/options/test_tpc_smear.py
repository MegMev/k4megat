from Gaudi.Configuration import *

# Data service
from Configurables import k4DataSvc
datasvc = k4DataSvc("EventDataSvc")
datasvc.input = 'tpcseg_megat_tut.root'
# datasvc.ForceLeaves= True

# Fetch the collection into TES
from Configurables import PodioInput
inp = PodioInput()
inp.collections = ["TpcPixelHits", "TpcStripHits"]

# Add algorithm
from Configurables import TpcSimpleSmearAlg
tpcpixelsmeared = TpcSimpleSmearAlg("TpcPixelSmear")
tpcpixelsmeared.inHits.Path = "TpcPixelHits"
tpcpixelsmeared.outHits.Path = "TpcPixelSmearedHits"
tpcpixelsmeared.energy_sigma = 10 # eV
tpcpixelsmeared.time_sigma = 100 # ps


tpcstripsmeared = TpcSimpleSmearAlg("TpcStripSmear")
tpcstripsmeared.inHits.Path = "TpcStripHits"
tpcstripsmeared.outHits.Path = "TpcStripSmearedHits"

# Select & Write the collections to disk ROOT file
from Configurables import PodioOutput
out = PodioOutput('out')
out.filename = 'tpcsmear_megat_tut.root'
out.outputCommands = ['keep *']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [inp, tpcstripsmeared,tpcpixelsmeared, out],
                EvtSel = 'NONE',
                EvtMax   = -1,
                ExtSvc = [datasvc],
                OutputLevel=INFO
 )

