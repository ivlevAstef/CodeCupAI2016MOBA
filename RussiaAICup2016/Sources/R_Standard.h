//
//File: R_StandardRole.h
//Author: Ivlev Alexander. Stef
//Created: 27/11/2016
//

#pragma once

#include "R_Role.h"
#include "R_SkillBuild.h"
#include "C_Extensions.h"

namespace AICup
{
  class StandardSkillBuild: public SkillBuild {
  public:
    StandardSkillBuild() : SkillBuild({
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::moveHast,
      SkillBranches::armorShield,
      SkillBranches::rangeMagicMissile,
      SkillBranches::meleeDamageFireBolt
    }) {

    }
  };

  class StandardRole: public Role {
  public:
    StandardRole() {
      audacity = 0;
      linePressureWizards = 0.25;
      lineAudacityWizards = 2.0;

      importanceOfXP = 1.5;
      importanceOfBonus = 1.0;


      buildPriority = 8.0;
      minionPriority = 1.0;
      treePriority = 15.0;
      wizardPriority = 1.5;

      audacityBuild = 3.0;
      audacityMinion = 1.0;
      audacityWizard = 1.5;
      attackSkillPriority = 1.5;

      desireChangeLine = 0.7;
      changeLineForeFrontPriority = 1.0;
      changeLinePathLengthPriority = 1.5;
      changeLineWizardCountPriority = -1.0; /// чем больше вражеских магов, тем меньшее желание туда идти
      changeLineWizardCountOnlyFriend = false;
      changeLineTowerBalancePriority = 0.75;
      changeLineLaneStrengthPriority = 0.15;
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = -3.5 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = 0.75 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 0.5 + 3.5 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };
}

