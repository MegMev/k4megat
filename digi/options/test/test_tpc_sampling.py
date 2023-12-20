from Gaudi.Configuration import *

# Data service
from Configurables import k4LegacyDataSvc
datasvc = k4LegacyDataSvc("EventDataSvc")
datasvc.input = 'tpcseg_megat.root'

# Fetch the collection into TES
from Configurables import PodioInput
inp = PodioInput()
inp.collections = ["TpcSegStripHits", "TpcDriftHits"]

# Add algorithm
from Configurables import TpcSamplingAlg
tpcSamplingAlg = TpcSamplingAlg("TpcSampling")
tpcSamplingAlg.inHits.Path = "TpcSegStripHits"
tpcSamplingAlg.simHits.Path = "TpcDriftHits"
tpcSamplingAlg.outHits.Path = "TpcDiagonalStripHits"
tpcSamplingAlg.sample_interval = 5 # ns
tpcSamplingAlg.shape_time = 5 # us
tpcSamplingAlg.nr_points = 512
tpcSamplingAlg.gain = 100
tpcSamplingAlg.amplitude_offset = 1000

# Select & Write the collections to disk ROOT file
from Configurables import PodioOutput
out = PodioOutput()
out.filename = 'tpcsample_megat.root'
out.outputCommands = ['keep *']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [inp, tpcSamplingAlg, out],
                EvtSel = 'NONE',
                EvtMax   = -1,
                ExtSvc = [datasvc],
                OutputLevel=INFO
 )

