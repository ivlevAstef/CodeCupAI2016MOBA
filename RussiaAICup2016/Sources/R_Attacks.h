//
//File: R_Attacks.h
//Author: Ivlev Alexander. Stef
//Created: 10/12/2016
//

#pragma once

#include "R_Role.h"
#include "R_SkillBuild.h"
#include "C_Extensions.h"

namespace AICup
{
  class AttackStanSkillBuild: public SkillBuild {
  public:
    AttackStanSkillBuild() : SkillBuild({
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::moveHast,
      SkillBranches::rangeMagicMissile,
      SkillBranches::meleeDamageFireBolt,
      SkillBranches::armorShield,
    }) {

    }
  };

  class AttackAOESkillBuild: public SkillBuild {
  public:
    AttackAOESkillBuild() : SkillBuild({
      SkillBranches::meleeDamageFireBolt,
      SkillBranches::armorShield,
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::rangeMagicMissile,
      SkillBranches::moveHast,
    }) {

    }
  };

  /// TODO: подогнать цифры
  class AttackRole: public Role {
  public:
    AttackRole() {
      audacity = 0;
      linePressureWizards = 0.25;
      lineAudacityWizards = 2.0;

      importanceOfXP = 2.5;
      importanceOfBonus = 1.0;


      buildPriority = 5.0;
      minionPriority = 1.0;
      treePriority = 7.0;
      wizardPriority = 1.5;

      audacityBuild = 1.0;
      audacityMinion = 0.8;
      audacityWizard = 1.2;
      attackSkillPriority = 1.5;

      desireChangeLine = 0.9;
      changeLineForeFrontPriority = 0.15;
      changeLinePathLengthPriority = 1.5;
      changeLineWizardCountPriority = -0.5; /// там где меньше врагов
      changeLineWizardCountOnlyFriend = false;
      changeLineTowerBalancePriority = 2.0; // бежим туда где больше вражеских вышек, при прочих равных
      changeLineLaneStrengthPriority = 0.9;
      changeLineExpiriencePriority = 0.8;

      attackMeleeWinThreshold = 0.4;

      friendWizardConfidence = 1.0;

      repulsionDodgeFireballPriority = 4.0;
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = -2.5 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = -0.5 + 0.75 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 0.5 + 3.5 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };

  class AttackStanRole: public AttackRole {
  public:
    AttackStanRole() : AttackRole() {
      attackWizardMeleePriority = 0.5;
      attackBuildMeleePriority = 1.0;
      attackMinionMeleePriority = 1.0;
    }
  };

  class AttackAOERole: public AttackRole {
  public:
    AttackAOERole(): AttackRole() {
      attackWizardMeleePriority = 5.0;
      attackBuildMeleePriority = 2.0;
      attackMinionMeleePriority = 3.0;
    }
  };
}