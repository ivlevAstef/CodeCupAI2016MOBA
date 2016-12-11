#include "R_SkillBuild.h"

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


SkillBuild::SkillBuild(std::vector<SkillBranch> build) : build(build) {
  currentLevel = 0;
}


/// устанавливает скилы
void SkillBuild::update(const model::Wizard& self, model::Move& move) {
  // не цикл, ибо нельзя установить в один тик несколько скилов
  if (currentLevel < self.getLevel()) {
    const size_t branchIndex = currentLevel / 5;
    const size_t skillIndex = currentLevel % 5;

    const auto skill = build[branchIndex][skillIndex];

    move.setSkillToLearn(skill);

    currentLevel++;
  }
}
