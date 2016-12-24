//
//File: R_Supports.h
//Author: Ivlev Alexander. Stef
//Created: 10/12/2016
//

#pragma once

#include "R_Role.h"
#include "R_SkillBuild.h"
#include "C_Extensions.h"

namespace AICup
{
  class SupportStanSkillBuild: public SkillBuild {
  public:
    SupportStanSkillBuild() : SkillBuild({
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::rangeMagicMissile,
      SkillBranches::moveHast,
      SkillBranches::armorShield,
      SkillBranches::meleeDamageFireBolt,
    }) {

    }
  };

  class SupportHasteSkillBuild: public SkillBuild {
  public:
    SupportHasteSkillBuild() : SkillBuild({
      SkillBranches::moveHast,
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::armorShield,
      SkillBranches::meleeDamageFireBolt,
      SkillBranches::rangeMagicMissile,
    }) {

    }
  };

  /// TODO: подогнать цифры
  class SupportRole: public Role {
  public:
    SupportRole() {
      audacity = 0;
      linePressureWizards = 0.25;
      lineAudacityWizards = 2.0;

      importanceOfXP = 1.5;
      importanceOfBonus = 1.0;


      buildPriority = 1.0;
      minionPriority = 0.75;
      treePriority = 5.0;
      wizardPriority = 1.5;

      audacityBuild = 1.0;
      audacityMinion = 1.0;
      audacityWizard = 2.5;
      attackSkillPriority = 3.5;

      desireChangeLine = 0.95;
      changeLineForeFrontPriority = 0.25;
      changeLinePathLengthPriority = 0.6;
      changeLineWizardCountPriority = 1.5;
      changeLineWizardCountOnlyFriend = false;
      changeLineTowerBalancePriority = 0.5; // бежим туда где больше вражеских вышек, при прочих равных
      changeLineLaneStrengthPriority = 1.0;
      changeLineExpiriencePriority = 0.2;

      attackMeleeWinThreshold = 0.8;

      attackWizardMeleePriority = 0.2;
      attackBuildMeleePriority = 0.5;
      attackMinionMeleePriority = 0.75;

      friendWizardConfidence = 1.0;

      repulsionDodgeFireballPriority = 6.0;
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = -4.0 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = -1.0 + 0.75 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 1.5 + 3.0 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };

  class SupportStanRole: public SupportRole {
  public:
    SupportStanRole() : SupportRole() {
    }
  };

  class SupportHasteRole: public SupportRole {
  public:
    SupportHasteRole() : SupportRole() {
    }
  };
}