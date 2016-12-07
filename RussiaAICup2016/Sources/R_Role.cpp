#include "R_Role.h"
#include "C_Extensions.h"
#include "E_Wizard.h"
#include "E_World.h"

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

Role::Role(const SkillBuild& skillBuild): skillBuild(skillBuild) {
  audacity = 1;
  linePressureWizards = 1;
  lineAudacityWizards = 1;
  importanceOfXP = 1;
  importanceOfBonus = 1;

  currentLevel = 0;

  buildPriority = 1;
  minionPriority = 1;
  treePriority = 1;
  wizardPriority = 1;
  attackSkillPriority = 1;

  audacityBuild = 1;
  audacityMinion = 1;
  audacityWizard = 1;

  desireChangeLine = 1;
  changeLinePathLengthPriority = 1;
  changeLineWizardCountPriority = 1;
  changeLineTowerBalancePriority = 1;
  changeLineLaneStrengthPriority = 1;
}


void Role::update(const Wizard& self, model::Move& move) {
  useStartedLinePriority = World::model().getTickIndex() < 750;

  while (currentLevel < self.getLevel()) {
    const size_t branchIndex = currentLevel / 5;
    const size_t skillIndex = currentLevel % 5;
    const auto skill = skillBuild[branchIndex][skillIndex];

    move.setSkillToLearn(skill);

    currentLevel++;
  }
}