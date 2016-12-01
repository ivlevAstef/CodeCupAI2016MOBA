#include "CM_CommandCastShield.h"
#include "E_Game.h"

using namespace AICup;

CommandCastShield::CommandCastShield() {
}

bool CommandCastShield::check(const Wizard& self) {
  /// ���� ����������
  if (!self.availableAction(model::ACTION_SHIELD)) {
    return false;
  }

  /// ����� � ��������
  if (self.cooldown(model::ACTION_SHIELD) > 0) {
    return false;
  }

  // ��� ����
  if (self.getMana() < Game::model().getShieldManacost()) {
    return false;
  }

  /// ��� � ��� ��� �����
  for (const auto& status : self.getStatuses()) {
    if (status.getType() == model::STATUS_SHIELDED) {
      return false;
    }
  }

  return true;
}


void CommandCastShield::execute(const Wizard& self, Result& result) {
  result.id = self.getId();
  result.action = model::ACTION_SHIELD;
}

double CommandCastShield::priority(const Wizard& self) {
  return 1000;
}