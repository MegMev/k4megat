// local
#include "SimComptonEventGeneratorTool.h"

// FCCSW
#include "SimKernel/Units.h"

// Gaudi
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/PhysicalConstants.h"

// CLHEP
#include <CLHEP/Random/RandFlat.h>

// Geant4
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

// datamodel
#include "edm4hep/MCParticleCollection.h"

namespace megat {

  // Declaration of the Tool
  DECLARE_COMPONENT_WITH_ID( SimComptonEventGeneratorTool, "SimComptonEventGeneratorTool" )

  SimComptonEventGeneratorTool::SimComptonEventGeneratorTool( const std::string& type, const std::string& nam,
                                                              const IInterface* parent )
      : GaudiTool( type, nam, parent ) {
    declareInterface<ISimEventProviderTool>( this );
    declareProperty( "GenParticles", m_genParticlesHandle, "Handle for the genparticles to be written" );
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

} // namespace megat
