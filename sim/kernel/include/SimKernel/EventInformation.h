#pragma once

#include "G4VUserEventInformation.hh"
#include <cstdint>
#include <iostream>
#include <map>

class G4Track;
namespace edm4hep {
  class MCParticleCollection;
}

namespace megat {

  /* Additional event information.
   *
   * Currently holds:
   * - the particle history in form of edm particles and vertices
   * - the type of interaction: Compton or not
   *
   */

  namespace sim {
    enum EventType { Compton = 0, Pair, PhotoElectric, Invalid = 32 };

    class EventInformation : public G4VUserEventInformation {
    public:
      EventInformation();
      virtual ~EventInformation() = default;

      /// Set external pointers to point at the particle and vertex collections.
      void setCollections( edm4hep::MCParticleCollection*& aMcParticleCollection );
      /// Add a particle to be tracked in the EDM collections
      void addParticle( const G4Track* aSecondary );

      /// Set the event type bits
      inline void setEventType( int newtype ) { m_evtType = newtype; }

      void setComptonScatter( bool bitval ) { setEventType( setBit( m_evtType, EventType::Compton, bitval ) ); }
      bool isComptonScatter() { return getBit( m_evtType, EventType::Compton ); }

      void setPairProduction( bool bitval ) { setEventType( setBit( m_evtType, EventType::Pair, bitval ) ); }
      bool isPairProduction() { return getBit( m_evtType, EventType::Pair ); }

      void setPhotoElectric( bool bitval ) { setEventType( setBit( m_evtType, EventType::PhotoElectric, bitval ) ); }
      bool isPhotoElectric() { return getBit( m_evtType, EventType::PhotoElectric ); }

      ///
      void Print() const {};

    private:
      inline int  setBit( int val, int num, bool bitval ) { return ( val & ~( 1 << num ) ) | ( bitval << num ); }
      inline bool getBit( int val, int num ) { return ( val & ( 1 << num ) ); }

    private:
      /// Pointer to the particle collection, ownership is intended to be transfered to SaveTool
      edm4hep::MCParticleCollection* m_mcParticles;
      /// Map to get the edm end vertex id from a Geant4 unique particle ID
      // std::map<size_t, size_t> m_g4IdToEndVertexMap;

      /// Event type (bit-segments)
      int m_evtType{ 0 };
    };
  } // namespace sim
} // namespace megat
