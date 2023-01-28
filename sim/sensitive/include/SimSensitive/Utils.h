#ifndef MEGAT_SIM_UTILS_H
#define MEGAT_SIM_UTILS_H

// DD4hep
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Segmentations.h"

// Geant
#include "G4Step.hh"

#ifdef HAVE_GEANT4_UNITS
#  define MM_2_CM 1.0
#else
#  define MM_2_CM 0.1
#endif

namespace megat {
  namespace utils {

    /** Retrieves the cellID based on the position of the step and the detector
     * segmentation.
     *  @param aSeg Handle to the segmentation of the volume.
     *  @param aStep Step in which particle deposited the energy.
     *  @param aPreStepPoint Flag indicating if the position of the deposit is the
     * beginning of the step (default) or the middle of the step.
     */
    uint64_t cellID( const dd4hep::Segmentation& aSeg, const G4Step& aStep, bool aPreStepPoint = true );

  } // namespace utils
} // namespace megat

#endif
