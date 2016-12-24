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
  /// если угол больше 90 градусов - выбран не оптимальный из двух возможных перпендикуляров
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

  //// для сокращения расчетов
  if (delta.length() > 600 + 35 + 100 + 90) {//максимальный радиус атаки + радиус мага + максимальный радиус снаряда(fireball) + время на поворот
    return false;
  }

  changeOfWin = Algorithm::symmetrialChangeOfWinning(self, wizard, self.getRole().getFriendWizardConfidence(), 1);

  const auto maxSpeed = Algorithm::maxSpeed(self, self.getAngle(), delta);

  const int time = (int)ceil(Algorithm::timeToTurnForAttack(self, wizard));
  const int frozen = EX::frozenTime(wizard);
  const int maxSafeTime = MAX(time, frozen);

  /// вначале проверяем, что нужно или нет уклонятся от удара посоха
  if (EX::availableSkill(wizard, model::ACTION_STAFF)) {
    int cooldown = MAX(maxSafeTime, EX::cooldownMaxSkill(wizard, model::ACTION_STAFF));


    distance = Game::model().getStaffRange() + self.getRadius() - (double(cooldown) - 1) * maxSpeed.length();
    if (delta.length() < distance) {
      return true;
    }
  }

  /// если не нужно, то дальше уже считаем для снарядов

  std::vector<MagicInfo> magics;

  double useSpeedForMoveSelf = maxSpeed.length();
  /// всегда считаем что пуля будет запущена со следующей позиции врага
  const auto wizardSpeed = Algorithm::maxSpeed(wizard, wizard.getAngle(), delta);
  auto bBeginPos = wizardPos + delta.normal() * wizardSpeed.length();
  /// всегда считаем что снаряд уже вылетел, и находиться на один тик ближе к нам и с учетом что маг к нам подошел на тик
  if (EX::availableSkill(wizard, model::ACTION_MAGIC_MISSILE)) { ///magic missile
    useSpeedForMoveSelf *= 1;
    const auto bPos = bBeginPos + delta.normal() * Game::model().getMagicMissileSpeed();
    magics.push_back(MagicInfo{
      Bullet(0, delta.normal() * Game::model().getMagicMissileSpeed(), Game::model().getMagicMissileRadius(),
      bBeginPos, bPos, wizard.getCastRange(), model::PROJECTILE_MAGIC_MISSILE, wizard.getFaction()),
      EX::cooldownMaxSkill(wizard, model::ACTION_MAGIC_MISSILE),
      EX::cooldownByMana(wizard, model:: ACTION_MAGIC_MISSILE)
    });
  }

  if (EX::availableSkill(wizard, model::ACTION_FIREBALL)) {
    useSpeedForMoveSelf *= 1.5;
    const auto bPos = bBeginPos + delta.normal() * Game::model().getFireballSpeed();
    magics.push_back(MagicInfo{
      Bullet(0, delta.normal() * Game::model().getFireballSpeed(), Game::model().getFireballExplosionMinDamageRange(),
      bBeginPos, bPos, wizard.getCastRange(), model::PROJECTILE_FIREBALL, wizard.getFaction()),
      EX::cooldownMaxSkill(wizard, model::ACTION_FIREBALL),
      EX::cooldownByMana(wizard, model::ACTION_FIREBALL)
    });
  }

  if (EX::availableSkill(wizard, model::ACTION_FROST_BOLT)) {
    useSpeedForMoveSelf *= 0.5;
    const auto bPos = bBeginPos + delta.normal() * Game::model().getFrostBoltSpeed();
    magics.push_back(MagicInfo{
      Bullet(0, delta.normal() * Game::model().getFrostBoltSpeed(), Game::model().getFrostBoltRadius(),
      bBeginPos, bPos, wizard.getCastRange(), model::PROJECTILE_FROST_BOLT, wizard.getFaction()),
      EX::cooldownMaxSkill(wizard, model::ACTION_FROST_BOLT),
      EX::cooldownByMana(wizard, model::ACTION_FROST_BOLT)
    });
  }

  double audicity = (1 - changeOfWin) * (1 - changeOfWin) * 4 - 4; /// 12, 0, -4
  useSpeedForMoveSelf *= sqrt(1 + changeOfWin) / sqrt(2); // 0, 0.7, 1
  for (auto& magic : magics) {
    const int maxCooldown = MAX(maxSafeTime, MAX(magic.cooldown, magic.manaCooldown));

    double bTr = audicity * wizardSpeed.length();
    double sTr = (useSpeedForMoveSelf * double(maxCooldown)) - self.maxSpeed();

    magic.bullet.pos += delta.normal() * bTr;
    magic.bullet.startPoint += delta.normal() * bTr;


    if (!Algorithm::canDodge(self, selfPos, delta + delta.normal() * sTr, magic.bullet)) {
      distance = delta.length() + self.getRadius(); /// уходим назад
      return true;
    }
  }

  return false;
}

void CommandAvoidWizard::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = selfPos - wizardPos;


  /// если шансы на победу маленькие, то стоит повернуться спиной к магу, ну или точнее тупо бежать к точке, ибо мы находимся ближе
  const auto pos = wizardPos + delta.normal() * distance;
  result.set(pos, self);

  if (changeOfWin > 0.5) { ///если шансы на победу высоки, то можно идти по наглому в лоб
    result.turnDirection = -result.turnDirection;
  } else if (changeOfWin > 0.05) { /// если шансы на победу приблизительно равны, то пытаемся по возможности встать боком
    result.turnDirection = minimalPerpendicular(self, wizardPos);
  }

  result.turnPriority = TurnPriority::avoidWizard + (10 - wizard.getId());
  result.priority = MovePriorities::avoidWizard(self, wizard) * self.getRole().getAudacityWizard();
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidWizard::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
}
#endif // ENABLE_VISUALIZATOR