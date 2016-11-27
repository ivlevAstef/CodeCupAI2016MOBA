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
        SkillBranches::rangeMagicMissile,
        SkillBranches::moveHast,
        SkillBranches::armorShield,
        SkillBranches::meleeDamageFireBolt,
      }) {

    }
  };
}

