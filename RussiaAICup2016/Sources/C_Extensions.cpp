#include "C_Extensions.h"
#include "E_Game.h"

#include "model/Wizard.h"
#include "model/Minion.h"

using namespace AICup;

double Extension::maxSpeed(const model::CircularUnit& obj) {
  const auto livingObj = dynamic_cast<const model::LivingUnit*>(&obj);
  if (nullptr == livingObj) {
    return 0;
  }

  const auto wizard = dynamic_cast<const model::Wizard*>(&obj);
  const auto minion = dynamic_cast<const model::Minion*>(&obj);

  double maxSpeed = 0;
  if (nullptr != wizard) {
    maxSpeed = Game::instance().model().getWizardForwardSpeed();
  } else if (nullptr != minion) {
    maxSpeed = Game::instance().model().getMinionSpeed();
  }


  for (const auto& status : livingObj->getStatuses()) {
    if (model::STATUS_HASTENED == status.getType()) {
      maxSpeed *= (1.0 + Game::instance().model().getHastenedMovementBonusFactor());
    }
  }

  return maxSpeed;
}