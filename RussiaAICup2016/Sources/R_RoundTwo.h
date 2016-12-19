//
//File: R_RoundTwo.h
//Author: Ivlev Alexander. Stef
//Created: 27/11/2016
//

#pragma once

#include "R_Role.h"
#include "R_SkillBuild.h"
#include "C_Extensions.h"

namespace AICup
{
  class RoundTwoSkillBuild: public SkillBuild {
  public:
    RoundTwoSkillBuild() : SkillBuild({
      SkillBranches::moveHast,
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::meleeDamageFireBolt,
      SkillBranches::armorShield,
      SkillBranches::rangeMagicMissile,

    }) {

    }
  };

  class RoundTwoRole: public Role {
  public:
    RoundTwoRole() {
      audacity = 0;
      linePressureWizards = 0.25;
      lineAudacityWizards = 2.0;

      importanceOfXP = 0.5;
      importanceOfBonus = 0.75;

      buildPriority = 3.0;
      minionPriority = 1.0;
      treePriority = 7.0;
      wizardPriority = 1.5;

      audacityBuild = 1.0;
      audacityMinion = 2.0;
      audacityWizard = 3.5;
      attackSkillPriority = 2.5;

      desireChangeLine = 0.9;
      changeLineForeFrontPriority = 0.0;
      changeLinePathLengthPriority = 1.0;
      changeLineWizardCountPriority = -3.0;
      changeLineWizardCountOnlyFriend = false;
      changeLineTowerBalancePriority = -0.5; // бежим туда где больше вражеских вышек, при прочих равных
      changeLineLaneStrengthPriority = 0.0;
      changeLineExpiriencePriority = 1.5;

      attackMeleeWinThreshold = 0.7;
      attackWizardMeleePriority = 0.5;
      attackBuildMeleePriority = 1.0;
      attackMinionMeleePriority = 1.0;

      friendWizardConfidence = 0.25;

      repulsionDodgeFireballPriority = 3.0;
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = -3.5 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = 0.05 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 0.5 + 3.5 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };


  class HardLineSkillBuild: public SkillBuild {
  public:
    HardLineSkillBuild() : SkillBuild({
      SkillBranches::moveHast,
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::armorShield,
      SkillBranches::rangeMagicMissile,
      SkillBranches::meleeDamageFireBolt
    }) {

    }
  };

  class HardLineRole: public RoundTwoRole {
  public:
    HardLineRole() : RoundTwoRole() {
      importanceOfXP = 2.5;
      importanceOfBonus = 0.4;

      buildPriority = 0.8;
      minionPriority = 1.2;
      wizardPriority = 1.2;

      audacityBuild = 5.0;
      audacityMinion = 1.5;
      audacityWizard = 2.0;
    }
  };

  class FastPushSkillBuild: public SkillBuild {
  public:
    FastPushSkillBuild() : SkillBuild({
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::moveHast,
      SkillBranches::meleeDamageFireBolt,
      SkillBranches::rangeMagicMissile,
      SkillBranches::armorShield,
    }) {

    }
  };


  class FastPushRole: public RoundTwoRole {
  public:
    FastPushRole() : RoundTwoRole() {
      importanceOfXP = 1.5;
      importanceOfBonus = 2.0;

      buildPriority = 4.0;
      minionPriority = 1.5;
      wizardPriority = 1.0;

      audacityBuild = 2.0;
      audacityMinion = 1.0;
      audacityWizard = 1.5;
    }
  };
}

