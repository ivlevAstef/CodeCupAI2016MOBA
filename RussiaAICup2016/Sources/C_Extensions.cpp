#include "C_Extensions.h"
#include "E_Game.h"
#include "C_Math.h"
#include "model/ActionType.h"
#include "model/Projectile.h"

using namespace AICup;

double speedFactor(const model::LivingUnit& obj) {
  const auto wizard = dynamic_cast<const model::Wizard*>(&obj);

  double factor = 1;

  for (const auto& status : obj.getStatuses()) {
    if (model::STATUS_HASTENED == status.getType()) {
      factor *= (1.0 + Game::instance().model().getHastenedMovementBonusFactor());
    }
  }

  if (nullptr != wizard) {
    for (const auto& skill : wizard->getSkills()) {
      if (model::SKILL_MOVEMENT_BONUS_FACTOR_PASSIVE_1 == skill) {
        factor *= (1.0 + Game::instance().model().getMovementBonusFactorPerSkillLevel());
      } else if (model::SKILL_MOVEMENT_BONUS_FACTOR_AURA_1 == skill) {
        factor *= (1.0 + 2 * Game::instance().model().getMovementBonusFactorPerSkillLevel());
      } else if (model::SKILL_MOVEMENT_BONUS_FACTOR_PASSIVE_2 == skill) {
        factor *= (1.0 + 3 * Game::instance().model().getMovementBonusFactorPerSkillLevel());
      } else if (model::SKILL_MOVEMENT_BONUS_FACTOR_AURA_2 == skill) {
        factor *= (1.0 + 4 * Game::instance().model().getMovementBonusFactorPerSkillLevel());
      }
    }
  }

  return factor;
}

bool EX::isTree(const model::Unit& unit) {
  return nullptr != dynamic_cast<const model::Tree*>(&unit);
}
bool EX::isNeutral(const model::Unit& unit) {
  return isMinion(unit) && unit.getFaction() == model::FACTION_NEUTRAL;
}
bool EX::isMinion(const model::Unit& unit) {
  return nullptr != dynamic_cast<const model::Minion*>(&unit);
}
bool EX::isBuilding(const model::Unit& unit) {
  return nullptr != dynamic_cast<const model::Building*>(&unit);
}
bool EX::isWizard(const model::Unit& unit) {
  return nullptr != dynamic_cast<const model::Wizard*>(&unit);
}

bool EX::isProjectile(const model::Unit& unit) {
  return nullptr != dynamic_cast<const model::Projectile*>(&unit);
}


const model::Tree& EX::asTree(const model::Unit& unit) {
  return *dynamic_cast<const model::Tree*>(&unit);
}
const model::Minion& EX::asMinion(const model::Unit& unit) {
  return *dynamic_cast<const model::Minion*>(&unit);
}
const model::Building& EX::asBuilding(const model::Unit& unit) {
  return *dynamic_cast<const model::Building*>(&unit);
}
const model::Wizard& EX::asWizard(const model::Unit& unit) {
  return *dynamic_cast<const model::Wizard*>(&unit);
}

double EX::maxSpeed(const model::CircularUnit& obj) {
  const auto livingObj = dynamic_cast<const model::LivingUnit*>(&obj);
  if (nullptr == livingObj) {
    return 0;
  }

  const auto wizard = dynamic_cast<const model::Wizard*>(&obj);
  const auto minion = dynamic_cast<const model::Minion*>(&obj);

  double maxSpeed = 0;
  if (nullptr != wizard) {
    maxSpeed = Game::instance().model().getWizardForwardSpeed();
  } else if (nullptr != minion) {
    maxSpeed = Game::instance().model().getMinionSpeed();
  }

  return maxSpeed * speedFactor(*livingObj);
}

double EX::maxStrafeSpeed(const model::Wizard& obj) {
  double maxSpeed = Game::instance().model().getWizardStrafeSpeed();
  return maxSpeed *= speedFactor(obj);
}

double EX::maxBackwardSpeed(const model::Wizard& obj) {
  double maxSpeed = Game::instance().model().getWizardBackwardSpeed();
  return maxSpeed *= speedFactor(obj);
}

