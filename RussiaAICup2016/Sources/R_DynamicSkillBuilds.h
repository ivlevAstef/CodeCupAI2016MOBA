//
//File: R_DynamicSkillBuilds.h
//Author: Ivlev Alexander. Stef
//Created: 10/12/2016
//

#pragma once

#include "R_Standard.h"
#include "R_SkillBuild.h"
#include "C_Extensions.h"

namespace AICup
{
  class HardLineSkillBuild: public SkillBuild {
  public:
    HardLineSkillBuild() : SkillBuild({
      SkillBranches::moveHast,
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::armorShield,
      SkillBranches::rangeMagicMissile,
      SkillBranches::meleeDamageFireBolt
    }) {

    }
  };

  class HardLineRole: public StandardRole {
  public:
    HardLineRole(): StandardRole() {
      importanceOfXP = 2.5;
      importanceOfBonus = 0.4;

      buildPriority = 0.8;
      minionPriority = 1.2;
      wizardPriority = 1.2;

      audacityBuild = 5.0;
      audacityMinion = 1.5;
      audacityWizard = 2.0;
    }
  };

  class FastPushSkillBuild: public SkillBuild {
  public:
    FastPushSkillBuild() : SkillBuild({
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::moveHast,
      SkillBranches::meleeDamageFireBolt,
      SkillBranches::rangeMagicMissile,
      SkillBranches::armorShield,
    }) {

    }
  };


  class FastPushRole: public StandardRole {
  public:
    FastPushRole() : StandardRole() {
      importanceOfXP = 1.5;
      importanceOfBonus = 2.0;

      buildPriority = 4.0;
      minionPriority = 1.5;
      wizardPriority = 1.0;

      audacityBuild = 2.0;
      audacityMinion = 1.0;
      audacityWizard = 1.5;
    }
  };
}

