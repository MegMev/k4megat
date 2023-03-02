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

using namespace std;
using namespace dd4hep;
using namespace dd4hep::rec;
using namespace dd4hep::detail;
using Type = dd4hep::rec::SurfaceType;

static Ref_t create_element( Detector& description, xml_h e, SensitiveDetector sens ) {
  xml_comp_t                x_det( e );
  int                       det_id   = x_det.id();
  string                    det_name = x_det.nameStr();
  xml::tools::VolumeBuilder builder( description, e, sens );
  DetElement                sdet( builder.detector );

  // --- set type of detector ---------------------
  if ( x_det.hasAttr( _U( sensitive ) ) )
    sens.setType( x_det.attr<string>( _U( sensitive ) ) );
  else
    sdet.setType( "compound" );

  // --- create an envelope volume and position it into the world ---------------------
  Volume envelope = xml::createPlacedEnvelope( description, e, sdet );
  envelope.setVisAttributes( description.invisible() ); // force hiding
  sdet.placement()->SetName( det_name.c_str() );        // force removing copyno and '_envelope' suffix

  // --- create the drift volume and anode surface ---------------------
  xml_comp_t x_driftvol = e.child( _Unicode( driftvol ) );
  Volume     drift_vol  = xml::createStdVolume( description, x_driftvol );
  drift_vol.setSensitiveDetector( sens );

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

  xml_h     x_anode = x_driftvol.child( _Unicode( anode_surface ) );
  xml_dim_t x_ao    = x_anode.child( _Unicode( origin ) );
  xml_dim_t x_an    = x_anode.child( _Unicode( normal ) );
  // u, v, inner/outer thickness are not really important
  Vector3D u{ 1., 0., 0. }, v{ 0., 1., 0. };
  double   outer_thickness{ 0.1 };
  double   inner_thickness{ 0.1 };
  // only n, o are critical, they define the drift space
  Vector3D n{ x_an.x(), x_an.y(), x_an.z() };
  Vector3D o{ x_ao.x(), x_ao.y(), x_ao.z() };
  VolPlane anode_surf( drift_vol, Type( Type::Plane, Type::Helper ), inner_thickness, outer_thickness, u, v, n, o );
  volSurfaceList( drift_de )->push_back( anode_surf );

  // --- all other components ---------------------
  builder.buildShapes( x_det );
  builder.buildVolumes( x_det );
  builder.placeDaughters( sdet, envelope, x_det );

  return sdet;
}
DECLARE_DETELEMENT( Megat_SimpleTpc, create_element )
