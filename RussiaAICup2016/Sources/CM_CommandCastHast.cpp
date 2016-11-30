#include "CM_CommandCastHast.h"
#include "E_Game.h"

using namespace AICup;

CommandCastHast::CommandCastHast() {
}

bool CommandCastHast::check(const Wizard& self) {
  /// ���� ����������
  if (!self.availableAction(model::ACTION_HASTE)) {
    return false;
  }

  /// ����� � ��������
  if (self.cooldown(model::ACTION_HASTE) > 0) {
    return false;
  }

  // ��� ����
  if (self.getMana() < Game::model().getHasteManacost()) {
    return false;
  }

  /// ��� � ��� ��� ������
  for (const auto& status : self.getStatuses()) {
    if (status.getType() == model::STATUS_HASTENED) {
      return false;
    }
  }

  return true;
}


void CommandCastHast::execute(const Wizard& self, Result& result) {
  result.id = self.getId();
  result.action = model::ACTION_HASTE;
}

double CommandCastHast::priority(const Wizard& self) {
  return 1000;
}