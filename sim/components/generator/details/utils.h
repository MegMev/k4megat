#pragma once

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <cmath>

namespace {
  double electron_mass = 0.510998910; // MeV

  //
  inline CLHEP::Hep3Vector isotropicVector() {
    double a, b, c;
    double n;
    do {
      a = CLHEP::RandFlat::shoot( -1, 1 );
      b = CLHEP::RandFlat::shoot( -1, 1 );
      c = CLHEP::RandFlat::shoot( -1, 1 );
      n = a * a + b * b + c * c;
    } while ( n > 1 || n == 0.0 );

    n = std::sqrt( n );
    a /= n;
    b /= n;
    c /= n;
    return { a, b, c };
  }
} // namespace
