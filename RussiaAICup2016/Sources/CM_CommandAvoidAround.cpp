//
//File: CM_CommandAvoidAround.cpp
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#include "CM_CommandAvoidAround.h"
#include "CM_CommandAvoidEnemy.h"
#include "E_World.h"
#include "C_Math.h"
#include "CM_Move.h"

using namespace AICup;

CommandAvoidAround::CommandAvoidAround() {

}

bool CommandAvoidAround::check(const model::Wizard& self) {
  enemies.clear();

  const auto selfPos = Position(self.getX(), self.getY());


  for (const auto& enemy : World::instance().aroundEnemies(self, self.getVisionRange() + self.getRadius() * 2)) {
    auto avoidCommand = CommandAvoidEnemy(enemy->getId());
    if (avoidCommand.check(self)) {
      const auto priority = avoidCommand.priority(self);
      const auto enemyPos = Position(enemy->getX(), enemy->getY());
      const auto distance = (selfPos - enemyPos).length();

      if (distance < avoidCommand.distance) {
        enemies.push_back(EnemyData{Position(enemy->getX(), enemy->getY()), priority, avoidCommand.distance});
      }
    }
  }

  return !enemies.empty();
}

int CommandAvoidAround::priority(const model::Wizard& self) {
  return 0;
}


void CommandAvoidAround::execute(const model::Wizard& self, model::Move& move) {
  const auto selfPos = Position(self.getX(), self.getY());

  Vector summaryMoveVector;
  double summaryPriority = 0;

  for (const auto& enemy : enemies) {
    const double priority = enemy.priority / 1000.0;
    const Vector vec = selfPos - enemy.pos;
    const Vector moveVector = vec.normal() * (enemy.minDistance - vec.length()) * priority;

    summaryPriority += priority;
    summaryMoveVector += moveVector;
  }

  summaryMoveVector /= summaryPriority;

  const auto pos = selfPos + summaryMoveVector.normal() * self.getVisionRange();
  const auto obstaclesGroups = World::instance().obstaclesGroup(self);

  const auto action = Move::move(self, pos, obstaclesGroups, SPEED_LIMIT_NOT_SET, MOVE_WITH_BACKWARD_ROTATE);

  move.setSpeed(action.speed);
  move.setStrafeSpeed(action.strafeSpeed);
  move.setTurn(action.turn);
}


#ifdef ENABLE_VISUALIZATOR
void CommandAvoidAround::visualization(const Visualizator& visualizator) const {

}
#endif // ENABLE_VISUALIZATOR