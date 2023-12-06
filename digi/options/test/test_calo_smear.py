from Gaudi.Configuration import *

# Data service
from Configurables import k4LegacyDataSvc
datasvc = k4LegacyDataSvc("EventDataSvc")
datasvc.input = 'megat_tut.root'
# datasvc.ForceLeaves= True

# Fetch the collection into TES
from Configurables import PodioInput
inp = PodioInput()
inp.collections = ["CztHits"]

# Add algorithm
from Configurables import CaloSimpleSmearAlg
calosmeared = CaloSimpleSmearAlg("CaloSmear")
calosmeared.inHits.Path = "CztHits"
calosmeared.outHits.Path = "CztSmearedHits"
calosmeared.energy_sigma = 100 # keV

# Select & Write the collections to disk ROOT file
from Configurables import PodioOutput
out = PodioOutput()
out.filename = 'calosmear_megat_tut.root'
out.outputCommands = ['keep *']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [inp, calosmeared, out],
                EvtSel = 'NONE',
                EvtMax   = -1,
                ExtSvc = [datasvc],
                OutputLevel=INFO
 )

