//
//File: R_RolePusher.h
//Author: Ivlev Alexander. Stef
//Created: 30/11/2016
//

#pragma once

#include "R_Role.h"
#include "E_Wizard.h"

namespace AICup
{
  class RolePusher: public Role {
  public:
    RolePusher() :
      Role({
        SkillBranches::magicalDamageFrostBolt,
        SkillBranches::moveHast,
        SkillBranches::armorShield,
        SkillBranches::rangeMagicMissile,
        SkillBranches::meleeDamageFireBolt,
      }) {
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
      changeLinePathLengthPriority = 1.0;
      changeLineWizardCountPriority = 0.5;
      changeLineTowerBalancePriority = 0.75;
      changeLineLaneStrengthPriority = 0.35;
    }

    void update(const Wizard& self, model::Move& move) override {
      Role::update(self, move);

      const auto realLife = self.getLife() - self.burnResidualDamage();

      audacity = -3.5 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = 0.5 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 1.0 + 5.0 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };
}

