#include "SimInterface/ISimEventProviderTool.h"
#include "SimKernel/Units.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/RndmGenerators.h"
#include "k4FWCore/DataHandle.h"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Vector/ThreeVector.h>

#include "details/utils.h"

/**
 *  Tool that generates compton events at origin, with fixed incident direction and
 *  isotropic scattering direction of photon.
 *  Mainly designed for reconstruction algorithm development.
 *
 *  Configurables:
 *  - incident energy range
 *  - incident direction
 *
 *  @author Yong Zhou
 */

namespace megat {

  class SimComptonSimpleGenerator : public GaudiTool, virtual public ISimEventProviderTool {
  public:
    SimComptonSimpleGenerator( const std::string& type, const std::string& name, const IInterface* parent )
        : GaudiTool( type, name, parent ) {
      declareInterface<ISimEventProviderTool>( this );
    }

    virtual StatusCode initialize() final;
    virtual G4Event*   g4Event() final;

  private:
    /// Random number generators
    Rndm::Numbers m_engyGen;

    Gaudi::Property<double> m_energyMin{ this, "energyMin", 1, "Minimum energy of incident photons [MeV]" };
    Gaudi::Property<double> m_energyMax{ this, "energyMax", 10, "Maximum energy of incident photons [MeV]" };
    Gaudi::Property<double> m_theta{ this, "theta", 0, "Theta angle of the incident direction [degree]" };
    Gaudi::Property<double> m_phi{ this, "phi", 0, "Phi angle of the incident direction [degree]" };

    /// particles
    G4ParticleDefinition* m_electronDef;
    G4ParticleDefinition* m_photonDef;
    CLHEP::Hep3Vector     m_direction{ 0, 0, 1 };
  };

  StatusCode SimComptonSimpleGenerator::initialize() {
    if ( GaudiTool::initialize().isFailure() ) { return StatusCode::FAILURE; }

    // 1. init rndm numbers
    auto _rdm_svc = svc<IRndmGenSvc>( "RndmGenSvc", true );
    m_engyGen.initialize( _rdm_svc, Rndm::Flat( m_energyMin, m_energyMax ) ).ignore();

    // 2. particles
    auto* particleTable = G4ParticleTable::GetParticleTable();
    m_electronDef       = particleTable->FindParticle( "e-" );
    m_photonDef         = particleTable->FindParticle( "gamma" );

    // 3. others
    if ( m_energyMin > m_energyMax ) {
      error() << "Maximum energy cannot be lower than the minumum energy" << endmsg;
      return StatusCode::FAILURE;
    }

    double dirX = std::sin( m_theta * CLHEP::degree ) * std::cos( m_phi * CLHEP::degree );
    double dirY = std::sin( m_theta * CLHEP::degree ) * std::sin( m_phi * CLHEP::degree );
    double dirZ = std::cos( m_theta * CLHEP::degree );
    m_direction = { dirX, dirY, dirZ };

    return StatusCode::SUCCESS;
  }

  G4Event* SimComptonSimpleGenerator::g4Event() {
    // 1. pre-init parameters
    auto dir1 = isotropicVector(); // scattered photon direction

    auto e0   = m_engyGen(); // incident photon energy
    auto E0_m = e0 / electron_mass;

    // calculated angular parameters
    auto onecost = 1. - dir1.cosTheta();
    auto epsilon = 1. / ( 1. + E0_m * onecost );
    dir1.rotateUz( m_direction );

    // calculated dynamic parameters
    auto e1         = epsilon * e0; // scattered photon energy
    auto eKinEnergy = e0 - e1;      // scattered electron kinetic energy
    auto eDirection = e0 * m_direction - e1 * dir1;
    eDirection      = eDirection.unit(); // scattered electron direction

    debug() << "incident photon energy = " << e0 << " MeV" << endmsg;
    debug() << "scattered photon energy = " << e1 << " MeV" << endmsg;
    debug() << "scattered electron energy = " << eKinEnergy << " MeV" << endmsg;
    debug() << "scattered electron mom = " << eDirection << endmsg;
    debug() << "scattered photon  mom= " << dir1 << endmsg;

    // 2. create primary vertexes
    auto theEvent = new G4Event();

    G4PrimaryParticle* elec = new G4PrimaryParticle( m_electronDef );
    elec->SetKineticEnergy( eKinEnergy );
    elec->SetMomentumDirection( static_cast<G4ThreeVector>( eDirection ) );

    G4PrimaryParticle* phot = new G4PrimaryParticle( m_photonDef );
    phot->SetTotalEnergy( e1 );
    phot->SetMomentumDirection( static_cast<G4ThreeVector>( dir1 ) );

    //
    G4PrimaryVertex* vertex = new G4PrimaryVertex( 0, 0, 0, 0. );
    vertex->SetPrimary( elec );
    vertex->SetPrimary( phot );
    theEvent->AddPrimaryVertex( vertex );

    return theEvent;
  }

  // component declaration
  DECLARE_COMPONENT_WITH_ID( SimComptonSimpleGenerator, "SimComptonSimpleGenerator" )

} // namespace megat
