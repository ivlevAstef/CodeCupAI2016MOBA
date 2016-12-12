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

double EX::damage(const model::Wizard& obj, const model::ActionType action) {
  if (model::ACTION_STAFF == action) {
    return staffAttack(obj);
  } else if (model::ACTION_MAGIC_MISSILE == action) {
    return magicMissleAttack(obj);
  } else if (model::ACTION_FROST_BOLT == action) {
    return Game::model().getFrostBoltDirectDamage();
  } else if (model::ACTION_FIREBALL == action) {
    return Game::model().getFireballExplosionMaxDamage();
  }
  return 0;
}


double EX::dps(const model::Wizard& obj) {
  double mmMana = MIN(1.0, double(obj.getMana()) / double(Game::model().getMagicMissileManacost()));
  double mmDps = dps(obj, model::ACTION_MAGIC_MISSILE) * mmMana;

  double result = dps(obj, model::ACTION_STAFF);
  result += mmDps;

  if (availableSkill(obj, model::ACTION_FROST_BOLT)) {
    double mana = MIN(1.0, double(obj.getMana()) / double(Game::model().getFrostBoltManacost()));
    double ffDps = dps(obj, model::ACTION_FROST_BOLT);
    result += ffDps*mana + mmDps;
  }

  if (availableSkill(obj, model::ACTION_FIREBALL)) {
    double mana = MIN(1.0, double(obj.getMana()) / double(Game::model().getFireballManacost()));
    double ffDps = dps(obj, model::ACTION_FIREBALL);
    result += MAX(ffDps, mmDps) - mmDps;
  }

  return result;
}

