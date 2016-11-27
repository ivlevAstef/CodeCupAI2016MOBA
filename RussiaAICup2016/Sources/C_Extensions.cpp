#include "C_Extensions.h"
#include "E_Game.h"
#include "E_World.h"
#include "C_Math.h"
#include "model/ActionType.h"
#include "model/Projectile.h"
#include <unordered_set>

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

template<typename Type>
Type sumSkills(const model::Wizard& obj, const std::unordered_set<model::SkillType> skills, Type valuePerLevel) {
  Type result = 0;
  for (const auto& skill: obj.getSkills()) {
    if (0 != skills.count(skill)) {
      result += valuePerLevel;
    }
  }
  return result;
}


template<typename Type>
Type maxSkills(const model::Wizard& obj, const std::vector<model::SkillType> skills, Type valuePerLevel) {
  const auto wizards = World::instance().aroundAuraWizards(obj);

  Type mult = 0;
  for (const auto& wizard : wizards) {
    for (const auto& skill : wizard->getSkills()) {
      for (size_t i = 0; i < skills.size(); i++) {
        if (skills[i] == skill) {
          mult = MAX(mult, (Type)(i + 1));
        }
      }
    }
  }

  return valuePerLevel * mult;
}

/// Skills
double EX::speedFactor(const model::Wizard& obj) {
  #define auraSkills \
    model::SKILL_MOVEMENT_BONUS_FACTOR_AURA_1, \
    model::SKILL_MOVEMENT_BONUS_FACTOR_AURA_2
  #define passiVeSkills \
    model::SKILL_MOVEMENT_BONUS_FACTOR_PASSIVE_1, \
    model::SKILL_MOVEMENT_BONUS_FACTOR_PASSIVE_2

  const auto perLevel = Game::model().getMovementBonusFactorPerSkillLevel();
  const double factor1 = sumSkills<double>(obj, {auraSkills}, perLevel);
  const double factor2 = maxSkills<double>(obj, {auraSkills}, perLevel);
  double factor = MAX(factor1, factor2);
  factor += sumSkills<double>(obj, {passiVeSkills}, perLevel);

  for (const auto& status : obj.getStatuses()) {
    if (model::STATUS_HASTENED == status.getType()) {
      factor += Game::model().getHastenedMovementBonusFactor();
      break;
    }
  }

  return 1 + factor;

  #undef auraSkills
  #undef passiVeSkills
}


double EX::attackRadius(const model::Wizard& obj) {
  #define auraSkills \
    model::SKILL_RANGE_BONUS_AURA_1, \
    model::SKILL_RANGE_BONUS_AURA_2
  #define passiVeSkills \
    model::SKILL_RANGE_BONUS_PASSIVE_1, \
    model::SKILL_RANGE_BONUS_PASSIVE_2

  const auto perLevel = Game::model().getRangeBonusPerSkillLevel();
  const double radius1 = sumSkills<double>(obj, {auraSkills}, perLevel);
  const double radius2 = maxSkills<double>(obj, {auraSkills}, perLevel);
  double radius = MAX(radius1, radius2);
  radius += sumSkills<double>(obj, {passiVeSkills}, perLevel);

  return Game::model().getWizardCastRange() + radius;

  #undef auraSkills
  #undef passiVeSkills
}

double EX::magicMissleAttack(const model::Wizard& obj) {
  #define auraSkills \
    model::SKILL_MAGICAL_DAMAGE_BONUS_AURA_1, \
    model::SKILL_MAGICAL_DAMAGE_BONUS_AURA_2
  #define passiVeSkills \
    model::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_1, \
    model::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_2


  const auto perLevel = Game::model().getMagicalDamageBonusPerSkillLevel();
  const int damage1 = sumSkills<int>(obj, {auraSkills}, perLevel);
  const int damage2 = maxSkills<int>(obj, {auraSkills}, perLevel);
  double damage = (double)MAX(damage1, damage2);
  damage += (double)sumSkills<int>(obj, {passiVeSkills}, perLevel);
  damage += (double)Game::model().getMagicMissileDirectDamage();

  for (const auto& status : obj.getStatuses()) {
    if (model::STATUS_EMPOWERED == status.getType()) {
      damage *= Game::model().getEmpoweredDamageFactor();
    }
  }

  return damage;

  #undef auraSkills
  #undef passiVeSkills
}

