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

bool CommandAttackEnemy::check(const model::Wizard& self) {
  const auto enemy = World::instance().unit(enemyId);
  if (nullptr == enemy) {
    return false;
  }

  selfPos = Position(self.getX(), self.getY());
  const auto enemyPos = Position(enemy->getX(), enemy->getY());
  const double distance = (selfPos - enemyPos).length();

  /// ���� ���� ������ ������, �� ����� ����������
  if (distance > self.getCastRange()) {
    return false;
  }

  /// �� ���� ��������� ������� �������� ��� �����, ��� ������� � ����� ��� �����
  return true;
}

int CommandAttackEnemy::priority(const model::Wizard& self) {
  const auto enemy = World::instance().unit(enemyId);
  if (nullptr == enemy) {
    return false;
  }

  const double lifePriority = double(enemy->getMaxLife()-enemy->getLife()) / double(enemy->getMaxLife());

  const auto wizardEnemy = static_cast<const model::Wizard*>(enemy);
  const auto minionEnemy = static_cast<const model::Minion*>(enemy);
  const auto buildEnemy = static_cast<const model::Building*>(enemy);

  double typePriority = 0;
  if (nullptr != wizardEnemy) {
    typePriority = 1;
  } else if (nullptr != minionEnemy) {
    switch (minionEnemy->getType()) {
      case model::MINION_ORC_WOODCUTTER:
        typePriority = 0.3;
        break;
      case model::MINION_FETISH_BLOWDART:
        typePriority = 0.5;
        break;
    }
  } else if (nullptr != buildEnemy) {
    typePriority = 0.6;
  }

  double statusPriority = 0.5;
  for (const auto& status : enemy->getStatuses()) {
    switch (status.getType()) {
      case model::STATUS_BURNING:
        statusPriority += 0.1;
      case model::STATUS_EMPOWERED:
        statusPriority += 0.2;
      case model::STATUS_FROZEN:
        statusPriority += 1.0;
      case model::STATUS_HASTENED:
        statusPriority -= 0.3;
      case model::STATUS_SHIELDED:
        statusPriority -= 1.0;
    }
  }
  statusPriority = MAX(0, MIN(statusPriority, 1));



  return 1000 * (0.3 * lifePriority + 0.6 * typePriority + 0.1 * statusPriority);
}

void CommandAttackEnemy::execute(const model::Wizard& self, model::Move& move) {
  const auto enemy = World::instance().unit(enemyId);
  assert(nullptr != enemy);

  const auto enemyPos = Position(enemy->getX(), enemy->getY());
  const auto dir = enemyPos - selfPos;

  const double angleDeviation = Math::angleDiff(dir.angle(), self.getAngle());

  /// � ����� ������ �������������� � �����, ��� ��������� ����� �����
  move.setTurn(angleDeviation);

  /// �� ��� ����, ����� ���������
  if (abs(angleDeviation) > Game::instance().model().getStaffSector()) {
    return;
  }

  move.setCastAngle(angleDeviation);

  const double distance = dir.length();

  /// ���� ��������� � ������ �����, ��� ����� ��� ������� �������, �� ���� �������
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