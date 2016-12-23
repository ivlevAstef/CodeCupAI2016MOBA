#include "CM_CommandAvoidWizard.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"
#include "A_Move.h"
#include "C_Math.h"
#include "A_WinPredictor.h"

using namespace AICup;

CommandAvoidWizard::CommandAvoidWizard(const model::Wizard& wizard): wizard(wizard) {
  changeOfWin = 0;
  distance = 0;
}

struct MagicInfo {
  Bullet bullet;
  int cooldown;
  int manaCooldown;
};

static Vector minimalPerpendicular(const Wizard& self, const Position& wizardPos) {
  const auto selfPos = EX::pos(self);
  const auto delta = selfPos - wizardPos;

  const auto turnDirection = delta.perpendicular();
  /// ���� ���� ������ 90 �������� - ������ �� ����������� �� ���� ��������� ���������������
  if (turnDirection.dot(Vector(1, 0).rotate(self.getAngle())) < 0) {
    return -turnDirection;
  }
  return turnDirection;
}

bool CommandAvoidWizard::check(const Wizard& self) {
  const auto mc = Game::model();

  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = selfPos - wizardPos;

  //// ��� ���������� ��������
  if (delta.length() > 600 + 35 + 100 + 90) {//������������ ������ ����� + ������ ���� + ������������ ������ �������(fireball) + ����� �� �������
    return false;
  }

  changeOfWin = Algorithm::changeOfWinning(self, wizardPos.x, wizardPos.y);

  const auto maxSpeed = Algorithm::maxSpeed(self, self.getAngle(), delta);

  const int time = (int)ceil(Algorithm::timeToTurnForAttack(self, wizard));
  const int frozen = EX::frozenTime(wizard);
  const int maxSafeTime = MAX(time, frozen);

  /// ������� ���������, ��� ����� ��� ��� ��������� �� ����� ������
  if (EX::availableSkill(wizard, model::ACTION_STAFF)) {
    int cooldown = MAX(maxSafeTime, EX::cooldownMaxSkill(wizard, model::ACTION_STAFF));


    distance = Game::model().getStaffRange() + self.getRadius() - (double(cooldown) - 1) * maxSpeed.length();
    if (delta.length() < distance) {
      return true;
    }
  }

  /// ���� �� �����, �� ������ ��� ������� ��� ��������

  std::vector<MagicInfo> magics;

  double useSpeedForMoveSelf = maxSpeed.length();
  /// ������ ������� ��� ���� ����� �������� �� ��������� ������� �����
  const auto wizardSpeed = Algorithm::maxSpeed(wizard, wizard.getAngle(), delta);
  auto bBeginPos = wizardPos + delta.normal() * wizardSpeed.length();
  /// ������ ������� ��� ������ ��� �������, � ���������� �� ���� ��� ����� � ��� � � ������ ��� ��� � ��� ������� �� ���
  if (EX::availableSkill(wizard, model::ACTION_MAGIC_MISSILE)) { ///magic missile
    useSpeedForMoveSelf *= 0.5;
    const auto bPos = bBeginPos + delta.normal() * Game::model().getMagicMissileSpeed();
    magics.push_back(MagicInfo{
      Bullet(0, delta.normal() * Game::model().getMagicMissileSpeed(), Game::model().getMagicMissileRadius(),
      bBeginPos, bPos, wizard.getCastRange(), model::PROJECTILE_MAGIC_MISSILE, wizard.getFaction()),
      EX::cooldownMaxSkill(wizard, model::ACTION_MAGIC_MISSILE),
      EX::cooldownByMana(wizard, model:: ACTION_MAGIC_MISSILE)
    });
  }

  if (EX::availableSkill(wizard, model::ACTION_FIREBALL)) {
    useSpeedForMoveSelf *= 0.5;
    const auto bPos = bBeginPos + delta.normal() * Game::model().getFireballSpeed();
    magics.push_back(MagicInfo{
      Bullet(0, delta.normal() * Game::model().getFireballSpeed(), Game::model().getFireballExplosionMinDamageRange(),
      bBeginPos, bPos, wizard.getCastRange(), model::PROJECTILE_FIREBALL, wizard.getFaction()),
      EX::cooldownMaxSkill(wizard, model::ACTION_FIREBALL),
      EX::cooldownByMana(wizard, model::ACTION_FIREBALL)
    });
  }

  if (EX::availableSkill(wizard, model::ACTION_FROST_BOLT)) {
    useSpeedForMoveSelf *= 0.15;
    const auto bPos = bBeginPos + delta.normal() * Game::model().getFrostBoltSpeed();
    magics.push_back(MagicInfo{
      Bullet(0, delta.normal() * Game::model().getFrostBoltSpeed(), Game::model().getFrostBoltRadius(),
      bBeginPos, bPos, wizard.getCastRange(), model::PROJECTILE_FROST_BOLT, wizard.getFaction()),
      EX::cooldownMaxSkill(wizard, model::ACTION_FROST_BOLT),
      EX::cooldownByMana(wizard, model::ACTION_FROST_BOLT)
    });
  }

  for (auto& magic : magics) {
    const int maxCooldown = MAX(maxSafeTime, MAX(magic.cooldown, magic.manaCooldown));

    double tr = (useSpeedForMoveSelf * double(maxCooldown)) - self.maxSpeed() - 12 * wizardSpeed.length();

    magic.bullet.pos -= delta.normal() * tr;
    magic.bullet.startPoint -= delta.normal() * tr;

    if (!Algorithm::canDodge(self, selfPos, delta, magic.bullet)) {
      distance = delta.length() + self.getRadius(); /// ������ �����
      return true;
    }
  }

  return false;
}

void CommandAvoidWizard::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = selfPos - wizardPos;


  /// ���� ����� �� ������ ���������, �� ����� ����������� ������ � ����, �� ��� ������ ���� ������ � �����, ��� �� ��������� �����
  const auto pos = wizardPos + delta.normal() * distance;
  result.set(pos, self);

  if (changeOfWin > 0.5) { ///���� ����� �� ������ ������, �� ����� ���� �� ������� � ���
    result.turnDirection = -result.turnDirection;
  } else if (changeOfWin > 0.05) { /// ���� ����� �� ������ �������������� �����, �� �������� �� ����������� ������ �����
    result.turnDirection = minimalPerpendicular(self, wizardPos);
  }

  result.turnPriority = TurnPriority::avoidWizard + (10 - wizard.getId());
  result.priority = MovePriorities::avoidWizard(self, wizard) * self.getRole().getAudacityWizard();
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidWizard::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
}
#endif // ENABLE_VISUALIZATOR