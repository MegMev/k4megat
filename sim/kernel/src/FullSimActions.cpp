#include "SimG4Common/FullSimActions.h"
#include "SimG4Common/ParticleHistoryAction.h"
#include "SimG4Common/ParticleHistoryEventAction.h"
#include <iostream>

namespace sim {
FullSimActions::FullSimActions(bool enableHistory, double aEnergyCut)
    : G4VUserActionInitialization(), m_enableHistory(enableHistory), m_energyCut(aEnergyCut) {}

FullSimActions::~FullSimActions() {}

void FullSimActions::Build() const {
  if (m_enableHistory) {
    SetUserAction(new ParticleHistoryEventAction());
    SetUserAction(new ParticleHistoryAction(m_energyCut));
  }
}
}
