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
dataSvc.input = "comptondigi_G4sim_pixel_Ar_5atm_1MeV.root"
# dataSvc.input = "compton_sim.root"
appMgr.ExtSvc += [dataSvc]

################################# Algorithms ########################################

from Configurables import PodioInput
inputAlg = PodioInput()
inputAlg.collections = ["TpcHits"]
appMgr.TopAlg += [inputAlg]

from Configurables import ClusteringInterpolationAlg
CIAlg = ClusteringInterpolationAlg("ClusteringInterpolationAlg")
CIAlg.inHits.Path = "TpcHits"
CIAlg.outHits.Path = "TpcCIHits"
appMgr.TopAlg += [CIAlg]

from Configurables import KalmanFilterDemoAlg
kfAlg = KalmanFilterDemoAlg("KalmanFilterDemoAlg")
kfAlg.inHits.Path = "TpcCIHits"
# kfAlg.digiHits.Path = "TpcHits"
kfAlg.outHits.Path = "TpcKfHits"
kfAlg.error_x = 0.5
kfAlg.error_y = 0.5
kfAlg.error_z = 0.5
appMgr.TopAlg += [kfAlg]

from Configurables import PodioOutput
outAlg = PodioOutput('outAlg')
outAlg.filename = 'cikftest_K10_4.root'
outAlg.outputCommands = ['drop *',
                         'keep TpcCIHits',
                         'keep TpcKfHits'
                        #  'keep CztKfHits'
                         ]
appMgr.TopAlg += [outAlg]