double EX::radiusForGuaranteedHit(const model::Wizard& obj) {
  static const double magicCalcConstant = 8;

  double radius = obj.getCastRange() + magicCalcConstant;


  for (const auto& skill : obj.getSkills()) {
    if (model::SKILL_RANGE_BONUS_PASSIVE_1 == skill) {
      radius += Game::instance().model().getRangeBonusPerSkillLevel();
    } else if (model::SKILL_RANGE_BONUS_AURA_1 == skill) {
      radius += 3 * Game::instance().model().getRangeBonusPerSkillLevel();
    } else if (model::SKILL_RANGE_BONUS_PASSIVE_2 == skill) {
      radius += 2 * Game::instance().model().getRangeBonusPerSkillLevel();
    } else if (model::SKILL_RANGE_BONUS_AURA_2 == skill) {
      radius += 4 * Game::instance().model().getRangeBonusPerSkillLevel();
    }
  }

  return radius;
}

std::vector<bool> EX::availableSkills(const model::Wizard& obj) {
  std::vector<bool> available;
  available.resize(7);
  available[model::ACTION_NONE] = true;
  available[model::ACTION_STAFF] = true;
  available[model::ACTION_MAGIC_MISSILE] = true;

  for (const auto& skill : obj.getSkills()) {
    if (model::SKILL_FIREBALL == skill) {
      available[model::ACTION_FIREBALL] = true;
    } else if (model::SKILL_FROST_BOLT == skill) {
      available[model::ACTION_FROST_BOLT] = true;
    } else if (model::SKILL_HASTE == skill) {
      available[model::ACTION_HASTE] = true;
    } else if (model::SKILL_SHIELD == skill) {
      available[model::ACTION_SHIELD] = true;
    }
  }

  return available;
}

int EX::minTimeForMagic(const model::Wizard& obj) {
  const auto& actionCooldowns = obj.getRemainingCooldownTicksByAction();
  const auto available = availableSkills(obj);

  int minCooldown = 999999;
  for (size_t action = 0; action < actionCooldowns.size(); action++) {
    if ((action == model::ACTION_MAGIC_MISSILE
      || action == model::ACTION_FROST_BOLT
      || action == model::ACTION_FIREBALL) && available[action]) {
      minCooldown = MIN(minCooldown, actionCooldowns[action]);
    }
  }

  return MAX(minCooldown, obj.getRemainingActionCooldownTicks());

}

double EX::turnSpeed(const model::Wizard& obj) {
  double maxTurnSpeed = Game::instance().model().getWizardMaxTurnAngle();


  for (const auto& status : obj.getStatuses()) {
    if (model::STATUS_HASTENED == status.getType()) {
      maxTurnSpeed *= (1.0 + Game::instance().model().getHastenedRotationBonusFactor());
    }
  }

  return maxTurnSpeed;
}

double EX::timeToTurnForAttack(const model::Unit& attacked, const model::Wizard& attacking) {
  const auto selfPos = Position(attacking.getX(), attacking.getY());
  const auto enemyPos = Position(attacked.getX(), attacked.getY());

  const auto dir = enemyPos - selfPos;
  double angleDeviation = Math::angleDiff(dir.angle(), attacking.getAngle());
  angleDeviation = ABS(angleDeviation);

  const double needTurnAngle = MAX(0, angleDeviation - Game::instance().model().getStaffSector()* 0.5);
  /// если времени до атаки больше чем времени до разворота, то можно пока не атаковать

  return needTurnAngle / turnSpeed(attacking);
}

double EX::magicMissleAttack(const model::Wizard& obj) {
  double power = Game::instance().model().getMagicMissileDirectDamage();


  for (const auto& status : obj.getStatuses()) {
    if (model::STATUS_EMPOWERED == status.getType()) {
      power *= Game::instance().model().getEmpoweredDamageFactor();
    }
  }


  for (const auto& skill : obj.getSkills()) {
    if (model::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_1 == skill) {
      power += Game::instance().model().getMagicalDamageBonusPerSkillLevel();
    } else if (model::SKILL_MAGICAL_DAMAGE_BONUS_AURA_1 == skill) {
      power += 2 * Game::instance().model().getMagicalDamageBonusPerSkillLevel();
    } else if (model::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_2 == skill) {
      power += 3 * Game::instance().model().getMagicalDamageBonusPerSkillLevel();
    } else if (model::SKILL_MAGICAL_DAMAGE_BONUS_AURA_2 == skill) {
      power += 4 * Game::instance().model().getMagicalDamageBonusPerSkillLevel();
    }
  }

  return power;
}