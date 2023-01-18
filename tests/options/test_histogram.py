from Gaudi.Configuration import *

# Data service
from Configurables import k4DataSvc
podioevent = k4DataSvc("EventDataSvc")
podioevent.input = "megat.edm4hep.root"

# Input
from Configurables import PodioInput
inp = PodioInput()
inp.collections = ["CztHits"]

# histogram
from Configurables import MegatTest_Histogram
hist = MegatTest_Histogram("histTest")
hist.caloSimHits.Path = "CztHits"
THistSvc().Output = ["sim DATAFILE='histSimple.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [inp, hist],
                EvtSel = 'NONE',
                EvtMax   = 1000,
                ExtSvc = [podioevent],
                OutputLevel=INFO
 )

