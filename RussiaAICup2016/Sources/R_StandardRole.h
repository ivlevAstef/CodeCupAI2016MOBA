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
      Role({
        SkillBranches::magicalDamageFrostBolt,
        SkillBranches::rangeMagicMissile,
        SkillBranches::moveHast,
        SkillBranches::meleeDamageFireBolt,
        SkillBranches::armorShield,
      }) {
      audacity = 0;

      audacityBuild = 2;
      audacityMinion = 0.9;
      audacityWizard = 1.5;
    }

    void update(const model::Wizard& self, model::Move& move) override {
      Role::update(self, move);

      audacity = - (1 - (self.getLife() / self.getMaxLife()));
    }
  };
}

