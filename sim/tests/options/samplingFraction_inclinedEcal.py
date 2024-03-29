from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_calibration.xml'
],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimSvc
geantservice = SimSvc("SimSvc", detector='SimDD4hepDetector', physicslist="SimFtfpBert", actions="SimFullSimActions")
geantservice.g4PostInitCommands += ["/run/setCut 0.1 mm"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# and a tool that saves the calorimeter hits
from Configurables import SimAlg, SimSaveCalHits, SimSingleParticleGeneratorTool
saveecaltool = SimSaveCalHits("saveECalBarrelHits",readoutName = ["ECalBarrelEta"])
saveecaltool.positionedCaloHits.Path = "ECalBarrelPositionedHits"
saveecaltool.caloHits.Path = "ECalBarrelHits"
from Configurables import SimSingleParticleGeneratorTool
pgun=SimSingleParticleGeneratorTool("SimSingleParticleGeneratorTool",saveEdm=True,
                                      particleName = "e-", energyMin = 50000, energyMax = 50000, etaMin = 0, etaMax = 0,
                                      OutputLevel = DEBUG)
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = SimAlg("SimAlg",
                    outputs= ["SimSaveCalHits/saveECalBarrelHits"],
                    eventProvider = pgun,
                    OutputLevel = DEBUG)

from Configurables import SamplingFractionInLayers
hist = SamplingFractionInLayers("hists",
                                 energyAxis = 50,
                                 readoutName = "ECalBarrelEta",
                                 layerFieldName = "layer",
                                 activeFieldName = "type",
                                 activeFieldValue = 0,
                                 numLayers = 8,
                                 OutputLevel = INFO)
hist.deposits.Path="ECalBarrelPositionedHits"

THistSvc().Output = ["rec DATAFILE='histSF_inclined_e50GeV_eta0_1events.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=False
THistSvc().OutputLevel=INFO

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
hist.AuditExecute = True

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, hist],
                EvtSel = 'NONE',
                EvtMax = 10,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice, audsvc],
                OutputLevel = DEBUG
)
