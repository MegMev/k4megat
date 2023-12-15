
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/RndmGenerators.h"

#include "SimInterface/ISimEventProviderTool.h"
#include "SimKernel/Units.h"
#include "k4FWCore/DataHandle.h"

// CLHEP
#include <CLHEP/Random/RandFlat.h>

// Geant4
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

// datamodel
#include "edm4hep/MCParticleCollection.h"

namespace megat {

  /** @class SimComptonEventGeneratorTool SimComptonEventGeneratorTool.h "SimComptonEventGeneratorTool.h"
   *
   *  Tool that generates compton events at origin, with a flux direction or 4-pi uniform direction
   *
   */

  class SimComptonEventGeneratorTool : public GaudiTool, virtual public ISimEventProviderTool {
  public:
    SimComptonEventGeneratorTool( const std::string& type, const std::string& name, const IInterface* parent );
    virtual ~SimComptonEventGeneratorTool();

    virtual StatusCode initialize() final;

    virtual G4Event* g4Event() final;

  private:
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

  SimComptonEventGeneratorTool::SimComptonEventGeneratorTool( const std::string& type, const std::string& nam,
                                                              const IInterface* parent )
      : GaudiTool( type, nam, parent ) {
    declareInterface<ISimEventProviderTool>( this );
    declareProperty( "GenParticles", m_genParticlesHandle, "Path name of the primary particle collection" );
  }

  SimComptonEventGeneratorTool::~SimComptonEventGeneratorTool() {}

