#include "SimInterface/ISimEventProviderTool.h"
#include "details/EcoMug.h"

#include "GaudiAlg/GaudiTool.h"
#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Vector/ThreeVector.h>

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

/**
 *  Tool that wraps EcoMug fast muon generator (https://github.com/dr4kan/EcoMug).
 *  Only basic feature set of EcoMug are exposed, the following advanced features are not
 *  integrated:
 *  - Custom differential flux
 *  - Custom rate and time estimation
 *  - EMMultiGen (i.e. backgound generation)
 *
 *  These advanced features may be integrated based on actual needs in the future.
 *
 *  @author Yong Zhou
 */

namespace megat {

  class SimEcoMugGenerator : public GaudiTool, virtual public ISimEventProviderTool {
  public:
    SimEcoMugGenerator( const std::string& type, const std::string& name, const IInterface* parent )
        : GaudiTool( type, name, parent ) {
      declareInterface<ISimEventProviderTool>( this );
    }

    virtual StatusCode initialize() final;
    virtual G4Event*   g4Event() final;

  private:
    Gaudi::Property<std::vector<double>> m_momRange{
        this, "MomentumRange", {}, "Range of momentum (min, max) of the muon [GeV/c]" };
    Gaudi::Property<std::vector<double>> m_phiRangeDirection{
        this, "PhiRangeDirection", {}, "Range of phi angle (min, max) of the generated muon direction [degree]" };
    Gaudi::Property<std::vector<double>> m_thetaRangeDirection{
        this, "ThetaRangeDirection", {}, "Range of theta angle (min, max) of the generated muon direction [degree]" };

    Gaudi::Property<std::string>         m_geometry{ this, "Geometry", "HalfSphere",
                                             "Geometry type of EcoMug (Sky/Cylinder/HalfSphere)" };
    Gaudi::Property<std::vector<double>> m_position{
        this, "Position", {}, "Center position of the generation geometry shape [mm]" };
    Gaudi::Property<std::vector<double>> m_size{
        this, "SizeXY", { 10., 10. }, "Full width (x, y) of the Sky plane [mm]" };
    Gaudi::Property<double> m_radius{ this, "Radius", 10., "Radius of the Cylinder/HalfSphere geometry [mm]" };
    Gaudi::Property<double> m_height{ this, "Height", 10., "Height of the Cylinder geometry [mm]" };
    Gaudi::Property<std::vector<double>> m_phiRangePosition{
        this,
        "PhiRangePosition",
        {},
        "Range of phi angle (min, max) of the generation position in Cylinder/HalfSphere [degree]" };
    Gaudi::Property<std::vector<double>> m_thetaRangePosition{
        this,
        "ThetaRangePosition",
        {},
        "Range of theta angle (min, max) of the generation position in Cylinder/HalfSphere [degree]" };

    Gaudi::Property<std::uint64_t> m_seed{ this, "Seed", 0., "Internal Random Seed of EcoMug" };
    Gaudi::Property<double> m_duration{ this, "Duration", -1, "Acquisition Time for event counts estimation [min]" };

    // actual generator
    std::unique_ptr<EcoMug> m_ecoMug{ nullptr };

    /// muon
    G4ParticleDefinition* m_muonDef;
  };

