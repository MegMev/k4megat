######################################################################################
###
### Steering file for ddsim simulation engine (batch mode)
###
### Usage: ddsim --steerFile batch_config.py
###
### Note:
### - ddsim uses DD4hepSimulation underneath, which can not be used for MT simulation
### - DD4hepSimulation uses key/value config setting to customized the run engine
### - only one action and one hit collection allowed for each subdetector in DD4hepSimulation
### - Sensitive detectors are categorized into two groups: tracker and calorimeter
### - sensitive actions are associated with sensitive detector based on two factors:
###   - type string : the default behavior
###     - types are hardcoded in detelement implementation
###     - there are two types automatically recognized by default:
###       - 'tracker' and 'calorimeter', this is a convention used in all dd4hep packages
###     - actions are specified by action.calo/action.tracker
###     - customized types can be added by sepcifying action.calorimetreSDTypes/action.trackerSDTypes
###   - name pattern : customized by user by specifying action.mapActions
### - step filter/filters can be assigned to each sensitive detector
######################################################################################

import os
from DDSim.DD4hepSimulation import DD4hepSimulation
from g4units import mm, GeV, MeV, deg

## The running engine
SIM = DD4hepSimulation()

## Verbosity use integers from 1(most) to 7(least) verbose
## or strings: VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS
SIM.printLevel = 3

## The type of action to do in this invocation
## batch: just simulate some events, needs numberOfEvents, and input file or gun
## vis: enable visualisation, run the macroFile if it is set
## qt: enable visualisation in Qt shell, run the macroFile if it is set
## run: run the macroFile and exit
## shell: enable interactive session
SIM.runType = "batch"

## The compact XML file, or multiple compact files, if the last one is the closer.
install_dir = os.environ['MEGAT_ROOT']
SIM.compactFile = [str("file:" + install_dir + "/geometry/compact/Megat.xml")]
## Outputfile from the simulation: .slcio, edm4hep.root and .root output files are supported
SIM.outputFile = "megat.edm4hep.root"

## number of events to simulate, used in batch mode
SIM.numberOfEvents = 1000
## Skip first N events when reading a file
SIM.skipNEvents = 0

## FourVector of translation for the Smearing of the Vertex position: x y z t
SIM.vertexOffset = [1.0, 1.0, 0.0, 0.0]
## FourVector of the Sigma for the Smearing of the Vertex position: x y z t
SIM.vertexSigma = [0.0, 0.0, 0.0, 0.0]


################################################################################
## Configuration for the DDG4 ParticleGun 
################################################################################
## Lorentz boost for the crossing angle, in radian!
SIM.enableGun = True

## The kinetic energy for the particle gun.
## If not None, it will overwrite the setting of momentumMin and momentumMax
SIM.gun.particle = "mu-"
SIM.gun.energy = 10 * GeV
SIM.gun.multiplicity = 1

##  direction of the particle gun, 3 vector 
SIM.gun.direction = (0, 0, -1)
SIM.gun.position = (0, 0, 0.0)

## choose the distribution of the random direction for theta
##
##     Options for random distributions:
##
##     'uniform' is the default distribution, flat in theta
##     'cos(theta)' is flat in cos(theta)
##     'eta', or 'pseudorapidity' is flat in pseudorapity
##     'ffbar' is distributed according to 1+cos^2(theta)
##
##     Setting a distribution will set isotrop = True
##
SIM.gun.distribution = 'uniform'

##  isotropic distribution for the particle gun
##
##     use the options phiMin, phiMax, thetaMin, and thetaMax to limit the range of randomly distributed directions
##     if one of these options is not None the random distribution will be set to True and cannot be turned off!
##
SIM.gun.isotrop = False

## Maximal momentum when using distribution (default = 0.0)
SIM.gun.momentumMin = 100 * MeV
SIM.gun.momentumMax = 1000 * GeV

SIM.gun.phiMax = 0 * deg
SIM.gun.phiMin = 360 * deg

SIM.gun.thetaMax = None
SIM.gun.thetaMin = None

