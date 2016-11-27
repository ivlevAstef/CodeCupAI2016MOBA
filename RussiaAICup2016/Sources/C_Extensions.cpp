#include "C_Extensions.h"
#include "E_Game.h"
#include "C_Math.h"
#include "model/ActionType.h"
#include "model/Projectile.h"

using namespace AICup;

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
  const auto wizard = dynamic_cast<const model::Wizard*>(&obj);
  const auto minion = dynamic_cast<const model::Minion*>(&obj);

  if (nullptr != wizard) {
    return maxSpeed(*wizard);
  } else if (nullptr != minion) {
    return Game::model().getMinionSpeed();
  }

  return 0;
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

////////////////// Wizard

double EX::speedFactor(const model::Wizard& obj) {
  double factor = 1;

  for (const auto& skill : obj.getSkills()) {
    if (model::SKILL_MOVEMENT_BONUS_FACTOR_PASSIVE_1 == skill
      || model::SKILL_MOVEMENT_BONUS_FACTOR_AURA_1 == skill
      || model::SKILL_MOVEMENT_BONUS_FACTOR_PASSIVE_2 == skill
      || model::SKILL_MOVEMENT_BONUS_FACTOR_AURA_2 == skill) {
      factor += Game::model().getMovementBonusFactorPerSkillLevel();
    }
  }

  for (const auto& status : obj.getStatuses()) {
    if (model::STATUS_HASTENED == status.getType()) {
      factor += Game::model().getHastenedMovementBonusFactor();
    }
  }

  return factor;
}

double EX::maxSpeed(const model::Wizard& obj) {
  return Game::model().getWizardForwardSpeed() * speedFactor(obj);
}
double EX::maxStrafeSpeed(const model::Wizard& obj) {
  return Game::model().getWizardStrafeSpeed() * speedFactor(obj);
}
double EX::maxBackwardSpeed(const model::Wizard& obj) {
  return Game::model().getWizardBackwardSpeed() * speedFactor(obj);
}

double EX::turnSpeed(const model::Wizard& obj) {
  double maxTurnSpeed = Game::model().getWizardMaxTurnAngle();

  for (const auto& status : obj.getStatuses()) {
    if (model::STATUS_HASTENED == status.getType()) {
      maxTurnSpeed *= (1.0 + Game::model().getHastenedRotationBonusFactor());
    }
  }

  return maxTurnSpeed;
}

double EX::attackRadius(const model::Wizard& obj) {
  double radius = obj.getCastRange();

  for (const auto& skill : obj.getSkills()) {
    if (model::SKILL_RANGE_BONUS_PASSIVE_1 == skill
      || model::SKILL_RANGE_BONUS_AURA_1 == skill
      || model::SKILL_RANGE_BONUS_PASSIVE_2 == skill
      || model::SKILL_RANGE_BONUS_AURA_2 == skill) {
      radius += Game::model().getRangeBonusPerSkillLevel();
    }
  }

  return radius;
}

double EX::magicMissleAttack(const model::Wizard& obj) {
  double power = Game::model().getMagicMissileDirectDamage();

  for (const auto& skill : obj.getSkills()) {
    if (model::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_1 == skill
      || model::SKILL_MAGICAL_DAMAGE_BONUS_AURA_1 == skill
      || model::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_2 == skill
      || model::SKILL_MAGICAL_DAMAGE_BONUS_AURA_2 == skill) {
      power += Game::model().getMagicalDamageBonusPerSkillLevel();
    }
  }

  for (const auto& status : obj.getStatuses()) {
    if (model::STATUS_EMPOWERED == status.getType()) {
      power *= Game::model().getEmpoweredDamageFactor();
    }
  }

  return power;
}
double EX::staffAttack(const model::Wizard& obj) {
  double damage = Game::model().getStaffDamage();

  for (const auto& skill : obj.getSkills()) {
    if (model::SKILL_STAFF_DAMAGE_BONUS_PASSIVE_1 == skill
      || model::SKILL_STAFF_DAMAGE_BONUS_AURA_1 == skill
      || model::SKILL_STAFF_DAMAGE_BONUS_PASSIVE_2 == skill
      || model::SKILL_STAFF_DAMAGE_BONUS_AURA_2 == skill) {
      damage += Game::model().getStaffDamageBonusPerSkillLevel();
    }
  }

  return damage;
}

double EX::armor(const model::Wizard& obj) {
  double armor = 0;

  for (const auto& skill : obj.getSkills()) {
    if (model::SKILL_MAGICAL_DAMAGE_ABSORPTION_PASSIVE_1 == skill
      || model::SKILL_MAGICAL_DAMAGE_ABSORPTION_AURA_1 == skill
      || model::SKILL_MAGICAL_DAMAGE_ABSORPTION_PASSIVE_2 == skill
      || model::SKILL_MAGICAL_DAMAGE_ABSORPTION_AURA_2 == skill) {
      armor += Game::model().getStaffDamageBonusPerSkillLevel();
    }
  }

  return armor;
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


/////////////////////// Support
double EX::radiusForGuaranteedHit(const model::Wizard& obj) {
  static const double magicCalcConstant = 8;
  return attackRadius(obj) + magicCalcConstant;
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