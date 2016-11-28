#include "CM_MovePriorities.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include "A_Attack.h"

using namespace AICup;

///������ ����
double MovePriorities::avoidBuild(const Wizard& self, const model::Building& build) {
  const int lifePriority = (300 * build.getLife()) / build.getMaxLife();
  return 900 + lifePriority;
}

double MovePriorities::avoidMinion(const Wizard& self, const model::Minion& minion) {
  const auto mc = Game::model();

  const auto selfPos = EX::pos(self);
  const auto enemyPos = EX::pos(minion);
  const double distance = (selfPos - enemyPos).length();

  const double lifePriority = (300 * minion.getLife()) / minion.getMaxLife();

  if (model::MINION_ORC_WOODCUTTER == minion.getType()) {
    if (distance < mc.getOrcWoodcutterAttackRange() + self.getRadius() + 50/*��������� �����*/) {
      return 700 + lifePriority;
    }
    return 250 + lifePriority;
  } else {
    return lifePriority + ((400 * minion.getRemainingActionCooldownTicks()) / minion.getCooldownTicks());
  }
}

double MovePriorities::avoidWizard(const Wizard& self, const model::Wizard& wizard) {
  const double timeToTurnAttack = Algorithm::timeToTurnForAttack(self, wizard);
  const double timeForMagic = EX::minTimeForMagic(wizard);
  const double timeForUnforzend = EX::frozenTime(wizard);

  /// ���� ���� ����� ������ ������, �� ��� �� ����� �������
  if (wizard.getLife() + 2/*��� ��� ���� �����������*/ < EX::magicMissleAttack(self)) {
    return 0;
  }

  const int lifePriority = (600 * wizard.getLife()) / wizard.getMaxLife();

  int statusPriority = 0;
  for (const auto& status : wizard.getStatuses()) {
    if (model::STATUS_EMPOWERED == status.getType()) {
      statusPriority += 400;
    } else if (model::STATUS_FROZEN == status.getType()) {
      statusPriority -= status.getRemainingDurationTicks() * 15;
    }
  }

  return MAX(1000, 500 + statusPriority + lifePriority - timeForMagic * 10 - timeToTurnAttack * 25 - timeForUnforzend * 50);
}

double MovePriorities::attackFollow(const Wizard& self, const model::Wizard& wizard) {
  return 450;
}

/// ��� ����� �����, ������� �������� �������� �� �����
/// ������������� ����� ����� ������ ���, ��������� ���� � ����� projectile �� ���������, � �� ���������
double MovePriorities::avoidProjectile(const Wizard& self, const model::Projectile& projectile) {
  return 15000 - self.getDistanceTo(projectile);
}

double MovePriorities::defendPoint(const Wizard&, const Position&) {
  return 0;
}

double MovePriorities::follow(const Wizard&, const model::LivingUnit&) {
  return 300;
}

double MovePriorities::getExpirience(const Wizard& wizard, const model::LivingUnit&) {
  return 100 * wizard.getRole().getImportanceOfXP();
}

double MovePriorities::keepDistance(const Wizard&, const Position, const double, const double) {
  return 500;
}

double MovePriorities::moveToBonus(const Wizard& self, const Position&) {
  return (50 + 4 * self.getMaxLife()) * self.getRole().getImportanceOfBonus();
}

double MovePriorities::moveToLine(const Wizard& self, const model::LaneType&) {
  return 20 * self.getRole().getImportanceOfXP();
}

double MovePriorities::moveToPoint(const Wizard&, const Position&) {
  return 10;
}

double MovePriorities::observeMap(const Wizard&) {
  return 0;
}