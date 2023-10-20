#pragma once

#include "GaudiKernel/IService.h"

class G4Event;

/** @class ISimSvc SimInterface/SimInterface/ISimSvc.h ISimSvc.h
 *
 *  Interface to the main Geant simulation service.
 *
 *  @author Anna Zaborowska
 */

class ISimSvc : virtual public IService {
public:
  DeclareInterfaceID( ISimSvc, 1, 0 );
  /**  Simulate the event with Geant.
   *   @param[in] aEvent An event to be processed.
   *   @return status code
   */
  virtual StatusCode processEvent( G4Event& aEvent ) = 0;
  /**  Retrieve the processed event.
   *   @param[out] aEvent The processed event.
   *   @return status code
   */
  virtual StatusCode retrieveEvent( G4Event*& aEvent ) = 0;
  /**  Terminate the event simulation.
   *   @return status code
   */
  virtual StatusCode terminateEvent() = 0;
};
