## mandatory
from Gaudi.Configuration import *

## configure AppMgr
from Configurables import ApplicationMgr
ApplicationMgr(
                EvtSel="NONE",
                EvtMax=1000, # event number
                OutputLevel=INFO,
                StopOnSignal=True,
                )

## configure algorithm
from Configurables import MgTut_RdmNumber
alg1 = MgTut_RdmNumber("Tut_RdmNumber")
ApplicationMgr().TopAlg += [alg1] # add algorithm
alg1.Mean = 10.
alg1.Sigma = 5.
alg1.Name = "hNewName"
alg1.Directory = "/dir1/dir2"

## configure hist service
THistSvc().Output = ["tutorial DATAFILE='tutorial.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO
ApplicationMgr().ExtSvc += [THistSvc()]
