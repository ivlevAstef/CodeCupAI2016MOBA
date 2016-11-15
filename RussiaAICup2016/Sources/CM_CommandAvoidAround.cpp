//
//File: CM_CommandAvoidAround.cpp
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#include "CM_CommandAvoidAround.h"
#include "CM_CommandAvoidEnemy.h"
#include "CM_CommandFollow.h"
#include "E_World.h"
#include "C_Math.h"

using namespace AICup;

CommandAvoidAround::CommandAvoidAround() {

}

bool CommandAvoidAround::check(const model::Wizard& self) {

  const int threshold = 850 * (self.getLife() / self.getMaxLife());

  const auto selfPos = Position(self.getX(), self.getY());

  const model::LivingUnit* followUnit = nullptr;
  double followMinDinstance = 0;
  double followMaxDinstance = 0;
  double minDistance = 100000;

  for (const auto& enemy : World::instance().aroundEnemies(self)) {
    auto avoidCommand = CommandAvoidEnemy(enemy->getId());
    if (avoidCommand.check(self)) {
      const auto priority = avoidCommand.priority(self);
      if (priority > threshold) {
        const auto enemyPos = Position(enemy->getX(), enemy->getY());
        const auto distance = (enemyPos - selfPos).length() - avoidCommand.maxDistance;
        if (distance < minDistance) {
          minDistance = distance;
          followUnit = enemy;
          followMinDinstance = avoidCommand.minDistance;
          followMaxDinstance = avoidCommand.maxDistance;
        }
      }
    }
  }

  if (nullptr == followUnit) {
    return false;
  }

  keepDistance = std::make_shared<CommandFollow>(followUnit->getId(), followMinDinstance, followMaxDinstance);

  return keepDistance->check(self);
}

int CommandAvoidAround::priority(const model::Wizard& self) {
  return keepDistance->priority(self);
}


void CommandAvoidAround::execute(const model::Wizard& self, model::Move& move) {
  assert(nullptr != keepDistance.get());
  keepDistance->execute(self, move);
}


#ifdef ENABLE_VISUALIZATOR
void CommandAvoidAround::visualization(const Visualizator& visualizator) const {
  keepDistance->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR