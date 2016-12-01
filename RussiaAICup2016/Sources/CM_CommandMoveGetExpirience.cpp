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

CommandMoveGetExpirience::CommandMoveGetExpirience() {
}


bool CommandMoveGetExpirience::check(const Wizard& self) {
  target = nullptr;

  static const double expirienceRadius = 550;
  double minLive = 100000;

  const auto selfPos = Position(self.getX(), self.getY());

  for (const auto& enemy : World::instance().aroundEnemies(self, expirienceRadius + 200)) {

    if (enemy->getLife() < minLive) {
      const auto enemyPos = Position(enemy->getX(), enemy->getY());
      const double distance = (selfPos - enemyPos).length();
      if (distance > expirienceRadius) {
        target = enemy;
        minLive = enemy->getLife();
      }
    }
  }

  if (nullptr == target) {
    return false;
  }

  if (target->getLife() > 24) {
    return false;
  }


  return true;
}

void CommandMoveGetExpirience::execute(const Wizard& self, Result& result) {
  assert(nullptr != target);

  result.set(EX::pos(*target), self);
  result.turnStyle = TurnStyle::TURN;
  result.turnPriority = TurnPriority::getExpirience;
}

double CommandMoveGetExpirience::priority(const Wizard& self) {
  return MovePriorities::getExpirience(self, *target);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveGetExpirience::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  assert(nullptr != target);

  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);
    const auto pos = EX::pos(*target);

    visualizator.line(from.x, from.y, pos.x, pos.y, 0xffff00);
    visualizator.fillCircle(pos.x, pos.y, 10, 0xffff00);
  }
}
#endif // ENABLE_VISUALIZATOR