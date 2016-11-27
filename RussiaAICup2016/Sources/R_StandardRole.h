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
      Role(1, 1, 1, {
        SkillBranches::moveHast,
        SkillBranches::magicalDamageFrostBolt,
        SkillBranches::rangeMagicMissile,
        SkillBranches::armorShield,
        SkillBranches::meleeDamageFireBolt,
      }) {

    }
  };
}

