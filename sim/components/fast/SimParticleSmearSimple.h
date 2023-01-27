#ifndef SIMG4FAST_G4PARTICLESMEARSIMPLE_H
#define SIMG4FAST_G4PARTICLESMEARSIMPLE_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

// FCCSW
#include "SimInterface/ISimParticleSmearTool.h"

namespace megat {

  /** @class SimParticleSmearSimple SimFast/src/components/SimParticleSmearSimple.h SimParticleSmearSimple.h
   *
   *  Simple particle smearing tool.
   *  Smears the momentum of the particle following the Gaussian distribution.
   *  The standard deviation of the Gaussian is set in the job options file ('sigma').
   *  [For more information please see](@ref md_sim_doc_geant4fastsim).
   *
   *  @author Anna Zaborowska
   */

  class SimParticleSmearSimple : public GaudiTool, virtual public ISimParticleSmearTool {
  public:
    explicit SimParticleSmearSimple( const std::string& type, const std::string& name, const IInterface* parent );
    virtual ~SimParticleSmearSimple();

    /**  Initialize the tool and a random number generator.
     *   @return status code
     */
    virtual StatusCode initialize() final;
    /**  Finalize.
     *   @return status code
     */
    virtual StatusCode finalize() final;

    /**  Smear the momentum of the particle
     *   @param aMom Particle momentum to be smeared.
     *   @param[in] aPdg Particle PDG code.
     *   @return status code
     */
    virtual StatusCode smearMomentum( CLHEP::Hep3Vector& aMom, int aPdg = 0 ) final;

    /**  Check conditions of the smearing model, especially if the given parametrs do not exceed the parameters of the
     * model.
     *   @param[in] aMinMomentum Minimum momentum.
     *   @param[in] aMaxMomentum Maximum momentum.
     *   @param[in] aMaxEta Maximum pseudorapidity.
     *   @return status code
     */
    inline virtual StatusCode checkConditions( double, double, double ) const final { return StatusCode::SUCCESS; }

  private:
    /// Random Number Service
    IRndmGenSvc* m_randSvc;
    /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
    Rndm::Numbers m_gauss;
    /// Constant resolution for the smearing (set by job options)
    Gaudi::Property<double> m_sigma{ this, "sigma", 0.01, "Constant resolution for the smearing" };
  };
} // namespace megat

#endif /* SIMG4FAST_G4PARTICLESMEARSIMPLE_H */