  StatusCode SimComptonEventGeneratorTool::initialize() {
    if ( GaudiTool::initialize().isFailure() ) { return StatusCode::FAILURE; }

    // 1. get random service
    auto _rdm_svc = svc<IRndmGenSvc>( "RndmGenSvc", true );
    p_thetaGen.initialize( _rdm_svc, Rndm::Flat( p_thetaMin, p_thetaMax ) );
    p_phiGen.initialize( _rdm_svc, Rndm::Flat( p_phiMin, p_phiMax ) );
    m_engyGen.initialize( _rdm_svc, Rndm::Flat( m_energyMin, m_energyMax ) );

    // 2.
    if ( !G4ParticleTable::GetParticleTable()->contains( "e-" ) ) {
      error() << "electron cannot be found in G4ParticleTable" << endmsg;
      return StatusCode::FAILURE;
    }

    if ( !G4ParticleTable::GetParticleTable()->contains( "gamma" ) ) {
      error() << "photon cannot be found in G4ParticleTable" << endmsg;
      return StatusCode::FAILURE;
    }

    if ( m_energyMin > m_energyMax ) {
      error() << "Maximum energy cannot be lower than the minumum energy" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( p_thetaMin > p_thetaMax ) {
      error() << "Maximum psudorapidity cannot be lower than the minumum psudorapidity" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( p_phiMin > p_phiMax ) {
      error() << "Maximum azimuthal angle cannot be lower than the minumum angle" << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  G4Event* SimComptonEventGeneratorTool::g4Event() {
    auto                  theEvent      = new G4Event();
    G4ParticleTable*      particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* electronDef   = particleTable->FindParticle( "e-" );
    debug() << "electron definition " << electronDef << " +++ "
            << "electron" << endmsg;
    G4ParticleDefinition* photonDef = particleTable->FindParticle( "gamma" );
    debug() << "photon definition " << photonDef << " +++ "
            << "photon" << endmsg;
    G4double e_mass = electronDef->GetPDGMass();
    debug() << "electron mass = " << e_mass << endmsg;

    double p_theta = p_thetaGen();
    double p_phi   = p_phiGen();
    debug() << "scattered photons theta, phi  = " << p_theta << " " << p_phi << endmsg;

    double m_Energy     = m_engyGen();
    double speedOfLight = CLHEP::c_light;
    double e_Energy =
        m_Energy *
        ( 1 - 1 / ( ( m_Energy / ( e_mass * std::pow( speedOfLight, 2 ) ) ) * ( 1 - std::cos( p_theta ) ) + 1 ) );
    debug() << "electron energy = " << e_Energy << endmsg;
    double p_Energy =
        m_Energy *
        ( 1 / ( ( m_Energy / ( e_mass * std::pow( speedOfLight, 2 ) ) ) * ( 1 - std::cos( p_theta ) ) + 1 ) );
    debug() << "photon energy = " << p_Energy << endmsg;

    double e_theta =
        std::atan( ( std::sin( p_theta ) ) /
                   ( 1 + m_Energy * ( 1 - std::cos( p_theta ) ) / ( e_mass * std::pow( speedOfLight, 2 ) ) -
                     std::cos( p_theta ) ) );
    // phi: The Angle between the projection of photons momentum in the XY plane and the positive direction of the X
    // axis
    double e_phi = M_PI - p_phi;
    debug() << "electron theta, phi  = " << e_theta << " " << e_phi << endmsg;

    double e_randomX = std::sin( e_theta ) * std::cos( e_phi );
    double e_randomY = std::sin( e_theta ) * std::sin( e_phi );
    double e_randomZ = std::cos( e_theta );

    double p_randomX = std::sin( p_theta ) * std::cos( p_phi );
    double p_randomY = std::sin( p_theta ) * std::sin( p_phi );
    double p_randomZ = std::cos( p_theta );

    G4ThreeVector electronDir = G4ThreeVector( e_randomX, e_randomY, e_randomZ );
    G4ThreeVector photonDir   = G4ThreeVector( p_randomX, p_randomY, p_randomZ );

    G4PrimaryVertex*   vertex = new G4PrimaryVertex( 0, 0, 0, 0. );
    G4PrimaryParticle* elec   = new G4PrimaryParticle( electronDef );
    G4PrimaryParticle* phot   = new G4PrimaryParticle( photonDef );

    elec->SetMass( e_mass );
    elec->SetKineticEnergy( e_Energy );
    elec->SetMomentumDirection( electronDir );
    elec->SetCharge( electronDef->GetPDGCharge() );

    phot->SetKineticEnergy( p_Energy );
    phot->SetMomentumDirection( photonDir );

    vertex->SetPrimary( elec );

    vertex->SetPrimary( phot );
    theEvent->AddPrimaryVertex( vertex );
    if ( m_saveEdm ) { saveToEdm( vertex, elec, phot ).ignore(); }
    return theEvent;
  }

  StatusCode SimComptonEventGeneratorTool::saveToEdm( const G4PrimaryVertex* aVertex, const G4PrimaryParticle* aGamma,
                                                      const G4PrimaryParticle* aElectron ) {
    // 0 create a new collection
    auto particles = m_genParticlesHandle.createAndPut();

    // 1 add gamma
    /// 1.1 create a new particle
    auto particle = particles->create();

    /// 1.2 set parameters of the new particle
    particle.setVertex( {
        aVertex->GetX0() * g42edm::length,
        aVertex->GetY0() * g42edm::length,
        aVertex->GetZ0() * g42edm::length,
    } );
    particle.setTime( aVertex->GetT0() * Gaudi::Units::c_light * g42edm::length );

    particle.setPDG( aGamma->GetPDGcode() );
    particle.setGeneratorStatus( 1 );
    particle.setMomentum( {
        (float)( aGamma->GetPx() * g42edm::energy ),
        (float)( aGamma->GetPy() * g42edm::energy ),
        (float)( aGamma->GetPz() * g42edm::energy ),
    } );
    particle.setMass( aGamma->GetMass() * g42edm::energy );

    // add electron
    /// 1.1 create a new particle
    particle = particles->create();

    /// 1.2 set parameters of the new particle
    particle.setVertex( {
        aVertex->GetX0() * g42edm::length,
        aVertex->GetY0() * g42edm::length,
        aVertex->GetZ0() * g42edm::length,
    } );
    particle.setTime( aVertex->GetT0() * Gaudi::Units::c_light * g42edm::length );

    particle.setPDG( aElectron->GetPDGcode() );
    particle.setGeneratorStatus( 1 );
    particle.setMomentum( {
        (float)( aElectron->GetPx() * g42edm::energy ),
        (float)( aElectron->GetPy() * g42edm::energy ),
        (float)( aElectron->GetPz() * g42edm::energy ),
    } );
    particle.setMass( aElectron->GetMass() * g42edm::energy );

    return StatusCode::SUCCESS;
  }

  // component declaration
  DECLARE_COMPONENT_WITH_ID( SimComptonEventGeneratorTool, "SimComptonEventGeneratorTool" )

} // namespace megat
