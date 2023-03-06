#pragma once

#include "DD4hep/DD4hepUnits.h"
#include "G4SystemOfUnits.hh"
#include <CLHEP/Units/SystemOfUnits.h>

/** Conversion between units.
 *
 *  Contains conversions between default units used by EDM, by Geant and by DD4hep
 *  CLHEP/Geant4 default units are MeV, mm, ns
 *  DD4hep default are MeV, cm, s (can be changed to G4 units by build option DD4HEP_USE_GEANT4_UNITS)
 *  Currently, Megat's EDM defaults are GeV, mm, ns
 *
 *  This header define the conversion factors by using CLHEP's system.
 *
 *  [todo] change default to MeV, mm and us when refactoring sim engine?
 *
 *  @author Yong Zhou
 */
namespace megat {
  namespace edmdefault {
    constexpr double length = CLHEP::mm;
    constexpr double energy = CLHEP::GeV;
    constexpr double time   = CLHEP::ns;
  } // namespace edmdefault
  namespace edm2g4 {
    constexpr double length = edmdefault::length / CLHEP::mm;
    constexpr double energy = edmdefault::energy / CLHEP::MeV;
    constexpr double time   = edmdefault::time / CLHEP::ns;
  } // namespace edm2g4
  namespace g42edm {
    constexpr double length = CLHEP::mm / edmdefault::length;
    constexpr double energy = CLHEP::MeV / edmdefault::energy;
    constexpr double time   = CLHEP::ns / edmdefault::time;
  } // namespace g42edm
  namespace dd2edm {
    constexpr double length = dd4hep::mm / edmdefault::length;
    constexpr double energy = dd4hep::GeV / edmdefault::energy;
    constexpr double time   = dd4hep::ns / edmdefault::time;
  } // namespace dd2edm
  namespace edm2dd {
    constexpr double length = edmdefault::length / CLHEP::mm;
    constexpr double energy = edmdefault::energy / CLHEP::GeV;
    constexpr double time   = edmdefault::time / CLHEP::ns;
  } // namespace edm2dd
} // namespace megat
