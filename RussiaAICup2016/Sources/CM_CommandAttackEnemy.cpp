//
//File: CM_CommandAttackEnemy.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandAttackEnemy.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_AttackPriorities.h"

using namespace AICup;

CommandAttackEnemy::CommandAttackEnemy(long long enemyId): enemyId(enemyId) {

}

bool CommandAttackEnemy::check(const Wizard& self) {
  const auto enemy = World::instance().unit(enemyId);
  if (nullptr == enemy) {
    return false;
  }

  selfPos = Position(self.getX(), self.getY());
  const auto enemyPos = Position(enemy->getX(), enemy->getY());
  const double distance = (selfPos - enemyPos).length();

  /// ���� ���� ������ ������, �� ����� ����������
  if (distance > EX::radiusForGuaranteedHit(self)) {
    return false;
  }

  /// ���� ������� �� ����� ������ ��� ������� �� ���������, �� ����� ���� �� ���������
  if (self.getRemainingActionCooldownTicks() > EX::timeToTurnForAttack(*enemy, self)) {
    return false;
  }

  /// ���� �� ���� ���� ������, �� �� ����� ���������
  for (const auto& tree : World::instance().trees()) {
    if (tree.getId() == enemy->getId()) {
      continue;
    }

    if (Math::distanceToSegment(EX::pos(tree), selfPos, enemyPos) < tree.getRadius() + 10/*������ �������*/) {
      return false;
    }
  }

  /// �� ���� ��������� ������� �������� ��� �����, ��� ������� � ����� ��� �����
  return true;
}


void CommandAttackEnemy::execute(const Wizard& self, Result& result) {
  const auto enemy = World::instance().unit(enemyId);
  assert(nullptr != enemy);

  const double distance = self.getDistanceTo(*enemy);

  result.unit = enemy;
  result.priority = AttackPriorities::attackEnemy(self, *enemy);

  if (distance < Game::model().getStaffRange() + enemy->getRadius() - EX::maxSpeed(*enemy)) {
    result.action = model::ACTION_STAFF;
  } else {
    result.action = model::ACTION_MAGIC_MISSILE;
  }
}

#ifdef ENABLE_VISUALIZATOR
void CommandAttackEnemy::visualization(const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto enemy = World::instance().unit(enemyId);
    assert(nullptr != enemy);

    visualizator.line(selfPos.x, selfPos.y, enemy->getX(), enemy->getY(), 0xff0000);
    visualizator.fillCircle(enemy->getX(), enemy->getY(), enemy->getRadius() * 0.75, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR