//
//File: CM_CommandAttackEnemy.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandAttackEnemy.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"

using namespace AICup;

CommandAttackEnemy::CommandAttackEnemy(long long enemyId): enemyId(enemyId) {

}

bool CommandAttackEnemy::check(const model::Wizard& self, model::Move& move) {
  const auto enemy = World::instance().unit(enemyId);
  if (nullptr == enemy) {
    return false;
  }

  selfPos = Position(self.getX(), self.getY());
  const auto enemyPos = Position(enemy->getX(), enemy->getY());
  const double distance = (selfPos - enemyPos).length();

  /// если враг совсем далеко, то атака невозможна
  if (distance > self.getCastRange()) {
    return false;
  }

  /// во всех остальных случаях возможна или атака, или поворот к юниту для атаки
  return true;
}

void CommandAttackEnemy::execute(const model::Wizard& self, model::Move& move) {
  const auto enemy = World::instance().unit(enemyId);
  assert(nullptr != enemy);

  const auto enemyPos = Position(enemy->getX(), enemy->getY());
  const auto dir = enemyPos - selfPos;

  const double angleDeviation = Math::angleDiff(dir.angle(), self.getAngle());

  /// в любом случае поворачиваемся к врагу, для упрощения своей жизни
  move.setTurn(angleDeviation);

  /// не тот угол, чтобы атаковать
  if (abs(angleDeviation) > Game::instance().model().getStaffSector()) {
    return;
  }

  move.setCastAngle(angleDeviation);

  const double distance = dir.length();

  /// если дистанция с врагом такая, что можно его ударить посохом, то бьем посохом
  if (distance < Game::instance().model().getStaffRange() - enemy->getRadius()) {
    move.setAction(model::ACTION_STAFF);
  } else {
    move.setMinCastDistance(distance - enemy->getRadius());
    move.setMaxCastDistance(distance + enemy->getRadius());
    move.setAction(model::ACTION_MAGIC_MISSILE);
  }
}

#ifdef ENABLE_VISUALIZATOR
void CommandAttackEnemy::visualization(const Visualizator& visualizator) const {
  const auto enemy = World::instance().unit(enemyId);
  assert(nullptr != enemy);

  visualizator.line(selfPos.x, selfPos.y, enemy->getX(), enemy->getY(), 0xff0000);
  visualizator.fillCircle(enemy->getX(), enemy->getY(), enemy->getRadius() * 0.75, 0xff0000);
}
#endif // ENABLE_VISUALIZATOR