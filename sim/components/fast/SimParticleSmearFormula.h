#ifndef SIMG4FAST_G4PARTICLESMEARFORMULA_H
#define SIMG4FAST_G4PARTICLESMEARFORMULA_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;
class IRndmGen;

// ROOT
#include "TFormula.h"

// FCCSW
#include "SimInterface/ISimParticleSmearTool.h"

namespace megat {

  /** @class SimParticleSmearFormula SimFast/src/components/SimParticleSmearFormula.h SimParticleSmearFormula.h
   *
   *  Formula particle smearing tool.
   *  The resolution dependence can be expressed by an arbitrary formula in the configuration.
   *  Smears momentum of the particle following a Gaussian distribution, using the evaluated formula as the mean.
   *  [For more information please see](@ref md_sim_doc_geant4fastsim).
   *
   *  @author Anna Zaborowska
   */

  class SimParticleSmearFormula : public GaudiTool, virtual public ISimParticleSmearTool {
  public:
    explicit SimParticleSmearFormula( const std::string& type, const std::string& name, const IInterface* parent );
    virtual ~SimParticleSmearFormula();

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
    /// TFormula representing resolution momentum-dependent for the smearing
    TFormula m_resolutionMomentum;
    /// Random Number Service
    SmartIF<IRndmGenSvc> m_randSvc;
    /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
    IRndmGen* m_gauss;
    /// string defining a TFormula representing resolution momentum-dependent for the smearing (set by job options)
    Gaudi::Property<std::string> m_resolutionMomentumStr{
        this, "resolutionMomentum", "",
        "string defining a TFormula representing resolution momentum-dependent for the smearing" };
  };
} // namespace megat

#endif /* SIMG4FAST_G4PARTICLESMEARSIMPLE_H */