double EX::dps(const model::Wizard& obj, const model::ActionType action) {
  if (model::ACTION_STAFF == action) {
    return staffAttack(obj) / Game::model().getStaffCooldownTicks();
  } else if (model::ACTION_MAGIC_MISSILE == action) {
    for (const auto& skill : obj.getSkills()) {
      if (skill == model::SKILL_ADVANCED_MAGIC_MISSILE) {
        return magicMissleAttack(obj) / Game::model().getWizardActionCooldownTicks();
      }
    }
    return magicMissleAttack(obj) / Game::model().getMagicMissileCooldownTicks();
  } else if (model::ACTION_FROST_BOLT == action) {
    return Game::model().getFrostBoltDirectDamage() / Game::model().getFrostBoltCooldownTicks();
  } else if (model::ACTION_FIREBALL == action) {
    return (Game::model().getFireballExplosionMaxDamage() + Game::model().getBurningSummaryDamage()) / Game::model().getFireballCooldownTicks();
  }
  return 0;
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

int EX::cooldownMaxSkill(const model::Wizard& obj, model::ActionType action) {
  const auto cd1 = obj.getRemainingCooldownTicksByAction()[action];
  const auto cd2 = obj.getRemainingActionCooldownTicks();
  return MAX(cd1, cd2);
}

int EX::cooldownByMana(const model::Wizard& obj, model::ActionType action) {
  double manaRegen = Game::model().getWizardBaseManaRegeneration() + Game::model().getWizardManaRegenerationGrowthPerLevel() * obj.getLevel();
  double manaCost = 0;
  if (model::ACTION_MAGIC_MISSILE == action) {
    manaCost = Game::model().getMagicMissileManacost();
  } else if (model::ACTION_FROST_BOLT == action) {
    manaCost = Game::model().getFrostBoltManacost();
  } else if (model::ACTION_FIREBALL == action) {
    manaCost = Game::model().getFireballManacost();
  } else if (model::ACTION_HASTE == action) {
    manaCost = Game::model().getHasteManacost();
  } else if (model::ACTION_SHIELD == action) {
    manaCost = Game::model().getShieldManacost();
  }

  if (manaCost <= obj.getMana()) {
    return 0;
  }

  return (manaCost - obj.getMana()) / manaRegen;
}

double EX::burnResidualDamage(const model::Wizard& obj) {
  double result = 0;

  for (const auto& status : obj.getStatuses()) {
    if (status.getType() == model::STATUS_BURNING) {
      result += (Game::model().getBurningSummaryDamage() * status.getRemainingDurationTicks()) / Game::model().getBurningDurationTicks();
    }
  }

  return result;
}

bool EX::isShield(const model::Wizard& obj) {
  for (const auto& status : obj.getStatuses()) {
    if (status.getType() == model::STATUS_SHIELDED) {
      return true;
    }
  }
  return false;
}

bool EX::isEmpower(const model::Wizard& obj) {
  for (const auto& status : obj.getStatuses()) {
    if (status.getType() == model::STATUS_EMPOWERED) {
      return true;
    }
  }
  return false;
}


double EX::danger(const model::Wizard& obj) {
  double lifeDanger = double(obj.getLife() - burnResidualDamage(obj)) / 10.0; /// 10 вначале - 35 вконце
  double mmDanger = 20 * dps(obj, model::ACTION_MAGIC_MISSILE); /// 4 вначале - 8 с вкаченной скорость, 5,3 c атакой+ - max = 10,66
  double staffDanger = 15 * dps(obj, model::ACTION_STAFF); // 3 вначале - 6 вконце
  double speedDanger = (maxSpeed(obj) * maxSpeed(obj)) / 5.333; /// 3 вначале - 6.75 на максиуме
  double armorDanger = armor(obj) * 1.5; /// 0 вначале - 6 вконце
  double shieldDanger = isShield(obj) ? 6 : 0; /// 0-6
  double empowerdDanger = isEmpower(obj) ? 6 : 0; ///0-6
  double frostDanger = availableSkill(obj, model::ACTION_FROST_BOLT) ? 15 : 0; ///0-15
  double frostCd = double(cooldownMaxSkill(obj, model::ACTION_FROST_BOLT)) / double(Game::model().getFrostBoltCooldownTicks());
  double fireDanger = availableSkill(obj, model::ACTION_FIREBALL) ? 8 : 0; ///0-8
  double fireCd = double(cooldownMaxSkill(obj, model::ACTION_FIREBALL)) / double(Game::model().getFireballCooldownTicks());

  frostDanger *= MAX(0.5, 1.0 - frostCd);
  fireDanger *= (1.0 - fireCd);

  mmDanger *= MIN(1.0, double(obj.getMana()) / double(2.0 * Game::model().getMagicMissileManacost()));
  frostDanger *= MIN(1.0, double(obj.getMana()) / double(Game::model().getFrostBoltManacost()));
  fireDanger *= MIN(1.0, double(obj.getMana()) / double(Game::model().getFireballManacost()));

  double frozenAntiDanger = -0.4 * EX::frozenTime(obj); ///0-24

  /// min 20
  /// max 100
  ///      10-35      4-10,66       3-6          3-6.75          0-6           0-6             0-6            0-15          0-8          -24-0
  return lifeDanger + mmDanger + staffDanger + speedDanger + armorDanger + shieldDanger + empowerdDanger + frostDanger + fireDanger + frozenAntiDanger;
}


/////////////////////// Support
double EX::radiusForGuaranteedDodge(const model::Wizard& self, double coef) {
  const auto radius = self.getRadius() + Game::model().getMagicMissileRadius();
  ///такая скорость, ибо наврятли условия будут идеальные
  const auto speed = maxSpeed(self) * coef +  maxStrafeSpeed(self) * (1 - coef);

  return Game::model().getMagicMissileSpeed() * (1.0 + (radius / speed));
}

double EX::radiusForGuaranteedDodgeFrostBolt(const model::Wizard& self, double coef) {
  const auto radius = self.getRadius() + Game::model().getFrostBoltRadius();
  ///такая скорость, ибо наврятли условия будут идеальные
  const auto speed = maxSpeed(self) * coef + maxStrafeSpeed(self) * (1 - coef);

  return Game::model().getFrostBoltSpeed() * (1.0 + (radius / speed));
}

double EX::radiusForGuaranteedDodgeFireBall(const model::Wizard& self, double coef) {
  const auto radius = self.getRadius() + Game::model().getFireballExplosionMinDamageRange();
  ///такая скорость, ибо наврятли условия будут идеальные
  const auto speed = maxSpeed(self) * coef + maxStrafeSpeed(self) * (1 - coef);

  return Game::model().getFireballSpeed() * (1.0 + (radius / speed));
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

int EX::frozenTime(const model::Wizard& obj) {
  for (const auto& status : obj.getStatuses()) {
    if (model::STATUS_FROZEN == status.getType()) {
      return status.getRemainingDurationTicks();
    }
  }
  return 0;
}