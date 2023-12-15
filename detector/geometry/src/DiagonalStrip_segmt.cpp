#include "DDSegmentation/Segmentation.h"
#include "SegmentationUtils.h"

#include <stdexcept>
#include <string>
#include <vector>

/*
 * Description:
 * Segment the readout plane with two sets of +45/-45 degree equal-distance lines.
 * Then, chaining along the pixels along the same diagonal line as one strip readout.
 * The default bin number scheme is the same as CartesianStripX/Y segmentation.
 *
 * Motivation:
 * It is the most common readout pattern of USTC's Micromegas
 *
 * Warning:
 * Guard-pads on the edge (a.k.a. half-sized pads) are treated as normal full-size pads.
 * [todo] solution needed
 *
 * Author:
 * Yong Zhou (zhouyong@ustc.edu.cn)
 *
 */

namespace {
  /* Useful utilities */

  // Transform center-based coordinates to edge-based coordinates
  // - position: position in the center-origin coordinate system
  // - cellSize: cell width
  // - offset: edge position in the center-origin coordinate system
  double toEdge( double position, double cellSize, double offset ) { return ( position - 0.5 * cellSize - offset ); }

  // Normalize the physical coordinates
  // - xpos, ypos:  hit position coordinates
  // - cellSize: square-diagonal length
  int toNormX( double xpos, double ypos, double cellSize ) { return std::floor( ( ypos - xpos ) / cellSize ); }
  int toNormY( double xpos, double ypos, double cellSize ) { return std::floor( ( ypos + xpos ) / cellSize ); }

  // Transform from normalized coordinate id to strip bin number
  // - xid, yid: return value of toNormX/Y
  int toBinX( int xid, int yid ) { return ( ( yid - xid ) >> 1 ); }
  int toBinY( int xid, int yid ) { return ( ( yid + xid + 1 ) >> 1 ); }

} // namespace

namespace {
  using namespace dd4hep::DDSegmentation;

  class DiagonalStrip : public Segmentation {
  public:
    DiagonalStrip( const std::string aCellEncoding = "" ) : Segmentation( aCellEncoding ) { init(); }
    DiagonalStrip( const BitFieldCoder* aDecoder ) : Segmentation( aDecoder ) { init(); }
    virtual ~DiagonalStrip() {}

    CellID   cellID( const Vector3D& aLocalPosition, const Vector3D& aGlobalPosition, const VolumeID& aVolumeID ) const;
    Vector3D position( const CellID& aCellID ) const;
    std::vector<double> cellDimensions( const CellID& cellID ) const { return { m_stripSize }; }
    void                neighbours( const CellID& cellID, std::set<CellID>& neighbours ) const;

  private:
    void init();

  private:
    double m_stripSize;
    double m_offsetX;
    double m_offsetY;
    int    m_idLayerX;
    int    m_idLayerY;

    std::string m_nameLayer;
    std::string m_nameStrip;
  };

  //
  inline void DiagonalStrip::init() {
    _type        = "DiagonalStrip";
    _description = "Diagonal pixel segmentation with X/Y pads chained into strips";

    registerParameter( "strip_size", "Strip width (i.e. diagonal length of pixel)", m_stripSize, 1.,
                       SegmentationParameter::LengthUnit );
    registerParameter( "offset_x", "Strip offset in X axis", m_offsetX, 0., SegmentationParameter::LengthUnit, true );
    registerParameter( "offset_y", "Strip offset in Y axis", m_offsetY, 0., SegmentationParameter::LengthUnit, true );
    registerParameter( "layer_id_x", "ID number of X layer", m_idLayerX, 0, SegmentationParameter::NoUnit, true );
    registerParameter( "layer_id_y", "ID number of Y layer", m_idLayerY, 1, SegmentationParameter::NoUnit, true );

    registerIdentifier( "identifier_layer", "Identifier for layer ID", m_nameLayer, "layer" );
    registerIdentifier( "identifier_strip", "Identifier for strip ID", m_nameStrip, "strip" );
  }

  //
  CellID DiagonalStrip::cellID( const Vector3D& aLocalPosition, const Vector3D& aGlobalPosition,
                                const VolumeID& aVolumeID ) const {
    auto newX  = toEdge( aLocalPosition.X, m_stripSize, m_offsetX );
    auto newY  = toEdge( aLocalPosition.Y, m_stripSize, m_offsetY );
    auto normX = toNormX( newX, newY, m_stripSize );
    auto normY = toNormY( newX, newY, m_stripSize );

    CellID newID = aVolumeID;
    int    newBin;
    // if same parity, it's a hit in x-strip
    if ( sameParity( normX, normY ) ) {
      newBin = toBinX( normX, normY );
      _decoder->set( newID, m_nameLayer, m_idLayerX );
    } else { // otherwrise, it's a hit in y-strip
      newBin = toBinY( normX, normY );
      _decoder->set( newID, m_nameLayer, m_idLayerY );
    }
    _decoder->set( newID, m_nameStrip, newBin );

    return newID;
  }

  //
  Vector3D DiagonalStrip::position( const CellID& aCellID ) const {
    Vector3D cellPosition;
    auto     layer_id = _decoder->get( aCellID, m_nameLayer );

    if ( layer_id == m_idLayerX ) {
      cellPosition.X =
          binToPosition( _decoder->get( aCellID, m_nameStrip ), m_stripSize, m_offsetX + 0.5 * m_stripSize );

    } else if ( layer_id == m_idLayerY ) {
      cellPosition.Y =
          binToPosition( _decoder->get( aCellID, m_nameStrip ), m_stripSize, m_offsetY + 0.5 * m_stripSize );
    }

    return cellPosition;
  }

  //
  void DiagonalStrip::neighbours( const CellID& cellID, std::set<CellID>& neighbours ) const {
    CellID nID          = cellID;
    int    currentValue = _decoder->get( cellID, m_nameStrip );
    // add both neighbouring cell IDs, don't add out of bound indices
    try {
      _decoder->set( nID, m_nameStrip, currentValue - 1 );
      neighbours.insert( nID );
    } catch ( std::runtime_error& e ) {
      // nothing to do
    }
    try {
      _decoder->set( nID, m_nameStrip, currentValue + 1 );
      neighbours.insert( nID );
    } catch ( std::runtime_error& e ) {
      // nothing to do
    }
  }

} // namespace

DECLARE_SEGMENTATION( DiagonalStrip, create_segmentation<::DiagonalStrip> )
