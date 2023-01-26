#ifndef SIMG4INTERFACE_ISIMG4REGIONTOOL_H
#define SIMG4INTERFACE_ISIMG4REGIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class ISimRegionTool SimInterface/SimInterface/ISimRegionTool.h ISimRegionTool.h
 *
 *  Interface to the tool creating region.
 *
 *  @author Anna Zaborowska
 */

class ISimRegionTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISimRegionTool, 1, 0);

  /**  Create region.
   *   @return status code
   */
  virtual StatusCode create() = 0;
};
#endif /* SIMG4INTERFACE_ISIMG4REGIONTOOL_H */
