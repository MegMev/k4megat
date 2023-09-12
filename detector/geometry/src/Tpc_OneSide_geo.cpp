//==========================================================================
//  DD4hep detector plugin of Megat TPC
//--------------------------------------------------------------------------
// Copyright  : USTC
// Author     : Yong Zhou
//
//==========================================================================
//
// A simply version with:
// - Drift volume
// - Anode surface
//
// Build strategy:
// 1. build the envelope (<envelope>)
// 2. create and placement of drift volume with anode surface (<driftvol>)
// 3. build other components to VolumeBuilder (solid, volume, physvol)
//
// Be aware of some refinement imposed:
// 1. force removing copyno and '_envelope' suffix of placement to keep path string compact
//    - for placement created by VolumeBuilder, use 'name' attribute for renaming
// 2. force hide envelope
//==========================================================================

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Volumes.h"
#include "DDRec/Surface.h"
#include "XML/Utilities.h"
#include "XML/VolumeBuilder.h"
#include "XML/XML.h"
#include <DD4hep/BuildType.h>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::rec;
using namespace dd4hep::detail;
using Type = dd4hep::rec::SurfaceType;

namespace {

  /// helper class for a planar surface for readout area
  class ReadoutPlaneImp : public VolPlaneImpl {
    double v_length, u_length;

  public:
    /// standard c'tor with all necessary arguments - origin is (0,0,0) if not given.
    ReadoutPlaneImp( SurfaceType typ, double thickness_inner, double thickness_outer, Vector3D u_val, Vector3D v_val,
                     Vector3D n_val, Vector3D o_val, dd4hep::Volume vol, int id_val )
        : VolPlaneImpl( typ, thickness_inner, thickness_outer, u_val, v_val, n_val, o_val, vol, id_val )
        , v_length( 0 )
        , u_length( 0 ) {}

    void setData( double u, double v ) {
      v_length = v;
      u_length = u;
    }

    double length_along_v() const { return v_length; }

    double length_along_u() const { return u_length; }

    void setID( int64_t id_val ) { _id = id_val; }

    // overwrite to include points inside the inner radius of the barrel
    bool insideBounds( const Vector3D& point, double epsilon ) const {
      Vector2D uvVec = globalToLocal( point );

      return ( std::abs( distance( point ) ) < epsilon ) && std::abs( uvVec[0] ) < u_length / 2. &&
             std::abs( uvVec[1] ) < v_length / 2.;
    }

    // create outer bounding lines for the given symmetry of the polyhedron
    virtual std::vector<std::pair<Vector3D, Vector3D>> getLines( unsigned ) {

      std::vector<std::pair<Vector3D, Vector3D>> lines;

      lines.push_back( std::make_pair( origin() + u_length / 2. * u() + v_length / 2. * v(),
                                       origin() - u_length / 2. * u() + v_length / 2. * v() ) );
      lines.push_back( std::make_pair( origin() - u_length / 2. * u() + v_length / 2. * v(),
                                       origin() - u_length / 2. * u() - v_length / 2. * v() ) );
      lines.push_back( std::make_pair( origin() - u_length / 2. * u() - v_length / 2. * v(),
                                       origin() + u_length / 2. * u() - v_length / 2. * v() ) );
      lines.push_back( std::make_pair( origin() + u_length / 2. * u() - v_length / 2. * v(),
                                       origin() + u_length / 2. * u() + v_length / 2. * v() ) );

      return lines;
    }
  };

  typedef VolSurfaceHandle<ReadoutPlaneImp> ReadoutPlane;

