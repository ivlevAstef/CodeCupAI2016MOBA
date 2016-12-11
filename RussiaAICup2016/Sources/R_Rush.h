//
//File: R_Rush.h
//Author: Ivlev Alexander. Stef
//Created: 11/12/2016
//

#pragma once

#include "R_Role.h"
#include "R_SkillBuild.h"
#include "C_Extensions.h"

namespace AICup
{
  class RushSkillBuild: public SkillBuild {
  public:
    RushSkillBuild() : SkillBuild({
      SkillBranches::meleeDamageFireBolt,
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::moveHast,
      SkillBranches::armorShield,
      SkillBranches::rangeMagicMissile,
    }) {

    }
  };


  /// TODO: подогнать цифры
  class RushRole: public Role {
  public:
    RushRole() {
      audacity = 0;
      linePressureWizards = 0;
      lineAudacityWizards = 0;

      importanceOfXP = 1.5;
      importanceOfBonus = 1.0;


      buildPriority = 1.0;
      minionPriority = 0.85;
      treePriority = 1.0;
      wizardPriority = 1.0;

      audacityBuild = 0.0;
      audacityMinion = 2.5;
      audacityWizard = 1.0;
      attackSkillPriority = 2.5;

      desireChangeLine = 0.0;

      changeLineForeFrontPriority = 1.0;
      changeLinePathLengthPriority = 1.0;
      changeLineWizardCountPriority = 1.0;
      changeLineWizardCountOnlyFriend = false;
      changeLineTowerBalancePriority = 1.0; // бежим туда где больше вражеских вышек, при прочих равных
      changeLineLaneStrengthPriority = 1.0;

      attackMeleeWinThreshold = -1.0;

      attackWizardMeleePriority = 5;
      attackMinionMeleePriority = 5;
      attackBuildMeleePriority = 250;
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = -1.5 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = 1.0 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 0.5 + 1.5 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };
}