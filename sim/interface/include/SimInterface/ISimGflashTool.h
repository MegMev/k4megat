#ifndef SIMG4INTERFACE_ISIMG4GFLASHTOOL_H
#define SIMG4INTERFACE_ISIMG4GFLASHTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant
#include "GVFlashShowerParameterisation.hh"

/** @class ISimGflashTool SimInterface/SimInterface/ISimGflashTool.h ISimGflashTool.h
 *
 *  Interface to the Gflash parametrisation tool.
 *  It returns the parametriation that should be attached to the GFlashShowerModel.
 *
 *  @author Anna Zaborowska
 */

class ISimGflashTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISimGflashTool, 1, 0);

  /**  Get the parametrisation
   *   @return unique pointer to the parametrisation
   */
  virtual std::unique_ptr<GVFlashShowerParameterisation> parametrisation() = 0;
};
#endif /* SIMG4INTERFACE_ISIMG4GFLASHTOOL_H */
