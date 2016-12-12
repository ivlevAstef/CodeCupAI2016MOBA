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
  double dodgeRange;
  double distanceCooldown;
  double manaDistanceCooldown;
  double projectileRadius;
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
  if (delta.length() > 600 + 35 + 100 + 60) {//максимальный радиус атаки + радиус мага + максимальный радиус снаряда(fireball) + время на поворот
    return false;
  }

  const auto center = wizardPos + delta * 0.8;
  changeOfWin = Algorithm::changeOfWinning(self, center.x, center.y);

  const double useSpeed = self.maxBackwardSpeed();

  std::vector<MagicInfo> magics;

  if (EX::availableSkill(wizard, model::ACTION_MAGIC_MISSILE)) { ///magic missile
    magics.push_back(MagicInfo{
      EX::radiusForGuaranteedDodge(self, 0),
      EX::cooldownMaxSkill(wizard, model::ACTION_MAGIC_MISSILE) * useSpeed,
      EX::cooldownByMana(wizard, model:: ACTION_MAGIC_MISSILE) * useSpeed,
      Game::model().getMagicMissileRadius()
    });
  }

  if (EX::availableSkill(wizard, model::ACTION_FIREBALL)) {
    magics.push_back(MagicInfo{
      EX::radiusForGuaranteedDodgeFireBall(self, 0),
      EX::cooldownMaxSkill(wizard, model::ACTION_FIREBALL) * useSpeed,
      EX::cooldownByMana(wizard, model::ACTION_FIREBALL) * useSpeed,
      Game::model().getFireballExplosionMinDamageRange()
    });
  }

  if (EX::availableSkill(wizard, model::ACTION_FROST_BOLT)) {
    magics.push_back(MagicInfo{
      EX::radiusForGuaranteedDodgeFrostBolt(self, 0) + 2 * self.maxSpeed()/*лучше перестраховаться*/,
      EX::cooldownMaxSkill(wizard, model::ACTION_FROST_BOLT) * useSpeed,
      EX::cooldownByMana(wizard, model::ACTION_FROST_BOLT) * useSpeed,
      Game::model().getFrostBoltRadius() + self.maxSpeed()/*лучше перестраховаться*/
    });
  }

  if (EX::availableSkill(wizard, model::ACTION_STAFF)) {
    magics.push_back(MagicInfo{
      Game::model().getStaffRange() + self.getRadius(),
      EX::cooldownMaxSkill(wizard, model::ACTION_STAFF) * useSpeed,
      0,
      0
    });
  }

  const double distanceTime = Algorithm::timeToTurnForAttack(self, wizard) * useSpeed;
  const double distanceFrozen = EX::frozenTime(wizard) * useSpeed;
  const double maxFromDistanceTimeFrozen = MAX(distanceTime, distanceFrozen);

  const double maxCastRange = wizard.getCastRange() + self.getRadius();


  double finalDodgeRange = 0;
  double finalCastRange = 0;
  for (const auto& magic : magics) {
    const double maxCooldownManaDistance = MAX(magic.distanceCooldown, magic.manaDistanceCooldown);
    const double stockDistance = MAX(maxCooldownManaDistance, maxFromDistanceTimeFrozen);
    const double dodgeRange = magic.dodgeRange - stockDistance;
    const double castRange = maxCastRange - stockDistance + magic.projectileRadius;

    finalDodgeRange = MAX(finalDodgeRange, dodgeRange);
    finalCastRange = MAX(finalCastRange, castRange);
  }

  distance = MIN(finalDodgeRange, finalCastRange);
  distance += self.maxSpeed();

  if (delta.length() > distance) {
    return false;
  }

  /// если на пути дерево, то тоже бояться не стоит
  if (Algorithm::checkIntersectedTree(selfPos, wizardPos, Game::model().getMagicMissileRadius())) {
    return false;
  }

  return true;
}

void CommandAvoidWizard::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = selfPos - wizardPos;


  const auto pos = wizardPos + delta.normal() * distance;
  result.set(pos, self);

  /// если шансы на победу маленькие, то стоит повернуться спиной к магу, ну или точнее тупо бежать к точке, ибо мы находимся ближе

  if (changeOfWin > 0.8) { ///если шансы на победу высоки, то можно идти по наглому в лоб
    result.turnDirection = -result.turnDirection;
  } else if (changeOfWin > -0.2) {
    result.turnDirection = minimalPerpendicular(self, wizardPos);
  }

  double changeOfWinPriority = (changeOfWin > 0) ? (1 - changeOfWin * 0.75) : (1 - changeOfWin);

  result.turnPriority = TurnPriority::avoidWizard + (10 - wizard.getId());
  result.priority = MovePriorities::avoidWizard(self, wizard) * changeOfWinPriority * self.getRole().getAudacityWizard();
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidWizard::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
}
#endif // ENABLE_VISUALIZATOR