  /// detector builder plugin entry
  Ref_t create_element( Detector& description, xml_h e, SensitiveDetector sens ) {
    xml_comp_t                x_det( e );
    int                       det_id   = x_det.id();
    string                    det_name = x_det.nameStr();
    xml::tools::VolumeBuilder builder( description, e, sens );
    DetElement                sdet( builder.detector );
    DetectorBuildType         build_type = description.buildType();
    SensitiveDetector         sd         = sens;

    // --- set type of detector ---------------------
    if ( x_det.hasAttr( _U( sensitive ) ) )
      sens.setType( x_det.attr<string>( _U( sensitive ) ) );
    else
      sdet.setType( "compound" );

    // --- check segmentation type ---------------------
    bool is_stripSeg = false;
    if ( sd.readout().segmentation().type() == "MultiSegmentation" ) { is_stripSeg = true; }

    // --- create an envelope volume and position it into the world ---------------------
    Volume envelope = xml::createPlacedEnvelope( description, e, sdet );
    envelope.setVisAttributes( description.invisible() ); // force hiding
    sdet.placement()->SetName( det_name.c_str() );        // force removing copyno and '_envelope' suffix

    // --- create the drift volume and anode surface ---------------------
    xml_comp_t x_driftvol = x_det.child( _Unicode( driftvol ) );
    Volume     drift_vol  = xml::createStdVolume( description, x_driftvol );
    if ( build_type == DetectorBuildType::BUILD_SIMU ) drift_vol.setSensitiveDetector( sd );

    bool        drift_useRot = false;
    bool        drift_usePos = false;
    Position    drift_pos;
    RotationZYX drift_rot;
    if ( x_driftvol.hasChild( _U( position ) ) ) {
      drift_usePos       = true;
      xml_comp_t env_pos = x_driftvol.position();
      drift_pos          = Position( env_pos.x(), env_pos.y(), env_pos.z() );
    }
    if ( x_driftvol.hasChild( _U( rotation ) ) ) {
      drift_useRot       = true;
      xml_comp_t env_rot = x_driftvol.rotation();
      drift_rot          = RotationZYX( env_rot.z(), env_rot.y(), env_rot.x() );
    }

    PlacedVolume drift_pv;
    if ( drift_useRot && drift_usePos ) {
      drift_pv = envelope.placeVolume( drift_vol, Transform3D( drift_rot, drift_pos ) );
    } else if ( drift_useRot ) {
      drift_pv = envelope.placeVolume( drift_vol, drift_rot );
    } else if ( drift_usePos ) {
      drift_pv = envelope.placeVolume( drift_vol, drift_pos );
    } else {
      drift_pv = envelope.placeVolume( drift_vol );
    }
    drift_pv->SetName( x_driftvol.nameStr().c_str() ); // remove copy number suffix
    DetElement drift_de( sdet, x_driftvol.nameStr(), det_id );
    drift_de.setPlacement( drift_pv );

    xml_comp_t x_anode = x_driftvol.child( _Unicode( anode_surface ) );
    xml_dim_t  x_ao    = x_anode.child( _Unicode( origin ) );
    xml_dim_t  x_an    = x_anode.child( _Unicode( normal ) );
    // u, v, inner/outer thickness are not really important
    Vector3D u{ 1., 0., 0. }, v{ 0., 1., 0. };
    double   outer_thickness{ 0.1 };
    double   inner_thickness{ 0.1 };
    // only n, o are critical, they define the drift space
    Vector3D n{ x_an.x(), x_an.y(), x_an.z() };
    Vector3D o{ x_ao.x(), x_ao.y(), x_ao.z() };
    Type     asurf_type( Type::Plane );
    asurf_type.setProperty( Type::Helper, true );
    if ( x_anode.isSensitive() ) { asurf_type.setProperty( Type::Sensitive, true ); }

    VolPlane anode_surf( drift_vol, asurf_type, inner_thickness, outer_thickness, u, v, n, o );
    volSurfaceList( drift_de )->push_back( anode_surf );

    // --- create the readout PCBs (for segmentation usage) ---------------------
    if ( x_det.hasChild( _Unicode( readout_pcbs ) ) ) {
      xml_comp_t x_pcbs   = x_det.child( _Unicode( readout_pcbs ) );
      string     pcb_name = x_pcbs.nameStr();
      Volume     pcb_vol  = xml::createStdVolume( description, x_pcbs );
      if ( build_type == DetectorBuildType::BUILD_RECO ) { pcb_vol.setSensitiveDetector( sd ); }

      int    nrow    = 1;
      int    ncol    = 1;
      double row_gap = 0.;
      double col_gap = 0.;
      if ( x_pcbs.hasChild( _U( row ) ) ) {
        xml_dim_t x_row = x_pcbs.child( _U( row ) );
        nrow            = x_row.nmodules();
        row_gap         = x_row.gap();
      }
      if ( x_pcbs.hasChild( _Unicode( column ) ) ) {
        xml_dim_t x_col = x_pcbs.child( _Unicode( column ) );
        ncol            = x_col.nmodules();
        col_gap         = x_col.gap();
      }

      bool        useRot = false;
      bool        usePos = false;
      Position    pos;
      RotationZYX rot;
      if ( x_pcbs.hasChild( _U( position ) ) ) {
        usePos             = true;
        xml_comp_t env_pos = x_pcbs.position();
        pos                = Position( env_pos.x(), env_pos.y(), env_pos.z() );
      }
      if ( x_pcbs.hasChild( _U( rotation ) ) ) {
        useRot             = true;
        xml_comp_t env_rot = x_pcbs.rotation();
        rot                = RotationZYX( env_rot.z(), env_rot.y(), env_rot.x() );
      }

      auto place_pcb = [&]( Volume pcb ) -> PlacedVolume {
        PlacedVolume pv;
        if ( useRot && usePos ) {
          pv = envelope.placeVolume( pcb, Transform3D( rot, pos ) );
        } else if ( useRot ) {
          pv = envelope.placeVolume( pcb, rot );
        } else if ( usePos ) {
          pv = envelope.placeVolume( pcb, pos );
        } else {
          pv = envelope.placeVolume( pcb );
        }
        pv->SetName( pcb.name() ); // remove copy number suffix
        return pv;
      };

      Vector3D u{ 1., 0., 0. }, v{ 0., 1., 0. }, n{ 0, 0, 1 }, o{ 0, 0, 0 };
      double   u_len{ 0 }, v_len{ 0 };
      double   outer_thickness{ 0.1 }, inner_thickness{ 0.1 };
      {
        xml_comp_t x_surf = x_pcbs.child( _Unicode( surface ) );
        u_len             = x_surf.attr<double>( _Unicode( dim_u ) );
        v_len             = x_surf.attr<double>( _Unicode( dim_v ) );
        xml_dim_t x_o     = x_surf.child( _Unicode( origin ) );
        xml_dim_t x_n     = x_surf.child( _Unicode( normal ) );
        xml_dim_t x_udir  = x_surf.child( _Unicode( u_direction ) );
        xml_dim_t x_vdir  = x_surf.child( _Unicode( v_direction ) );
        u                 = { x_udir.x(), x_udir.y(), x_udir.z() };
        v                 = { x_vdir.x(), x_vdir.y(), x_vdir.z() };
        n                 = { x_n.x(), x_n.y(), x_n.z() };
        o                 = { x_o.x(), x_o.y(), x_o.z() };
      }

      auto attach_pcb_surf = [&]( DetElement de ) -> ReadoutPlane {
        ReadoutPlane pcb_surf( pcb_vol, Type( Type::Plane, Type::Sensitive ), inner_thickness, outer_thickness, u, v, n,
                               o );
        pcb_surf->setData( u_len, v_len );
        volSurfaceList( de )->push_back( pcb_surf );
        return pcb_surf;
      };

      if ( nrow == 1 && ncol == 1 ) {
        PlacedVolume pv = place_pcb( pcb_vol );
        DetElement   de( sdet, x_pcbs.nameStr(), det_id );
        de.setPlacement( pv );
        attach_pcb_surf( de );
      } else if ( is_stripSeg ) {
        printout( FATAL, "Megat_OnesideTpc", "+++ Do not support strip readout with multiple PCBs" );
        throw std::runtime_error( "+++ Failed to create one-side TPC: " + det_name );
      } else {
        Assembly   pcb_env( pcb_name + "_assembly" );
        DetElement pcb_env_de( sdet, pcb_env.name(), det_id );

        auto   bbox    = pcb_vol.boundingBox();
        double xunit   = 2 * bbox.x();
        double yunit   = 2 * bbox.y();
        double xoffset = -( xunit * ncol + col_gap * ( ncol - 1 ) ) / 2 + xunit / 2;
        double yoffset = -( yunit * nrow + row_gap * ( nrow - 1 ) ) / 2 + yunit / 2;
        for ( int r_id = 0; r_id < nrow; r_id++ ) {
          for ( int c_id = 0; c_id < ncol; c_id++ ) {
            int          pcb_id = r_id * ncol + c_id;
            PlacedVolume pcb_pv = pcb_env.placeVolume(
                pcb_vol, Position( xoffset + c_id * ( xunit + col_gap ), yoffset + r_id * ( yunit + row_gap ), 0 ) );
            pcb_pv.addPhysVolID( "pcb", pcb_id );

            DetElement pcb_de( pcb_env_de, pcb_pv.name(), pcb_id );
            pcb_de.setPlacement( pcb_pv );
            auto surf = attach_pcb_surf( pcb_de );
            surf->setID( pcb_id );
          }
        }
        PlacedVolume pv = place_pcb( pcb_env );
        pcb_env_de.setPlacement( pv );
      }
    }

    // --- all other components ---------------------
    builder.buildShapes( x_det );
    builder.buildVolumes( x_det );
    builder.placeDaughters( sdet, envelope, x_det );

    return sdet;
  }
} // namespace

DECLARE_DETELEMENT( Megat_OneSideTpc, create_element )
