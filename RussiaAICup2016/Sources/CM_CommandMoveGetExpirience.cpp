//
//File: CM_CommandMoveGetExpirience.cpp
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#include "CM_CommandMoveGetExpirience.h"
#include "CM_CommandFollow.h"
#include "E_World.h"
#include "A_Move.h"
#include "E_Game.h"

using namespace AICup;

CommandMoveGetExpirience::CommandMoveGetExpirience() {
}


bool CommandMoveGetExpirience::check(const model::Wizard& self) {
  static const double expirienceRadius = 600 - 10/*на всякий случай*/;
  const model::LivingUnit* selectedUnit = nullptr;
  double minLive = 100000;


  const auto selfPos = Position(self.getX(), self.getY());

  for (const auto& enemy : World::instance().aroundEnemies(self, expirienceRadius + 200)) {

    if (enemy->getLife() < minLive) {
      const auto enemyPos = Position(enemy->getX(), enemy->getY());
      const double distance = (selfPos - enemyPos).length();
      if (distance > expirienceRadius) {
        selectedUnit = enemy;
        minLive = enemy->getLife();
      }
    }
  }

  if (nullptr == selectedUnit) {
    return false;
  }

  if (selectedUnit->getLife() > 24) {
    return false;
  }


  expiriencePos = Position(selectedUnit->getX(), selectedUnit->getY());
  followCommand = std::make_shared<CommandFollow>(selectedUnit->getId(), 0, expirienceRadius);
  return followCommand->check(self);
}

int CommandMoveGetExpirience::priority(const model::Wizard& self) {
  assert(nullptr != followCommand.get());
  return followCommand->priority(self);
}

void CommandMoveGetExpirience::execute(const model::Wizard& self, Result& result) {
  assert(nullptr != followCommand.get());
  followCommand->execute(self, result);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveGetExpirience::visualization(const Visualizator& visualizator) const {
  assert(nullptr != followCommand.get());

  visualizator.fillCircle(expiriencePos.x, expiriencePos.y, 10, 0xffff00);
}
#endif // ENABLE_VISUALIZATOR