################################################################################
## Helper holding sensitive detector actions.
##
##   The default tracker and calorimeter actions can be set with
##
##   >>> SIM = DD4hepSimulation()
##   >>> SIM.action.tracker=('Geant4TrackerWeightedAction', {'HitPositionCombination': 2, 'CollectSingleDeposits': False})
##   >>> SIM.action.calo = "Geant4CalorimeterAction"
##
##   The default sensitive actions for calorimeters and trackers are applied based on the sensitive type.
##   The list of sensitive types can be changed with
##
##   >>> SIM = DD4hepSimulation()
##   >>> SIM.action.trackerSDTypes = ['tracker', 'myTrackerSensType']
##   >>> SIM.calor.calorimeterSDTypes = ['calorimeter', 'myCaloSensType']
##
##   For specific subdetectors specific sensitive detectors can be set based on patterns in the name of the subdetector.
##
##   >>> SIM = DD4hepSimulation()
##   >>> SIM.action.mapActions['tpc'] = "TPCSDAction"
##
##   and additional parameters for the sensitive detectors can be set when the map is given a tuple
##
##   >>> SIM = DD4hepSimulation()
##   >>> SIM.action.mapActions['ecal'] =( "CaloPreShowerSDAction", {"FirstLayerNumber": 1} )
##
################################################################################

## List of sd types recognized as Calorimeter.
SIM.action.calorimeterSDTypes = ['calorimeter']
## The default calorimeter action.
SIM.action.calo = "Geant4CalorimeterAction"
# SIM.action.calo = "Geant4OpticalCalorimeterAction"
# SIM.action.calo = "Geant4ScintillatorCalorimeterAction"

## List of sd types recognized as Tracker.
SIM.action.trackerSDTypes = ['tracker']
##  The default tracker action
SIM.action.tracker = "Geant4TrackerAction"
# SIM.action.tracker = "Geant4OpticalTrackerAction"
# SIM.action.tracker = "Geant4TrackerCombineAction"

## Create a map of patterns and actions to be applied to sensitive detectors.
##
##     Example: if the name of the detector matches 'tpc' the TPCSDAction is used.
##
##       SIM.action.mapActions['tpc'] = "TPCSDAction"
SIM.action.mapActions = {}


################################################################################
## Configuration for sensitive detector filters
##
##   Set the default filter for 'tracker'
##   >>> SIM.filter.tracker = "edep1kev"
##   Use no filter for 'calorimeter' by default
##   >>> SIM.filter.calo = ""
##
##   Assign a filter to a sensitive detector via pattern matching
##   >>> SIM.filter.mapDetFilter['FTD'] = "edep1kev"
##
##   Or more than one filter:
##   >>> SIM.filter.mapDetFilter['FTD'] = ["edep1kev", "geantino"]
##
##   Don't use the default filter or anything else:
##   >>> SIM.filter.mapDetFilter['TPC'] = None ## or "" or []
##
##   Create a custom filter. The dictionary is used to instantiate the filter later on
##   >>> SIM.filter.filters['edep3kev'] = dict(name="EnergyDepositMinimumCut/3keV", parameter={"Cut": 3.0*keV} )
##
################################################################################

##  define a list of filter objects: map between name and parameter dictionary
##  The items can be used later for each sensitive detector
SIM.filter.filters = {'geantino': {'name': 'GeantinoRejectFilter/GeantinoRejector', 'parameter': {}}, 'edep1kev': {'name': 'EnergyDepositMinimumCut', 'parameter': {'Cut': 0.001}}, 'edep0': {'name': 'EnergyDepositMinimumCut/Cut0', 'parameter': {'Cut': 0.0}}}

##     default filter for all calorimeter/tracker sensitive detectors, single filter allowed;
##     this is applied if no other filter is used for a calorimeter/tracker
SIM.filter.calo = "edep0"
SIM.filter.tracker = "edep1kev"


##  a map between name patterns and filter objects, using subdetector name to attach filters to sensitive detector 
SIM.filter.mapDetFilter = {}


################################################################################
## Configuration for the Particle Handler/ MCTruth treatment 
################################################################################

## Enable lots of printout on simulated hits and MC-truth information
SIM.part.enableDetailedHitsAndParticleInfo = False

##  Keep all created particles
SIM.part.keepAllParticles = False

## Minimal distance between particle vertex and endpoint of parent after
##     which the vertexIsNotEndpointOfParent flag is set
##     
SIM.part.minDistToParentVertex = 2.2e-14

## MinimalKineticEnergy to store particles created in the tracking region
SIM.part.minimalKineticEnergy = 1.0 * MeV

##  Printout at End of Tracking 
SIM.part.printEndTracking = False

##  Printout at Start of Tracking 
SIM.part.printStartTracking = False

## List of processes to save, on command line give as whitespace separated string in quotation marks
SIM.part.saveProcesses = ['Decay']

