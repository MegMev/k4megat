#include "SimInterface/IGeoSvc.h"
#include "SimInterface/ISimVertexSmearTool.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "DD4hep/DetectorTools.h"
#include "DD4hep/Shapes.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "TGeoMatrix.h"

#include <string>

/**
 *  Smear the vertex (origin point) homogeniously inside a volume's bounding box.
 *  The target volume is customized by its path in the detector element geometry hierarchy tree.
 *
 *  @author Yong Zhou
 */

namespace megat {

  class SimVertexSmearVolumeTool : public GaudiTool, virtual public ISimVertexSmearTool {
  public:
    SimVertexSmearVolumeTool( const std::string& type, const std::string& name, const IInterface* parent )
        : GaudiTool( type, name, parent ) {
      declareInterface<ISimVertexSmearTool>( this );
    }

    ~SimVertexSmearVolumeTool() = default;

    StatusCode initialize() final;

    CLHEP::Hep3Vector smearVertex();

  private:
    dd4hep::Solid m_solid; // solid node
    TGeoHMatrix   m_trans; // global transform of the target volume

    /// Random number generators
    Rndm::Numbers m_rndmX;
    Rndm::Numbers m_rndmY;
    Rndm::Numbers m_rndmZ;

    /// path string of the target detector element, which provides the bounding box
    Gaudi::Property<std::string> m_volPath{ this, "volumePath", "/world/TPC/Gas",
                                            "Path string of the target node in detector element tree" };
  };

  StatusCode SimVertexSmearVolumeTool::initialize() {
    // 0. init base class
    if ( GaudiTool::initialize().isFailure() ) { return StatusCode::FAILURE; }

    // 1. fetch geometry svc and find the pv
    auto _geo_svc = svc<IGeoSvc>( "GeoSvc", true );
    if ( !_geo_svc ) {
      error() << "Can't locate GeoSvc" << endmsg;
      return StatusCode::FAILURE;
    }

    auto _de = _geo_svc->getDetElement( m_volPath );
    if ( !_de.isValid() ) {
      error() << "Can't find DetElement: " << m_volPath << endmsg;
      return StatusCode::FAILURE;
    }
    m_solid = _de.solid();
    dd4hep::detail::tools::PlacementPath _ppath;
    dd4hep::detail::tools::placementPath( _de, _ppath );
    dd4hep::detail::tools::placementTrafo( _ppath, false, m_trans );

    // 2. fetch rndm svc and init the flat generator
    auto _rdm_svc = svc<IRndmGenSvc>( "RndmGenSvc", true );
    if ( !_rdm_svc ) {
      error() << "Can't locate RndmGenSvc" << endmsg;
      return StatusCode::FAILURE;
    }

    // [caution] TGeo use cm, while DD4hep use mm in k4megat
    auto _box = _de.volume().boundingBox();
    auto _x   = _box.x() / CLHEP::cm;
    auto _y   = _box.y() / CLHEP::cm;
    auto _z   = _box.z() / CLHEP::cm;

    m_rndmX.initialize( _rdm_svc, Rndm::Flat( -_x, _x ) ).ignore();
    m_rndmY.initialize( _rdm_svc, Rndm::Flat( -_y, _y ) ).ignore();
    m_rndmZ.initialize( _rdm_svc, Rndm::Flat( -_z, _z ) ).ignore();

    return StatusCode::SUCCESS;
  }

  CLHEP::Hep3Vector SimVertexSmearVolumeTool::smearVertex() {
    double _local[3], _global[3];

    do {
      _local[0] = m_rndmX();
      _local[1] = m_rndmY();
      _local[2] = m_rndmZ();

    } while ( !m_solid->Contains( _local ) );

    m_trans.LocalToMaster( _local, _global );
    return { _global[0], _global[1], _global[2] };
  }

  DECLARE_COMPONENT_WITH_ID( SimVertexSmearVolumeTool, "SimVertexSmearVolumeTool" )
} // namespace megat
