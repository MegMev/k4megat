#include "Analysis/IdConverter.h"

namespace megat {
  namespace utility {
    using edm4hep::Vector3f;

    IdConverter::IdConverter( std::string tag ) {
      auto& detector = Detector::getInstance( tag );
      if ( detector.state() != Detector::State::READY ) {
        throw std::runtime_error( "Load the detector geometry first" );
      }
      m_volumeManager = VolumeManager::getVolumeManager( detector );
      m_description   = &detector;
    }

    //
    Vector3f IdConverter::position( const CellID& cell ) const {
      double l[3], e[3], g[3];

      const VolumeManagerContext* context = findContext( cell );
      if ( context == NULL ) return { 0, 0, 0 };

      auto det   = context->element;
      auto r     = findReadout( det );
      auto seg   = r.segmentation();
      auto local = seg.position( cell );
      local.GetCoordinates( l );

      const TGeoMatrix& volToElement = context->toElement();
      volToElement.LocalToMaster( l, e );

      const TGeoMatrix& elementToGlobal = det.nominal().worldTransformation();
      elementToGlobal.LocalToMaster( e, g );

      // it's ugly, but can't find better alt
      return { static_cast<float>( g[0] ), static_cast<float>( g[1] ), static_cast<float>( g[2] ) };
    }

    //
    std::vector<double> IdConverter::cellDimensions( const CellID& cell ) const {
      auto context = findContext( cell );
      if ( context == nullptr ) return {};
      auto r   = findReadout( context->element );
      auto seg = r.segmentation();
      return seg.cellDimensions( cell );
    }

    //
    CellID IdConverter::cellID( const Vector3f& global ) const {
      CellID result( 0 );

      TGeoManager* geoManager = m_description->world().volume()->GetGeoManager();
      PlacedVolume pv         = geoManager->FindNode( global.x, global.y, global.z );

      if ( pv.isValid() && pv.volume().isSensitive() ) {
        TGeoHMatrix* m    = geoManager->GetCurrentMatrix();
        double       g[3] = { global.x, global.y, global.z };
        double       l[3];
        m->MasterToLocal( g, l );

        SensitiveDetector sd = pv.volume().sensitiveDetector();
        Readout           r  = sd.readout();

        // collect all volIDs for the current path
        PlacedVolume::VolIDs volIDs;
        volIDs.insert( std::end( volIDs ), std::begin( pv.volIDs() ), std::end( pv.volIDs() ) );

        TGeoPhysicalNode pN( geoManager->GetPath() );

        unsigned motherCount = 0;

        while ( pN.GetMother( motherCount ) != NULL ) {
          PlacedVolume mPv = pN.GetMother( motherCount++ );
          if ( mPv.isValid() && pN.GetMother( motherCount ) != NULL ) // world has no volIDs
            volIDs.insert( std::end( volIDs ), std::begin( mPv.volIDs() ), std::end( mPv.volIDs() ) );
        }
        VolumeID volIDPVs = r.idSpec().encode( volIDs );

        result = r.segmentation().cellID( { l[0], l[1], l[2] }, { g[0], g[1], g[2] }, volIDPVs );
      }

      return result;
    }

    //
    const VolumeManagerContext* IdConverter::findContext( const CellID& cellID ) const {
      return m_volumeManager.lookupContext( cellID );
    }

    //
    Readout IdConverter::findReadout( const DetElement& det ) const {
      // first check if top level is a sensitive detector
      if ( det.volume().isValid() and det.volume().isSensitive() ) {
        SensitiveDetector sd = det.volume().sensitiveDetector();
        if ( sd.isValid() and sd.readout().isValid() ) { return sd.readout(); }
      }

      // if not, return the first sensitive daughter volume's readout
      Readout r = findReadout( det.placement() );
      if ( r.isValid() ) return r;

      // nothing found !?
      return Readout();
    }

    Readout IdConverter::findReadout( const PlacedVolume& pv ) const {
      // first check if we are in a sensitive volume
      if ( pv.volume().isSensitive() ) {
        SensitiveDetector sd = pv.volume().sensitiveDetector();
        if ( sd.isValid() and sd.readout().isValid() ) { return sd.readout(); }
      }

      for ( Int_t idau = 0, ndau = pv->GetNdaughters(); idau < ndau; ++idau ) {
        PlacedVolume dpv = pv->GetDaughter( idau );
        Readout      r   = findReadout( dpv );
        if ( r.isValid() ) return r;
      }
      return Readout();
    }

    bool IdConverter::isStrip( std::string tpc_name ) {
      auto it = m_multiseg.find( tpc_name );
      if ( it != m_multiseg.end() ) return it->second;

      auto tpc_sd = m_description->sensitiveDetector( tpc_name );
      auto tpc_ro = tpc_sd.readout();
      if ( tpc_ro.segmentation().type() == "MultiSegmentation" ) {
        m_multiseg.emplace( tpc_name, true );
      } else {
        m_multiseg.emplace( tpc_name, false );
      }
      return m_multiseg[tpc_name];
    }

    BitFieldCoder* IdConverter::decoder( std::string det_name ) {
      auto it = m_decoders.find( det_name );
      if ( it != m_decoders.end() ) { return it->second; }

      auto tpc_sd  = m_description->sensitiveDetector( det_name );
      auto decoder = tpc_sd.idSpec().decoder();
      m_decoders.emplace( det_name, decoder );

      return decoder;
    }

  } // namespace utility
} // namespace megat
