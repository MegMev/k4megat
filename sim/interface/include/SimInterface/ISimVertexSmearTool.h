#pragma once

// Gaudi
#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/IAlgTool.h"

class ISimVertexSmearTool : virtual public IAlgTool {
public:
  DeclareInterfaceID( ISimVertexSmearTool, 1, 0 );

  /**  Smear the vertex of the particle
   *   @param aVertex Particle vertex to be smeared.
   *   @return status code
   */
  virtual StatusCode smearVertex( CLHEP::Hep3Vector& aVertex ) = 0;
};
