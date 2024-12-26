# Input:
# TpcSimHits: output of simulation job option like compton_sim.py
#
# Output:
# TpcKfHits: kalman-filered TPC hits

from Gaudi.Configuration import *

# ApplicationMgr
from Configurables import ApplicationMgr
appMgr = ApplicationMgr(
                EvtSel = 'NONE',
                EvtMax   = 1000,
                OutputLevel=INFO
               )

################################# Servicec ########################################

# Data service
from Configurables import k4DataSvc
dataSvc = k4DataSvc("EventDataSvc")
dataSvc.input = "compton_sim.root"
appMgr.ExtSvc += [dataSvc]

################################# Algorithms ########################################

from Configurables import PodioInput
inputAlg = PodioInput()
inputAlg.collections = ["TpcSimHits","CaloSimHits"]
appMgr.TopAlg += [inputAlg]

#from Configurables import KalmanFilterDemoAlg
#kfAlg = KalmanFilterDemoAlg("KalmanFilterDemoAlg")
#kfAlg.inHits.Path = "TpcSimHits"
#kfAlg.outHits.Path = "TpcKfHits"
#kfAlg.error_x = 0.5
#kfAlg.error_y = 0.5
#kfAlg.error_z = 0.5
#appMgr.TopAlg += [kfAlg]

from Configurables import HitProcessorAlg
HitAlg = HitProcessorAlg("HitProcessorAlg")
HitAlg.inHits_Tpc.Path = "TpcSimHits"
HitAlg.inHits_Czt.Path = "CaloSimHits"
HitAlg.outHits_Tpc.Path = "TpcKfHits"
HitAlg.outHits_Czt.Path = "CztKfHits"
#HitAlg.error_x = 0.5
#HitAlg.error_y = 0.5
#HitAlg.error_z = 0.5
appMgr.TopAlg += [HitAlg]

from Configurables import PodioOutput
outAlg = PodioOutput('outAlg')
outAlg.filename = 'kftest.root'
outAlg.outputCommands = ['drop *',
                         'keep TpcKfHits',
                         'keep CztKfHits'
                         ]
appMgr.TopAlg += [outAlg]

