#ifndef SIMG4INTERFACE_ISIMG4SAVEOUTPUTTOOL_H
#define SIMG4INTERFACE_ISIMG4SAVEOUTPUTTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant
class G4Event;

/** @class ISimSaveOutputTool SimInterface/SimInterface/ISimSaveOutputTool.h ISimSaveOutputTool.h
 *
 *  Interface to the output saving tool.
 *
 *  @author Anna Zaborowska
 */

class ISimSaveOutputTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISimSaveOutputTool, 1, 0);

  /**  Save the data output.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) = 0;
};
#endif /* SIMG4INTERFACE_ISIMG4SAVEOUTPUTTOOL_H */
