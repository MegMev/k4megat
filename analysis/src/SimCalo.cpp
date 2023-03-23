#include "Analysis/SimCalo.h"
// #include "Analysis/Utils.h"

#include <ROOT/RVec.hxx>
// #include <math.h>

#include "DD4hep/Detector.h"

namespace megat {

  using namespace ROOT::VecOps;
  using HitData = edm4hep::SimCalorimeterHitData;

  namespace SimCalo {
    //
    dd4hep::DDSegmentation::BitFieldCoder* m_decoder;

    void loadGeometry( std::string xmlGeometryPath, std::string readoutName ) {
      dd4hep::Detector* dd4hepgeo = &( dd4hep::Detector::getInstance() );
      dd4hepgeo->fromCompact( xmlGeometryPath );
      dd4hepgeo->volumeManager();
      dd4hepgeo->apply( "DD4hepVolumeManager", 0, 0 );
      m_decoder = dd4hepgeo->readout( readoutName ).idSpec().decoder();
    }

    // calo hit
    // RVec<float> getHit_y( const RVec<HitData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.position.y ); }
    //   return result;
    // }

    // RVec<float> getHit_z( const RVec<HitData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.position.z ); }
    //   return result;
    // }

    // RVec<float> getHit_phi( const RVec<HitData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) {
    //     TVector3 t3;
    //     t3.SetXYZ( p.position.x, p.position.y, p.position.z );
    //     result.push_back( t3.Phi() );
    //   }
    //   return result;
    // }

    // RVec<int> getHit_phiBin( const RVec<HitData>& in ) {
    //   RVec<int> result;
    //   for ( auto& p : in ) {
    //     dd4hep::DDSegmentation::CellID cellId = p.cellID;
    //     result.push_back( m_decoder->get( cellId, "phi" ) );
    //   }
    //   return result;
    // }

    // RVec<float> getHit_theta( const RVec<HitData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) {
    //     TVector3 t3;
    //     t3.SetXYZ( p.position.x, p.position.y, p.position.z );
    //     result.push_back( t3.Theta() );
    //   }
    //   return result;
    // }

    // RVec<float> getHit_eta( const RVec<HitData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) {
    //     TVector3 t3;
    //     t3.SetXYZ( p.position.x, p.position.y, p.position.z );
    //     result.push_back( t3.Eta() );
    //   }
    //   return result;
    // }

    // RVec<int> getHit_etaBin( const RVec<HitData>& in ) {
    //   RVec<int> result;
    //   for ( auto& p : in ) {
    //     dd4hep::DDSegmentation::CellID cellId = p.cellID;
    //     result.push_back( m_decoder->get( cellId, "eta" ) );
    //   }
    //   return result;
    // }

    // RVec<float> getHit_energy( const RVec<HitData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.energy ); }
    //   return result;
    // }

    // RVec<int> getHit_layer( const RVec<HitData>& in ) {
    //   RVec<int> result;
    //   for ( auto& p : in ) {
    //     dd4hep::DDSegmentation::CellID cellId = p.cellID;
    //     result.push_back( m_decoder->get( cellId, "layer" ) );
    //   }
    //   return result;
    // }

    // RVec<TVector3> getHit_positionVector3( const RVec<HitData>& in ) {
    //   RVec<TVector3> result;
    //   for ( auto& p : in ) {
    //     TVector3 t3;
    //     t3.SetXYZ( p.position.x, p.position.y, p.position.z );
    //     result.push_back( t3 );
    //   }
    //   return result;
    // }

    // // calo cluster
    // RVec<float> getCaloCluster_x( const RVec<ClusterData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.position.x ); }
    //   return result;
    // }

    // RVec<float> getCaloCluster_y( const RVec<ClusterData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.position.y ); }
    //   return result;
    // }

    // RVec<float> getCaloCluster_z( const RVec<ClusterData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.position.z ); }
    //   return result;
    // }

    // RVec<float> getCaloCluster_phi( const RVec<ClusterData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) {
    //     TVector3 t3;
    //     t3.SetXYZ( p.position.x, p.position.y, p.position.z );
    //     result.push_back( t3.Phi() );
    //   }
    //   return result;
    // }

    // RVec<float> getCaloCluster_theta( const RVec<ClusterData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) {
    //     TVector3 t3;
    //     t3.SetXYZ( p.position.x, p.position.y, p.position.z );
    //     result.push_back( t3.Theta() );
    //   }
    //   return result;
    // }

