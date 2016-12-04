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
        SkillBranches::meleeDamageFireBolt,
        SkillBranches::magicalDamageFrostBolt,
        SkillBranches::moveHast,
        SkillBranches::rangeMagicMissile,
        SkillBranches::armorShield,
      }) {
      audacity = 0;
      audacityWithWizards = -1.0;

      importanceOfXP = 1.5;
      importanceOfBonus = 0.9;


      buildPriority = 8.0;
      minionPriority = 1.0;
      treePriority = 3.5;
      wizardPriority = 1.5;

      audacityBuild = 3.0;
      audacityMinion = 1.0;
      audacityWizard = 1.25;
      attackSkillPriority = 1.5;

      desireChangeLine = 3.5;
      changeLinePathLengthPriority = 1.0;
      changeLineWizardCountPriority = 0.125;
      changeLineTowerBalancePriority = 0.5;
      changeLineLaneStrengthPriority = 0.25;
    }

    void update(const Wizard& self, model::Move& move) override {
      Role::update(self, move);

      const auto realLife = self.getLife() - self.burnResidualDamage();

      audacity = -3.5 * (1 - (float(realLife) / float(self.getMaxLife())));
      audacityWithWizards = -1.0 - 3.0 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };
}

