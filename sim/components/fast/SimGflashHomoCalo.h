#ifndef SIMG4FAST_SIMG4GFLASHHOMOCALO_H
#define SIMG4FAST_SIMG4GFLASHHOMOCALO_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimInterface/ISimGflashTool.h"

/** @class SimGflashHomoCalo SimFast/src/components/SimGflashHomoCalo.h SimGflashHomoCalo.h
 *
 *  Tool creating a parametrisation of a homogenous calorimeter.
 *  The original parameters from arXiv:hep-ex/0001020v1 are taken.
 *  Material of the calorimeter is set in a property '\b material'.
 *
 *  @author Anna Zaborowska
*/

class SimGflashHomoCalo : public GaudiTool, virtual public ISimGflashTool {
public:
  explicit SimGflashHomoCalo(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimGflashHomoCalo();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /**  Get the parametrisation
   *   @return unique pointer to the parametrisation
   */
  virtual std::unique_ptr<GVFlashShowerParameterisation> parametrisation() final;

private:
  /// Material name of the homogenous calorimeter (to be searched for in Geant NIST table)
  Gaudi::Property<std::string> m_material{
      this, "material", "", "Material name of the homogenous calorimeter (to be searched for in Geant NIST table)"};
};

#endif /* SIMG4FAST_SIMG4GFLASHHOMOCALO_H */
