#ifndef SIMG4COMPONENTS_G4COMPTONEVENTGENERATORTOOL_H
#define SIMG4COMPONENTS_G4COMPTONEVENTGENERATORTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

// FCCSW
#include "SimInterface/ISimEventProviderTool.h"
#include "k4FWCore/DataHandle.h"

// Geant4
#include "G4SystemOfUnits.hh"

//
#include "SimKernel/Units.h"
#include <cmath>

// Forward declarations
// Geant4
class G4Event;
class G4PrimaryVertex;
class G4PrimaryParticle;
// datamodel
namespace edm4hep {
  class MCParticleCollection;
}

namespace megat {

  /** @class SimComptonEventGeneratorTool SimComptonEventGeneratorTool.h "SimComptonEventGeneratorTool.h"
   *
   *  Tool that generates ComptonEvent(contain the scattered photon and e-) with parameters set via options file.
   *
   *  adapted from @author Andrea Dell'Acqua, J. Lingemann
   *  2023/9/19 @date   2014-10-01
   */

  class SimComptonEventGeneratorTool : public GaudiTool, virtual public ISimEventProviderTool {
  public:
    /// Standard constructor
    SimComptonEventGeneratorTool( const std::string& type, const std::string& name, const IInterface* parent );

    virtual StatusCode initialize() final;

    /// Destructor
    virtual ~SimComptonEventGeneratorTool();

    /// Generates primaries using the parameters set via options file, uses CLHEP:RandFlat random number generator
    /// @returns G4Event with primaries generated through G4ParticleGun (ownership is transferred to the caller)
    virtual G4Event* g4Event() final;

  private:
    /// Saves primary vertex and particle to FCC EDM (called if property saveEDM is set to true)
    StatusCode saveToEdm( const G4PrimaryVertex*, const G4PrimaryParticle*, const G4PrimaryParticle* );

    /// Random number generators
    Rndm::Numbers p_thetaGen;
    Rndm::Numbers p_phiGen;
    Rndm::Numbers m_engyGen;

    /// Suppose the incident photon is directed along the Z axis
    /// the compton event occurs at the origin

    /// Minimum energy of the incident photons, set with energyMin
    Gaudi::Property<double> m_energyMin{ this, "energyMin", 10 * CLHEP::MeV,
                                         "[MeV] Minimum energy of incident photons" };
    /// Maximum energy of the incident photons, set with energyMax
    Gaudi::Property<double> m_energyMax{ this, "energyMax", 100 * CLHEP::MeV,
                                         "[MeV]Maximum energy of incident photons" };

    /// Minimum theta of the photons generated, set with thetaMin, the positive Angle to the Z axis
    Gaudi::Property<double> p_thetaMin{ this, "p_thetaMin", 0, "[radian]Minimum theta of generated photons" };
    /// Maximum theta of the photons generated, set with thetaMax,
    Gaudi::Property<double> p_thetaMax{ this, "p_thetaMax", M_PI, "[radian]Maximum theta of generated photons" };
    /// Minimum phi of the photons generated, set with phiMin,
    /// phi: The Angle between the projection of electron momentum in the XY plane and the positive direction of the X
    /// axis
    Gaudi::Property<double> p_phiMin{ this, "p_phiMin", 0., "[radian]Minimum phi of generated photons" };
    /// Maximum phi of the photons generated, set with phiMax
    Gaudi::Property<double> p_phiMax{ this, "p_phiMax", M_PI, "[radian]Maximum phi of generated photons" };

    /// Flag whether to save primary particle to EDM, set with saveEdm
    Gaudi::Property<bool> m_saveEdm{ this, "saveEdm", false };
    /// Handle for the genparticles to be written
    DataHandle<edm4hep::MCParticleCollection> m_genParticlesHandle{ "GenParticles", Gaudi::DataHandle::Writer, this };
  };

} // namespace megat

#endif