  StatusCode SimEcoMugGenerator::initialize() {
    if ( GaudiTool::initialize().isFailure() ) { return StatusCode::FAILURE; }

    // 0. create EcoMug & muon particle
    m_ecoMug            = std::make_unique<EcoMug>();
    auto* particleTable = G4ParticleTable::GetParticleTable();
    m_muonDef           = particleTable->FindParticle( "mu-" );

    // 1. common parameters
    if ( !m_momRange.empty() ) {
      if ( m_momRange.size() != 2 ) {
        error() << "incorrect momentum range config length: " << m_momRange.size() << endmsg;
        return StatusCode::FAILURE;
      }

      m_ecoMug->SetMinimumMomentum( m_momRange[0] );
      m_ecoMug->SetMaximumMomentum( m_momRange[1] );
    }

    if ( !m_phiRangeDirection.empty() ) {
      if ( m_phiRangeDirection.size() != 2 ) {
        error() << "incorrect direction phi range config length: " << m_phiRangeDirection.size() << endmsg;
        return StatusCode::FAILURE;
      }
      m_ecoMug->SetMinimumPhi( m_phiRangeDirection[0] * CLHEP::degree );
      m_ecoMug->SetMaximumPhi( m_phiRangeDirection[1] * CLHEP::degree );
    }

    if ( !m_thetaRangeDirection.empty() ) {
      if ( m_thetaRangeDirection.size() != 2 ) {
        error() << "incorrect direction theta range config length: " << m_thetaRangeDirection.size() << endmsg;
        return StatusCode::FAILURE;
      }
      m_ecoMug->SetMinimumTheta( m_thetaRangeDirection[0] * CLHEP::degree );
      m_ecoMug->SetMaximumTheta( m_thetaRangeDirection[1] * CLHEP::degree );
    }

    // tempos
    std::array<double, 3> _position;
    if ( !m_position.empty() ) {
      if ( m_position.size() != 3 ) {
        error() << "incorrect position config length: " << m_position.size() << endmsg;
        return StatusCode::FAILURE;
      }
      for ( size_t i = 0; i < 3; i++ ) { _position[i] = m_position[i]; }
    }

    std::vector<double> _phi_pos;
    if ( !m_phiRangePosition.empty() ) {
      if ( m_phiRangePosition.size() != 2 ) {
        error() << "incorrect position phi range config length: " << m_phiRangePosition.size() << endmsg;
        return StatusCode::FAILURE;
      }
      _phi_pos = m_phiRangePosition.value();
    }

    std::vector<double> _theta_pos;
    if ( !m_thetaRangePosition.empty() ) {
      if ( m_thetaRangePosition.size() != 2 ) {
        error() << "incorrect position theta range config length: " << m_thetaRangePosition.size() << endmsg;
        return StatusCode::FAILURE;
      }
      _theta_pos = m_thetaRangePosition.value();
    }

    // 2. special parameters
    std::transform( m_geometry.begin(), m_geometry.end(), m_geometry.begin(),
                    []( unsigned char c ) { return std::toupper( c ); } );
    if ( m_geometry == "HALFSPHERE" || m_geometry == "SPHERE" ) {
      m_ecoMug->SetUseHSphere();
      m_ecoMug->SetHSphereRadius( m_radius );

      //
      if ( !_position.empty() ) { m_ecoMug->SetHSphereCenterPosition( _position ); }

      if ( !_phi_pos.empty() ) {
        m_ecoMug->SetHSphereMinPositionPhi( _phi_pos[0] * CLHEP::degree );
        m_ecoMug->SetHSphereMaxPositionPhi( _phi_pos[1] * CLHEP::degree );
      }

      if ( !_theta_pos.empty() ) {
        m_ecoMug->SetHSphereMinPositionTheta( _theta_pos[0] * CLHEP::degree );
        m_ecoMug->SetHSphereMaxPositionTheta( _theta_pos[1] * CLHEP::degree );
      }
    } else if ( m_geometry == "SKY" ) {
      m_ecoMug->SetUseSky();
      m_ecoMug->SetSkySize( { m_size[0], m_size[1] } );

      if ( !_position.empty() ) { m_ecoMug->SetSkyCenterPosition( _position ); }

    } else if ( m_geometry == "CYLINDER" ) {
      m_ecoMug->SetUseCylinder();
      m_ecoMug->SetCylinderRadius( m_radius );
      m_ecoMug->SetCylinderHeight( m_height );

      if ( !_position.empty() ) { m_ecoMug->SetCylinderCenterPosition( _position ); }

      if ( !_phi_pos.empty() ) {
        m_ecoMug->SetCylinderMinPositionPhi( _phi_pos[0] * CLHEP::degree );
        m_ecoMug->SetCylinderMaxPositionPhi( _phi_pos[1] * CLHEP::degree );
      }
    } else {
      error() << "unrecognized geometry type: " << m_geometry << endmsg;
      return StatusCode::FAILURE;
    }

    // 3. rdm seed
    m_ecoMug->SetSeed( m_seed );

    // 4. estimated event counts for specified duration
    if ( m_duration > 0 ) {
      auto _area     = m_ecoMug->GetGenSurfaceArea() * EMUnits::mm2 / EMUnits::m2;
      auto _avg_rate = m_ecoMug->GetAverageGenRate() / EMUnits::hertz * EMUnits::m2;
      info() << "Expected event counts for " << m_duration << " mins: " << _area * _avg_rate * m_duration * 60
             << endmsg;
    }

    return StatusCode::SUCCESS;
  }

  G4Event* SimEcoMugGenerator::g4Event() {
    // generate new muon with EcoMug
    m_ecoMug->Generate();

    // fetch muon property
    auto   muon_pos   = m_ecoMug->GetGenerationPosition(); // in mm
    double muon_mom   = m_ecoMug->GetGenerationMomentum(); // in GeV
    double muon_theta = m_ecoMug->GetGenerationTheta();    // in radian
    double muon_phi   = m_ecoMug->GetGenerationPhi();      // in radian

    CLHEP::Hep3Vector _mom = { std::sin( muon_theta ) * std::cos( muon_phi ), // mom direction
                               std::sin( muon_theta ) * std::sin( muon_phi ), cos( muon_theta ) };
    _mom *= muon_mom;   // mom vector
    _mom *= CLHEP::GeV; // unit conversion: GeV -> MeV

    // create G4 event
    auto* theEvent = new G4Event();

    auto* muon = new G4PrimaryParticle( m_muonDef );
    muon->SetMomentum( _mom.x(), _mom.y(), _mom.z() );
    auto* vertex = new G4PrimaryVertex( muon_pos[0], muon_pos[1], muon_pos[2], 0 );
    vertex->SetPrimary( muon );

    theEvent->AddPrimaryVertex( vertex );

    return theEvent;
  }

  // component declaration
  DECLARE_COMPONENT_WITH_ID( SimEcoMugGenerator, "SimEcoMugGenerator" )

} // namespace megat
