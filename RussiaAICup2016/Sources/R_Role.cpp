#include "R_Role.h"
#include "C_Extensions.h"

using namespace AICup;

const std::vector<model::SkillType> SkillBranches::rangeMagicMissile = {
  model::SKILL_RANGE_BONUS_PASSIVE_1,
  model::SKILL_RANGE_BONUS_AURA_1,
  model::SKILL_RANGE_BONUS_PASSIVE_2,
  model::SKILL_RANGE_BONUS_AURA_2,
  model::SKILL_ADVANCED_MAGIC_MISSILE,
};

const std::vector<model::SkillType> SkillBranches::magicalDamageFrostBolt = {
  model::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_1,
  model::SKILL_MAGICAL_DAMAGE_BONUS_AURA_1,
  model::SKILL_MAGICAL_DAMAGE_BONUS_PASSIVE_2,
  model::SKILL_MAGICAL_DAMAGE_BONUS_AURA_2,
  model::SKILL_FROST_BOLT,
};

const std::vector<model::SkillType> SkillBranches::meleeDamageFireBolt = {
  model::SKILL_STAFF_DAMAGE_BONUS_PASSIVE_1,
  model::SKILL_STAFF_DAMAGE_BONUS_AURA_1,
  model::SKILL_STAFF_DAMAGE_BONUS_PASSIVE_2,
  model::SKILL_STAFF_DAMAGE_BONUS_AURA_2,
  model::SKILL_FIREBALL,
};

const std::vector<model::SkillType> SkillBranches::moveHast = {
  model::SKILL_MOVEMENT_BONUS_FACTOR_PASSIVE_1,
  model::SKILL_MOVEMENT_BONUS_FACTOR_AURA_1,
  model::SKILL_MOVEMENT_BONUS_FACTOR_PASSIVE_2,
  model::SKILL_MOVEMENT_BONUS_FACTOR_AURA_2,
  model::SKILL_HASTE,
};

const std::vector<model::SkillType> SkillBranches::armorShield = {
  model::SKILL_MAGICAL_DAMAGE_ABSORPTION_PASSIVE_1,
  model::SKILL_MAGICAL_DAMAGE_ABSORPTION_AURA_1,
  model::SKILL_MAGICAL_DAMAGE_ABSORPTION_PASSIVE_2,
  model::SKILL_MAGICAL_DAMAGE_ABSORPTION_AURA_2,
  model::SKILL_SHIELD
};

Role::Role(const float audacity,
  const float importanceOfXP,
  const float importanceOfBonus,
  const SkillBuild& skillBuild):
  audacity(audacity),
  importanceOfXP(importanceOfXP),
  importanceOfBonus(importanceOfBonus),
  skillBuild(skillBuild)
  {
  currentLevel = 0;
}

void Role::update(const model::Wizard& self, model::Move& move) {
  while (currentLevel < self.getLevel()) {
    const size_t branchIndex = currentLevel / 5;
    const size_t skillIndex = currentLevel % 5;
    const auto skill = skillBuild[branchIndex][skillIndex];

    move.setSkillToLearn(skill);

    currentLevel++;
  }

  printf("Speed: %f\n", EX::maxSpeed(self));
  printf("Magic Attack: %f\n", EX::magicMissleAttack(self));
  printf("Staff Attack: %f\n", EX::staffAttack(self));
  printf("Armor: %f\n", EX::armor(self));
  printf("Radius: %f\n", EX::attackRadius(self));
}