#include "CM_CommandCastShield.h"
#include "E_Game.h"
#include "E_World.h"

using namespace AICup;

CommandCastShield::CommandCastShield() {
}

bool CommandCastShield::check(const Wizard& self) {
  /// скил недоступен
  if (!self.availableAction(model::ACTION_SHIELD)) {
    return false;
  }

  /// скилл в кулдауне
  if (self.cooldown(model::ACTION_SHIELD) > 0) {
    return false;
  }

  // нет маны
  if (self.getMana() < Game::model().getShieldManacost()) {
    return false;
  }

  /// уже маны достаточно на прокаст двух, то может быть положим на своих
  if (self.getMana() >= 2 * Game::model().getShieldManacost()) {
    return true;
  }

  for (const auto& status : self.getStatuses()) {
    if (status.getType() == model::STATUS_SHIELDED) {
      return false;
    }
  }

  return true;
}


void CommandCastShield::execute(const Wizard& self, Result& result) {
  result.unit = &self;
  result.action = model::ACTION_SHIELD;
  result.priority = 1000;

  for (const auto& wizard : World::model().getWizards()) {
    if (wizard.getFaction() != self.getFaction() || wizard.getDistanceTo(self) > self.getCastRange()) {
      continue;
    }

    bool found = false;
    for (const auto& status : self.getStatuses()) {
      if (status.getType() == model::STATUS_SHIELDED) {
        found = true;
        continue;
      }
    }

    if (!found) {
      result.unit = &wizard;
      return;
    }
  }


  /// накладывать только на себя не имеет смысла, если уже есть этот статус
  for (const auto& status : self.getStatuses()) {
    if (status.getType() == model::STATUS_SHIELDED) {
      result.priority = 0;
    }
  }
}