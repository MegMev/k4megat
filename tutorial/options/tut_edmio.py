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
THistSvc().Output = ["tutorial DATAFILE='tut_edmio.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [inp, hist],
                EvtSel = 'NONE',
                EvtMax   = -1,
                ExtSvc = [datasvc],
                OutputLevel=INFO
 )

