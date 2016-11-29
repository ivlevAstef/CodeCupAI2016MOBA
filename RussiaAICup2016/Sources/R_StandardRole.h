//
//File: R_StandardRole.h
//Author: Ivlev Alexander. Stef
//Created: 27/11/2016
//

#pragma once

#include "R_Role.h"

namespace AICup
{
  class StandardRole: public Role {
  public:
    StandardRole() :
      Role(0.75, 1, 1, {
        SkillBranches::magicalDamageFrostBolt,
        SkillBranches::meleeDamageFireBolt,
        SkillBranches::rangeMagicMissile,
        SkillBranches::moveHast,
        SkillBranches::armorShield,
      }) {
      audacityBuild = 2;
      audacityMinion = 0.9;
      audacityWizard = 1.5;
    }

    void update(const model::Wizard& self, model::Move& move) override {
      Role::update(self, move);

      audacity = 0.75 * self.getLife() / self.getMaxLife();
    }
  };
}

