#ifndef SIMG4FULL_FULLSIMACTIONS_H
#define SIMG4FULL_FULLSIMACTIONS_H

#include "G4VUserActionInitialization.hh"

/** @class FullSimActions SimFull/SimFull/FullSimActions.h FullSimActions.h
 *
 *  User action initialization for full simulation.
 *
 *  @author A. Zaborowska
 *  @author J. Lingemann (adding particle history)
 */
namespace megat {

  namespace sim {
    class FullSimActions : public G4VUserActionInitialization {
    public:
      FullSimActions( bool enableHistory, double aEnergyCut );
      virtual ~FullSimActions();
      /// Create all user actions.
      virtual void Build() const final;

    private:
      /// Flag whether or not to store particle history
      bool m_enableHistory;
      /// energy threshold for secondaries to be saved
      double m_energyCut;
    };
  } // namespace sim
} // namespace megat

#endif /* SIMG4FULL_FULLSIMACTIONS_H */