    // RVec<float> getCaloCluster_eta( const RVec<ClusterData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) {
    //     TVector3 t3;
    //     t3.SetXYZ( p.position.x, p.position.y, p.position.z );
    //     result.push_back( t3.Eta() );
    //   }
    //   return result;
    // }

    // RVec<float> getCaloCluster_energy( const RVec<ClusterData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.energy ); }
    //   return result;
    // }

    // RVec<TVector3> getCaloCluster_positionVector3( const RVec<ClusterData>& in ) {
    //   RVec<TVector3> result;
    //   for ( auto& p : in ) {
    //     TVector3 t3;
    //     t3.SetXYZ( p.position.x, p.position.y, p.position.z );
    //     result.push_back( t3 );
    //   }
    //   return result;
    // }

    // RVec<int> getCaloCluster_firstCell( const RVec<ClusterData>& in ) {
    //   RVec<int> result;
    //   for ( auto& p : in ) { result.push_back( p.hits_begin ); }
    //   return result;
    // }

    // RVec<int> getCaloCluster_lastCell( const RVec<ClusterData>& in ) {
    //   RVec<int> result;
    //   for ( auto& p : in ) { result.push_back( p.hits_end ); }
    //   return result;
    // }

    // RVec<std::vector<float>> getCaloCluster_energyInLayers( const RVec<ClusterData>& in, const RVec<HitData>& cells,
    //                                                         const int nLayers ) {
    //   static const int         layer_idx = m_decoder->index( "layer" );
    //   static const int         cryo_idx  = m_decoder->index( "cryo" );
    //   RVec<std::vector<float>> result;
    //   result.reserve( in.size() );

    //   for ( const auto& c : in ) {
    //     std::vector<float> energies( nLayers, 0 );
    //     for ( auto i = c.hits_begin; i < c.hits_end; i++ ) {
    //       int layer  = m_decoder->get( cells[i].cellID, layer_idx );
    //       int cryoID = m_decoder->get( cells[i].cellID, cryo_idx );
    //       if ( cryoID == 0 ) { energies[layer] += cells[i].energy; }
    //     }
    //     result.push_back( energies );
    //   }
    //   return result;
    // }

    // RVec<float> getSimParticleSecondaries_x( const RVec<MCParticleData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.vertex.x ); }
    //   return result;
    // }

    // RVec<float> getSimParticleSecondaries_y( const RVec<MCParticleData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.vertex.y ); }
    //   return result;
    // }

    // RVec<float> getSimParticleSecondaries_z( const RVec<MCParticleData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.vertex.z ); }
    //   return result;
    // }

    // RVec<float> getSimParticleSecondaries_PDG( const RVec<MCParticleData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) { result.push_back( p.PDG ); }
    //   return result;
    // }

    // RVec<float> getSimParticleSecondaries_phi( const RVec<MCParticleData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) {
    //     TLorentzVector tlv;
    //     tlv.SetXYZM( p.momentum.x, p.momentum.y, p.momentum.z, p.mass );
    //     result.push_back( tlv.Phi() );
    //   }
    //   return result;
    // }

    // RVec<float> getSimParticleSecondaries_theta( const RVec<MCParticleData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) {
    //     TLorentzVector tlv;
    //     tlv.SetXYZM( p.momentum.x, p.momentum.y, p.momentum.z, p.mass );
    //     result.push_back( tlv.Theta() );
    //   }
    //   return result;
    // }

    // RVec<float> getSimParticleSecondaries_eta( const RVec<MCParticleData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) {
    //     TLorentzVector tlv;
    //     tlv.SetXYZM( p.momentum.x, p.momentum.y, p.momentum.z, p.mass );
    //     result.push_back( tlv.Eta() );
    //   }
    //   return result;
    // }

    // RVec<float> getSimParticleSecondaries_energy( const RVec<MCParticleData>& in ) {
    //   RVec<float> result;
    //   for ( auto& p : in ) {
    //     TLorentzVector tlv;
    //     tlv.SetXYZM( p.momentum.x, p.momentum.y, p.momentum.z, p.mass );
    //     result.push_back( tlv.E() );
    //   }
    //   return result;
    // }

  } // namespace SimCalo

} // namespace megat