## Optionally enable an extended Particle Handler
SIM.part.userParticleHandler = ""


################################################################################
## Configuration for the PhysicsList 
################################################################################

## If true, add decay processes for all particles.
## 
##     Only enable when creating a physics list not based on an existing Geant4 list!
##     
SIM.physics.decays = False

## The name of the Geant4 Physics list.
SIM.physics.list = "FTFP_BERT"

##  location of particle.tbl file containing extra particles and their lifetime information
## 
##     For example in $DD4HEP/examples/DDG4/examples/particle.tbl
##     
SIM.physics.pdgfile = None

##  The global geant4 rangecut for secondary production
## 
##     Default is 0.7 mm as is the case in geant4 10
## 
##     To disable this plugin and be absolutely sure to use the Geant4 default range cut use "None"
## 
##     Set printlevel to DEBUG to see a printout of all range cuts,
##     but this only works if range cut is not "None"
##     
SIM.physics.rangecut = 0.7

## Set of PDG IDs that will not be passed from the input record to Geant4.
## 
##     Quarks, gluons and W's Z's etc should not be treated by Geant4
##     
SIM.physics.rejectPDGs = {3201, 1, 3203, 2, 4101, 3, 4103, 4, 5, 6, 21, 23, 24, 5401, 25, 2203, 5403, 3101, 3103, 4403, 2101, 5301, 2103, 5303, 4301, 1103, 4303, 5201, 5203, 3303, 4201, 4203, 5101, 5103, 5503}

## Set of PDG IDs for particles that should not be passed to Geant4 if their properTime is 0.
## 
##     The properTime of 0 indicates a documentation to add FSR to a lepton for example.
##     
SIM.physics.zeroTimePDGs = {17, 11, 13, 15}


################################################################################
## Properties for the random number generator 
################################################################################

## If True, calculate random seed for each event basedon eventID and runID
## Allows reproducibility even whenSkippingEvents
SIM.random.file = None
SIM.random.luxury = 1
# replace gRandom with CLHEP engine
SIM.random.replace_gRandom = True
# long int as seed (may need it to reproduce simulation results)
SIM.random.seed = None
# CLHEP engineName
SIM.random.type = None
# not used in implementation
SIM.random.enableEventSeed = False


################################################################################
## Configuration for the output levels of DDG4 components 
################################################################################

## Output level for input sources
SIM.output.inputStage = 3

## Output level for Geant4 kernel
SIM.output.kernel = 3

## Output level for ParticleHandler
SIM.output.part = 3

## Output level for Random Number Generator setup
SIM.output.random = 6


################################################################################
## Configuration for Output Files. 
################################################################################

## Set a function to configure the outputFile.
##
##     The function must take a ``DD4hepSimulation`` object as its only argument and return ``None``.
##
##     For example one can add this to the ddsim steering file:
##
##       def exampleUserPlugin(dd4hepSimulation):
##         '''Example code for user created plugin.
##
##         :param DD4hepSimulation dd4hepSimulation: The DD4hepSimulation instance, so all parameters can be accessed
##         :return: None
##         '''
##         from DDG4 import EventAction, Kernel
##         dd = dd4hepSimulation  # just shorter variable name
##         evt_root = EventAction(Kernel(), 'Geant4Output2ROOT/' + dd.outputFile, True)
##         evt_root.HandleMCTruth = True or False
##         evt_root.Control = True
##         if not dd.outputFile.endswith(dd.outputConfig.myExtension):
##           output = dd.outputFile + dd.outputConfig.myExtension
##         evt_root.Output = output
##         evt_root.enableUI()
##         Kernel().eventAction().add(evt_root)
##         return None
##
##       SIM.outputConfig.userOutputPlugin = exampleUserPlugin
##       # arbitrary options can be created and set via the steering file or command line
##       SIM.outputConfig.myExtension = '.csv'
SIM.outputConfig.userOutputPlugin = None



################################################################################
## Configuration for the magnetic field (stepper)
################################################################################
# SIM.field.delta_chord = 0.25
# SIM.field.delta_intersection = 0.001
# SIM.field.delta_one_step = 0.01
# SIM.field.eps_max = 0.001
# SIM.field.eps_min = 5e-05
# SIM.field.equation = "Mag_UsualEqRhs"
# SIM.field.largest_step = 10000.0
# SIM.field.min_chord_step = 0.01
# SIM.field.stepper = "ClassicalRK4"

