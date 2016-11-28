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
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "C_Extensions.h"

using namespace AICup;

CommandMoveGetExpirience::CommandMoveGetExpirience(Algorithm::PathFinder& finder):
  MoveCommand(finder) {
}


bool CommandMoveGetExpirience::check(const Wizard& self) {
  unit = nullptr;
  followCommand = nullptr;

  static const double expirienceRadius = 600 - 10/*на всякий случай*/;
  double minLive = 100000;

  const auto selfPos = Position(self.getX(), self.getY());

  for (const auto& enemy : World::instance().aroundEnemies(self, expirienceRadius + 200)) {

    if (enemy->getLife() < minLive) {
      const auto enemyPos = Position(enemy->getX(), enemy->getY());
      const double distance = (selfPos - enemyPos).length();
      if (distance > expirienceRadius) {
        unit = enemy;
        minLive = enemy->getLife();
      }
    }
  }

  if (nullptr == unit) {
    return false;
  }

  if (unit->getLife() > 24) {
    return false;
  }


  followCommand = std::make_shared<CommandFollow>(pathFinder, unit->getId(), 0, expirienceRadius);
  return followCommand->check(self);
}

void CommandMoveGetExpirience::execute(const Wizard& self, Result& result) {
  assert(nullptr != followCommand.get());
  followCommand->execute(self, result);
  result.turnPriority = TurnPriority::getExpirience;
}

double CommandMoveGetExpirience::priority(const Wizard& self) {
  return MovePriorities::getExpirience(self, *unit);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveGetExpirience::visualization(const model::Wizard&, const Visualizator& visualizator) const {
  assert(nullptr != followCommand.get());

  if (Visualizator::POST == visualizator.getStyle()) {
    const auto pos = EX::pos(*unit);
    visualizator.fillCircle(pos.x, pos.y, 10, 0xffff00);
  }
}
#endif // ENABLE_VISUALIZATOR