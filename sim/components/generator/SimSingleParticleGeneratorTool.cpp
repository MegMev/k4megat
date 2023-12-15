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

#include "edm4hep/MCParticleCollection.h"

/** @class SimSingleParticleGeneratorTool SimSingleParticleGeneratorTool.h "SimSingleParticleGeneratorTool.h"
 *
 *  Tool that generates single particles with parameters set via options file.
 *
 *  @author Andrea Dell'Acqua, J. Lingemann
 *  @modified Yong Zhou
 */

namespace megat {

  class SimSingleParticleGeneratorTool : public GaudiTool, virtual public ISimEventProviderTool {
  public:
    SimSingleParticleGeneratorTool( const std::string& type, const std::string& name, const IInterface* parent )
        : GaudiTool( type, name, parent ) {
      declareInterface<ISimEventProviderTool>( this );
      declareProperty( "GenParticles", m_genParticlesHandle, "Path name of the primary particle collection" );
    }

    virtual StatusCode initialize() final {
      if ( GaudiTool::initialize().isFailure() ) { return StatusCode::FAILURE; }

      auto _rdm_svc = svc<IRndmGenSvc>( "RndmGenSvc", true );
      m_energyGen.initialize( _rdm_svc, Rndm::Flat( m_energyMin, m_energyMax ) ).ignore();
      m_thetaGen.initialize( _rdm_svc, Rndm::Flat( m_thetaMin, m_thetaMax ) ).ignore();
      m_phiGen.initialize( _rdm_svc, Rndm::Flat( m_phiMin, m_phiMax ) ).ignore();
      m_vxGen.initialize( _rdm_svc, Rndm::Flat( -m_vertexX, m_vertexX ) ).ignore();
      m_vyGen.initialize( _rdm_svc, Rndm::Flat( -m_vertexY, m_vertexY ) ).ignore();
      m_vzGen.initialize( _rdm_svc, Rndm::Flat( -m_vertexZ, m_vertexZ ) ).ignore();

      if ( !G4ParticleTable::GetParticleTable()->contains( m_particleName.value() ) ) {
        error() << "Particle " << m_particleName << " cannot be found in G4ParticleTable" << endmsg;
        return StatusCode::FAILURE;
      }
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

    virtual G4Event* g4Event() final {
      auto theEvent = new G4Event();

      //
      G4ParticleTable*      particleTable = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition* particleDef   = particleTable->FindParticle( m_particleName.value() );
      debug() << "particle definition " << particleDef << " +++ " << m_particleName << endmsg;
      G4double mass = particleDef->GetPDGMass();
      debug() << "particle mass = " << mass << endmsg;

      double particleEnergy = m_energyGen();
      debug() << "particle energy = " << particleEnergy << " MeV" << endmsg;

      double theta = m_thetaGen();
      double phi   = m_phiGen();
      debug() << "particle theta, phi  = " << theta << " " << phi << endmsg;

      double dirX = std::sin( theta ) * std::cos( phi );
      double dirY = std::sin( theta ) * std::sin( phi );
      double dirZ = std::cos( theta );

      G4ThreeVector particleDir      = G4ThreeVector( dirX, dirY, dirZ );
      G4ThreeVector particlePosition = G4ThreeVector( m_vxGen(), m_vyGen(), m_vzGen() );

      G4PrimaryParticle* part = new G4PrimaryParticle( particleDef );
      part->SetMass( mass );
      part->SetKineticEnergy( particleEnergy );
      part->SetMomentumDirection( particleDir );
      part->SetCharge( particleDef->GetPDGCharge() );

      G4PrimaryVertex* vertex = new G4PrimaryVertex( particlePosition, 0. );
      vertex->SetPrimary( part );
      theEvent->AddPrimaryVertex( vertex );

      //
      if ( m_saveEdm ) { saveToEdm( vertex, part ).ignore(); }

      return theEvent;
    }

  private:
    /// Saves primary vertex and particle to output collection
    StatusCode saveToEdm( const G4PrimaryVertex* aVertex, const G4PrimaryParticle* aParticle ) {
      edm4hep::MCParticleCollection* particles = new edm4hep::MCParticleCollection();
      auto                           particle  = particles->create();

      particle.setVertex( {
          aVertex->GetX0() * g42edm::length,
          aVertex->GetY0() * g42edm::length,
          aVertex->GetZ0() * g42edm::length,
      } );
      particle.setTime( aVertex->GetT0() * Gaudi::Units::c_light * g42edm::length );

      particle.setPDG( aParticle->GetPDGcode() );
      particle.setCharge( aParticle->GetCharge() );
      particle.setGeneratorStatus( 1 );
      particle.setMomentum( {
          (float)( aParticle->GetPx() * g42edm::energy ),
          (float)( aParticle->GetPy() * g42edm::energy ),
          (float)( aParticle->GetPz() * g42edm::energy ),
      } );
      particle.setMass( aParticle->GetMass() * g42edm::energy );

      m_genParticlesHandle.put( particles );
      return StatusCode::SUCCESS;
    }

  private:
    /// Random numbers
    Rndm::Numbers m_energyGen;
    Rndm::Numbers m_thetaGen;
    Rndm::Numbers m_phiGen;
    Rndm::Numbers m_vxGen;
    Rndm::Numbers m_vyGen;
    Rndm::Numbers m_vzGen;

    /// Minimum energy of the particles generated, set with energyMin
    Gaudi::Property<double> m_energyMin{ this, "energyMin", 1, "Minimum energy of generated particles [MeV]" };
    /// Maximum energy of the particles generated, set with energyMax
    Gaudi::Property<double> m_energyMax{ this, "energyMax", 1, "Maximum energy of generated particles [MeV]" };
    /// Minimum eta of the particles generated, set with etaMin
    Gaudi::Property<double> m_thetaMin{ this, "thetaMin", 0., "Minimum eta of generated particles [degree]" };
    /// Maximum eta of the particles generated, set with etaMax
    Gaudi::Property<double> m_thetaMax{ this, "thetaMax", 180 * CLHEP::deg,
                                        "Maximum eta of generated particles [degree]" };
    /// Minimum phi of the particles generated, set with phiMin
    Gaudi::Property<double> m_phiMin{ this, "phiMin", 0., "Minimum phi of generated particles [degree]" };
    /// Maximum phi of the particles generated, set with phiMax
    Gaudi::Property<double> m_phiMax{ this, "phiMax", 360 * CLHEP::deg, "Maximum phi of generated particles [degree]" };
    /// x position of the vertex associated with the particles generated, set with vertexX
    Gaudi::Property<double> m_vertexX{ this, "vertexX", 0, "Vertex X [mm]" };
    /// y position of the vertex associated with the particles generated, set with vertexY
    Gaudi::Property<double> m_vertexY{ this, "vertexY", 0, "Vertex Y [mm]" };
    /// z position of the vertex associated with the particles generated, set with vertexZ
    Gaudi::Property<double> m_vertexZ{ this, "vertexZ", 0, "Vertex Z [mm]" };
    /// Name of the generated particle, set with particleName
    Gaudi::Property<std::string> m_particleName{ this, "particleName", "geantino", "Name of the generated particles" };
    /// Flag whether to save primary particle to EDM, set with saveEdm
    Gaudi::Property<bool> m_saveEdm{ this, "saveEdm", false, "Save to data store" };

    /// Handle for the genparticles to be written
    DataHandle<edm4hep::MCParticleCollection> m_genParticlesHandle{ "GenParticles", Gaudi::DataHandle::Writer, this };
  };

  DECLARE_COMPONENT_WITH_ID( SimSingleParticleGeneratorTool, "SimSingleParticleGeneratorTool" )

} // namespace megat
