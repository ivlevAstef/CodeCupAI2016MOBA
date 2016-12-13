#include "A_WinPredictor.h"
#include "C_Math.h"
#include "E_InfluenceMap.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Extensions.h"

using namespace AICup;

double Algorithm::changeOfWinning(const Wizard& self, const double x, const double y) {
  auto pos = EX::pos(self);
  if (x > 0 && y > 0) {
    pos = Position(x, y);
  }

  double wizardSummaryDanger = 0;
  for (const auto& wizard : World::instance().wizards()) {
    if ((EX::pos(wizard) - pos).length() < self.getVisionRange() + 250 || self.getId() == wizard.getId()) {
      if (self.getId() == wizard.getId()) {
        wizardSummaryDanger += EX::danger(wizard);
      } else if (self.getFaction() == wizard.getFaction()) {
        wizardSummaryDanger += self.getRole().getFriendWizardConfidence() * EX::danger(wizard);
      } else {
        wizardSummaryDanger -= EX::danger(wizard);
      }
    }
  }

  /// определяем перевес сил, так как маги мало на него влияют то, он по факту сообщит чьих крипов больше, с учетом вышек
  float strength = InfluenceMap::instance().getStrength(pos, self.getVisionRange()+250) / 5;

  double priority = wizardSummaryDanger + double(strength);

  /// по факту можно расписать так - если сил на полтора мага первого уровня больше, то шанс на победу (одной из сторон) очень большой.
  return INTERVAL(-1, priority / 30, 1);
}