double EX::staffAttack(const model::Wizard& obj) {
  #define auraSkills \
    model::SKILL_STAFF_DAMAGE_BONUS_AURA_1, \
    model::SKILL_STAFF_DAMAGE_BONUS_AURA_2
  #define passiVeSkills \
    model::SKILL_STAFF_DAMAGE_BONUS_PASSIVE_1, \
    model::SKILL_STAFF_DAMAGE_BONUS_PASSIVE_2


  const auto perLevel = Game::model().getStaffDamageBonusPerSkillLevel();
  const int damage1 = sumSkills<int>(obj, {auraSkills}, perLevel);
  const int damage2 = maxSkills<int>(obj, {auraSkills}, perLevel);
  double damage = (double)MAX(damage1, damage2);
  damage += (double)sumSkills<int>(obj, {passiVeSkills}, perLevel);
  damage += (double)Game::model().getStaffDamage();

  for (const auto& status : obj.getStatuses()) {
    if (model::STATUS_EMPOWERED == status.getType()) {
      damage *= Game::model().getEmpoweredDamageFactor();
    }
  }

  return damage;

  #undef auraSkills
  #undef passiVeSkills
}

double EX::armor(const model::Wizard& obj) {
  #define auraSkills \
    model::SKILL_MAGICAL_DAMAGE_ABSORPTION_AURA_1, \
    model::SKILL_MAGICAL_DAMAGE_ABSORPTION_AURA_2,
  #define passiVeSkills \
    model::SKILL_MAGICAL_DAMAGE_ABSORPTION_PASSIVE_1, \
    model::SKILL_MAGICAL_DAMAGE_ABSORPTION_PASSIVE_2


  const auto perLevel = Game::model().getMagicalDamageAbsorptionPerSkillLevel();
  const int armor1 = sumSkills<int>(obj, {auraSkills}, perLevel);
  const int armor2 = maxSkills<int>(obj, {auraSkills}, perLevel);
  int armor = MAX(armor1, armor2);
  armor += sumSkills<int>(obj, {passiVeSkills}, perLevel);

  return armor;

  #undef auraSkills
  #undef passiVeSkills
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

bool EX::availableSkill(const model::Wizard& obj, model::ActionType action) {
  return availableSkills(obj)[action];
}

int EX::cooldownSkill(const model::Wizard& obj, model::ActionType action) {
  return obj.getRemainingCooldownTicksByAction()[action];
}

/////////////////////// Support
double EX::radiusForGuaranteedHit(const model::Wizard& obj, const model::CircularUnit& enemy) {
  //TODO: надо учитывать врага
  if (isWizard(enemy)) {
    return 508; /// сложно попасть с дальше дистанции
  }
  if (isMinion(enemy)) {
    return attackRadius(obj);
  }
  return attackRadius(obj) + enemy.getRadius();
}

double EX::radiusForGuaranteedHitFrostBolt(const model::Wizard& obj, const model::CircularUnit& enemy) {
  //TODO: надо учитывать врага
  if (isWizard(enemy)) {
    return 440; /// сложно попасть с дальше дистанции
  }
  return 0;
}

double EX::radiusForGuaranteedHitFireBall(const model::Wizard& obj, const model::CircularUnit& enemy) {
  //TODO: надо учитывать врага
  if (isWizard(enemy)) {
    return 420; /// сложно попасть с дальше дистанции
  }
  if (isMinion(enemy)) {
    return attackRadius(obj);
  }
  return 0;
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