//
//File: CM_CommandAvoidAround.cpp
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#include "CM_CommandAvoidAround.h"
#include "CM_CommandAvoidEnemy.h"
#include "CM_CommandKeepDistance.h"
#include "E_World.h"
#include "C_Math.h"

using namespace AICup;

CommandAvoidAround::CommandAvoidAround() {

}

bool CommandAvoidAround::check(const model::Wizard& self) {
  aroundEnemies.clear();

  const int threshold = 850 * (self.getLife() / self.getMaxLife());

  for (const auto& enemy : World::instance().aroundEnemies(self)) {
    auto avoidCommand = CommandAvoidEnemy(enemy->getId());
    if (avoidCommand.check(self)) {
      const auto priority = avoidCommand.priority(self);
      if (priority > threshold) {
        const auto enemyPos = Position(enemy->getX(), enemy->getY());
        aroundEnemies.push_back(EnemyData{enemyPos, priority, avoidCommand.minDistance, avoidCommand.maxDistance});
      }
    }
  }

  Position dangerCenter;
  double dangerPriority = 0;
  double minDistance = 0;
  double maxDistance = 0;

  for (const auto& data : aroundEnemies) {
    dangerCenter += data.pos * (data.priority / 100.0);
    dangerPriority += data.priority / 100.0;

    minDistance = MAX(minDistance, data.minDistance);
    maxDistance = MAX(maxDistance, data.maxDistance);
  }

  dangerCenter /= dangerPriority;

  keepDistance = std::make_shared<CommandKeepDistance>(dangerCenter.x, dangerCenter.y, minDistance, maxDistance);

  return keepDistance->check(self);
}

int CommandAvoidAround::priority(const model::Wizard& self) {
  return 0;
}


void CommandAvoidAround::execute(const model::Wizard& self, model::Move& move) {
  assert(nullptr != keepDistance.get());
  keepDistance->execute(self, move);
}


#ifdef ENABLE_VISUALIZATOR
void CommandAvoidAround::visualization(const Visualizator& visualizator) const {
  for (const auto& enemy : aroundEnemies) {
    visualizator.text(enemy.pos.x - 20, enemy.pos.y - 40, enemy.priority, 0xff9999);
    visualizator.circle(enemy.pos.x, enemy.pos.y, enemy.minDistance, 0xff9999);
    visualizator.circle(enemy.pos.x, enemy.pos.y, enemy.maxDistance, 0xff9999);
  }
  keepDistance->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR