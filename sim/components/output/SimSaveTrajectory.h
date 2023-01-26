#ifndef SIMG4COMPONENTS_G4SAVETRAJECTORY
#define SIMG4COMPONENTS_G4SAVETRAJECTORY

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "k4FWCore/DataHandle.h"
#include "SimInterface/ISimSaveOutputTool.h"
class IGeoSvc;

// datamodel
namespace edm4hep {
class TrackerHitCollection;
}

/** @class SimSaveTrajectory SimComponents/src/SimSaveTrajectory.h SimSaveTrajectory.h
 *
 * Tool to save Geant4 Trajectory data. Requires Geant to be run with the command "/tracking/storeTrajectory 1".
 *  Note that access to trajectories is expensive, so this tool should only be used for debugging and visualisation.
 *
 */

class SimSaveTrajectory : public GaudiTool, virtual public ISimSaveOutputTool {
public:
  explicit SimSaveTrajectory(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimSaveTrajectory();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save the data output.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) final;

private:
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Handle for trajectory hits including position information
  DataHandle<edm4hep::TrackerHitCollection> m_trackHits{"Hits/Trajectory",
                                                                      Gaudi::DataHandle::Writer, this};
};

#endif /* SIMG4COMPONENTS_G4SAVETRAJECTORY */
