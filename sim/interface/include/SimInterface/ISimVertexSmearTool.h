#pragma once

// Gaudi
#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/IAlgTool.h"

class ISimVertexSmearTool : virtual public IAlgTool {
public:
  DeclareInterfaceID( ISimVertexSmearTool, 1, 0 );

  /*  return the smeared vertex with the assumption origin vertex (0, 0, 0) */
  virtual CLHEP::Hep3Vector smearVertex() = 0;
};
