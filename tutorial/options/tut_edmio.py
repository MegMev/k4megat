from Gaudi.Configuration import *

# Data service
from Configurables import k4DataSvc
datasvc = k4DataSvc("EventDataSvc")
datasvc.input = "megat_tut.root"

# Fetch the collection into TES
from Configurables import PodioInput
inp = PodioInput()
inp.collections = ["CztHits"]

# Add algorithm
from Configurables import MgTut_EDMIO
hist = MgTut_EDMIO("histTest")
hist.CaloSimHits.Path = "CztHits"
hist.OffsetCaloSimHits.Path = "OffsetCztHits"
THistSvc().Output = ["tutorial DATAFILE='tut_edmio.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

# Select & Write the collections to disk ROOT file
from Configurables import PodioOutput
out = PodioOutput('out')
#### caveat: the filename must be different from HistSvc output file
out.filename = 'offset_megat_tut.root'
#### drop all but keep the offset collection
out.outputCommands = ['drop *', 'keep OffsetCztHits']
# out.outputCommands = ['keep *', 'drop CztHits']

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [inp, hist, out],
                EvtSel = 'NONE',
                EvtMax   = -1,
                ExtSvc = [datasvc],
                OutputLevel=INFO
 )

