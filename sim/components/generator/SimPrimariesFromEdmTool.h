#ifndef SIMG4COMPONENTS_G4PRIMARIESFROMEDMTOOL_H
#define SIMG4COMPONENTS_G4PRIMARIESFROMEDMTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "k4FWCore/DataHandle.h"

#include "SimInterface/ISimEventProviderTool.h"

#include "G4VUserPrimaryGeneratorAction.hh"

// Forward declarations
// datamodel
namespace edm4hep {
  class MCParticleCollection;
}

namespace megat {

  /** @class SimPrimariesFromEdmTool SimPrimariesFromEdmTool.h "SimPrimariesFromEdmTool.h"
   *
   *  Tool to translate an EDM MCParticleCollection into a G4Event
   *
   *  @author A. Zaborowska, J. Lingemann, A. Dell'Acqua
   *  @date   2016-01-11
   */
  class SimPrimariesFromEdmTool : public GaudiTool, virtual public ISimEventProviderTool {
  public:
    /// Standard constructor
    SimPrimariesFromEdmTool( const std::string& type, const std::string& name, const IInterface* parent );

    virtual ~SimPrimariesFromEdmTool();

    StatusCode initialize() final;

    /// Translates the input (edm4hep::MCParticleCollection) into a G4Event
    /// @returns G4Event with primaries generated from MCParticleCollection (ownership is transferred to the caller)
    virtual G4Event* g4Event() final;

  private:
    /// Handle for the EDM MC particles to be read
    DataHandle<edm4hep::MCParticleCollection> m_genParticles{ "GenParticles", Gaudi::DataHandle::Reader, this };
  };

} // namespace megat

#endif
