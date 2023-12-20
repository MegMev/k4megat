#include "SimInterface/ISimEventProviderTool.h"
#include "SimKernel/Units.h"
#include "k4FWCore/DataHandle.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/RndmGenerators.h"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

/**
 *  Tool that generates one single particle, configurables:
 *  - energy range
 *  - direction range
 *  - vertex position
 *
 *  @modified Yong Zhou
 *
 */

namespace megat {

  class SimSingleParticleGenerator : public GaudiTool, virtual public ISimEventProviderTool {
  public:
    SimSingleParticleGenerator( const std::string& type, const std::string& name, const IInterface* parent )
        : GaudiTool( type, name, parent ) {
      declareInterface<ISimEventProviderTool>( this );
    }

    virtual StatusCode initialize() final;
    virtual G4Event*   g4Event() final;

  private:
    /// Random numbers
    Rndm::Numbers m_energyGen;
    Rndm::Numbers m_thetaGen;
    Rndm::Numbers m_phiGen;

    /// Minimum energy of the particles generated, set with energyMin
    Gaudi::Property<double> m_energyMin{ this, "energyMin", 1, "Minimum energy of generated particles [MeV]" };
    /// Maximum energy of the particles generated, set with energyMax
    Gaudi::Property<double> m_energyMax{ this, "energyMax", 1, "Maximum energy of generated particles [MeV]" };
    /// Minimum eta of the particles generated, set with etaMin
    Gaudi::Property<double> m_thetaMin{ this, "thetaMin", 0., "Minimum eta of generated particles [degree]" };
    /// Maximum eta of the particles generated, set with etaMax
    Gaudi::Property<double> m_thetaMax{ this, "thetaMax", 180, "Maximum eta of generated particles [degree]" };
    /// Minimum phi of the particles generated, set with phiMin
    Gaudi::Property<double> m_phiMin{ this, "phiMin", 0., "Minimum phi of generated particles [degree]" };
    /// Maximum phi of the particles generated, set with phiMax
    Gaudi::Property<double> m_phiMax{ this, "phiMax", 360., "Maximum phi of generated particles [degree]" };
    /// x position of the vertex associated with the particles generated, set with vertexX
    Gaudi::Property<double> m_vertexX{ this, "vertexX", 0, "Vertex X [mm]" };
    /// y position of the vertex associated with the particles generated, set with vertexY
    Gaudi::Property<double> m_vertexY{ this, "vertexY", 0, "Vertex Y [mm]" };
    /// z position of the vertex associated with the particles generated, set with vertexZ
    Gaudi::Property<double> m_vertexZ{ this, "vertexZ", 0, "Vertex Z [mm]" };

    /// Name of the generated particle, set with particleName
    Gaudi::Property<std::string> m_particleName{ this, "particleName", "geantino", "Name of the generated particles" };
    G4ParticleDefinition*        m_particleDef;
  };

  StatusCode SimSingleParticleGenerator::initialize() {
    if ( GaudiTool::initialize().isFailure() ) { return StatusCode::FAILURE; }

    // 1. init rndm numbers
    auto _rdm_svc = svc<IRndmGenSvc>( "RndmGenSvc", true );
    m_energyGen.initialize( _rdm_svc, Rndm::Flat( m_energyMin, m_energyMax ) ).ignore();
    m_thetaGen.initialize( _rdm_svc, Rndm::Flat( m_thetaMin, m_thetaMax ) ).ignore();
    m_phiGen.initialize( _rdm_svc, Rndm::Flat( m_phiMin, m_phiMax ) ).ignore();

    // 2. particle
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    if ( !particleTable->contains( m_particleName.value() ) ) {
      error() << "Particle " << m_particleName << " cannot be found in G4ParticleTable" << endmsg;
      return StatusCode::FAILURE;
    }

    m_particleDef = particleTable->FindParticle( m_particleName.value() );
    debug() << "particle definition " << m_particleDef << " +++ " << m_particleName << endmsg;

    // 3. others
    if ( m_energyMin > m_energyMax ) {
      error() << "Maximum energy cannot be lower than the minumum energy" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_thetaMin > m_thetaMax ) {
      error() << "Maximum psudorapidity cannot be lower than the minumum psudorapidity" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_phiMin > m_phiMax ) {
      error() << "Maximum azimuthal angle cannot be lower than the minumum angle" << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  G4Event* SimSingleParticleGenerator::g4Event() {
    auto theEvent = new G4Event();

    //
    double particleEnergy = m_energyGen() * CLHEP::MeV;
    debug() << "particle energy = " << particleEnergy << " MeV" << endmsg;

    double theta = m_thetaGen() * CLHEP::deg;
    double phi   = m_phiGen() * CLHEP::deg;
    debug() << "particle theta, phi  = " << theta << " " << phi << " degree" << endmsg;

    double dirX = std::sin( theta ) * std::cos( phi );
    double dirY = std::sin( theta ) * std::sin( phi );
    double dirZ = std::cos( theta );

    auto* part = new G4PrimaryParticle( m_particleDef );
    part->SetKineticEnergy( particleEnergy );
    part->SetMomentumDirection( { dirX, dirY, dirZ } );

    //
    G4PrimaryVertex* vertex = new G4PrimaryVertex( m_vertexX, m_vertexY, m_vertexZ, 0. );
    vertex->SetPrimary( part );
    theEvent->AddPrimaryVertex( vertex );

    return theEvent;
  }

  // component declaration
  DECLARE_COMPONENT_WITH_ID( SimSingleParticleGenerator, "SimSingleParticleGenerator" )

} // namespace megat
