#include "DD4hep/Detector.h"
#include "DDG4/Factories.h"

#include "SimSensitive/AggregateCalorimeterSD.h"
#include "SimSensitive/BirksLawCalorimeterSD.h"
#include "SimSensitive/FullParticleAbsorptionSD.h"
#include "SimSensitive/GflashCalorimeterSD.h"
#include "SimSensitive/SimpleCalorimeterSD.h"
#include "SimSensitive/SimpleDriftChamber.h"
#include "SimSensitive/SimpleTrackerSD.h"

namespace megat {
  namespace sim {

    // All G4VSensitiveDetector are deleted by ~G4SDManager called by ~G4RunManagerKernel
    // Factory method to create an instance of SimpleTrackerSD
    static G4VSensitiveDetector* create_simple_tracker_sd( const std::string& aDetectorName, dd4hep::Detector& aLcdd ) {
      std::string readoutName = aLcdd.sensitiveDetector( aDetectorName ).readout().name();
      return new SimpleTrackerSD( aDetectorName, readoutName,
                                  aLcdd.sensitiveDetector( aDetectorName ).readout().segmentation() );
    }
    // Factory method to create an instance of SimpleCalorimeterSD
    static G4VSensitiveDetector* create_simple_calorimeter_sd( const std::string& aDetectorName,
                                                               dd4hep::Detector&  aLcdd ) {
      std::string readoutName = aLcdd.sensitiveDetector( aDetectorName ).readout().name();
      return new SimpleCalorimeterSD( aDetectorName, readoutName,
                                      aLcdd.sensitiveDetector( aDetectorName ).readout().segmentation() );
    }
    // Factory method to create an instance of SimpleCalorimeterSD with Birks law
    static G4VSensitiveDetector* create_birks_law_calorimeter_sd( const std::string& aDetectorName,
                                                                  dd4hep::Detector&  aLcdd ) {
      std::string readoutName = aLcdd.sensitiveDetector( aDetectorName ).readout().name();
      return new BirksLawCalorimeterSD( aDetectorName, readoutName,
                                        aLcdd.sensitiveDetector( aDetectorName ).readout().segmentation() );
    }
    // Factory method to create an instance of AggregateCalorimeterSD
    static G4VSensitiveDetector* create_aggregate_calorimeter_sd( const std::string& aDetectorName,
                                                                  dd4hep::Detector&  aLcdd ) {
      std::string readoutName = aLcdd.sensitiveDetector( aDetectorName ).readout().name();
      return new AggregateCalorimeterSD( aDetectorName, readoutName,
                                         aLcdd.sensitiveDetector( aDetectorName ).readout().segmentation() );
    }
    // Factory method to create an instance of GflashCalorimeterSD
    static G4VSensitiveDetector* create_gflash_calorimeter_sd( const std::string& aDetectorName,
                                                               dd4hep::Detector&  aLcdd ) {
      std::string readoutName = aLcdd.sensitiveDetector( aDetectorName ).readout().name();
      return new GflashCalorimeterSD( aDetectorName, readoutName,
                                      aLcdd.sensitiveDetector( aDetectorName ).readout().segmentation() );
    }
    // Factory method to create an instance of FullParticleAbsorptionSD
    static G4VSensitiveDetector* create_full_particle_absorbtion_sd( const std::string& aDetectorName,
                                                                     dd4hep::Detector&  aLcdd ) {
      std::string readoutName = aLcdd.sensitiveDetector( aDetectorName ).readout().name();
      return new FullParticleAbsorptionSD( aDetectorName, readoutName,
                                           aLcdd.sensitiveDetector( aDetectorName ).readout().segmentation() );
    }
    // Factory method to create an instance of SimpleDriftChamber
    static G4VSensitiveDetector* create_simple_driftchamber( const std::string& aDetectorName,
                                                             dd4hep::Detector&  aLcdd ) {
      std::string readoutName = aLcdd.sensitiveDetector( aDetectorName ).readout().name();
      return new SimpleDriftChamber( aDetectorName, readoutName,
                                     aLcdd.sensitiveDetector( aDetectorName ).readout().segmentation() );
    }

  } // namespace sim
} // namespace megat

DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR( MegatSimpleTrackerSD, megat::sim::create_simple_tracker_sd )
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR( MegatSimpleCalorimeterSD, megat::sim::create_simple_calorimeter_sd )
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR( MegatBirksLawCalorimeterSD, megat::sim::create_birks_law_calorimeter_sd )
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR( MegatAggregateCalorimeterSD, megat::sim::create_aggregate_calorimeter_sd )
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR( MegatGflashCalorimeterSD, megat::sim::create_gflash_calorimeter_sd )
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR( MegatFullParticleAbsorptionSD,
                                          megat::sim::create_full_particle_absorbtion_sd )
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR( MegatSimpleDriftChamber, megat::sim::create_simple_driftchamber )
