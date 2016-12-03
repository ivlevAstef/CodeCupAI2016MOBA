//
//File: R_RolePusher.h
//Author: Ivlev Alexander. Stef
//Created: 30/11/2016
//

#pragma once

#include "R_Role.h"

namespace AICup
{
  class RolePusher: public Role {
  public:
    RolePusher() :
      Role({
        SkillBranches::magicalDamageFrostBolt,
        SkillBranches::moveHast,
        SkillBranches::rangeMagicMissile,
        SkillBranches::meleeDamageFireBolt,
        SkillBranches::armorShield,
      }) {
      audacity = -1.5;

      importanceOfXP = 1.5;
      importanceOfBonus = 0.9;


      buildPriority = 10.0;
      minionPriority = 1.0;
      treePriority = 3.5;
      wizardPriority = 1.5;

      audacityBuild = 3.0;
      audacityMinion = 1.0;
      audacityWizard = 1.25;
      attackSkillPriority = 1.5;

      desireChangeLine = 0.75;
      changeLinePathLengthPriority = 2.5;
      changeLineWizardCountPriority = 1;
      changeLineTowerBalancePriority = 1;
      changeLineLaneStrengthPriority = 1.5;
    }

    void update(const model::Wizard& self, model::Move& move) override {
      Role::update(self, move);

      audacity = -1.5 - 5.5 * (1 - (float(self.getLife()) / float(self.getMaxLife())));
    }
  };
}

