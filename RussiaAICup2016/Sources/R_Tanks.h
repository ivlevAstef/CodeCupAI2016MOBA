//
//File: R_Tanks.h
//Author: Ivlev Alexander. Stef
//Created: 10/12/2016
//

#pragma once

#include "R_Role.h"
#include "R_SkillBuild.h"
#include "C_Extensions.h"

namespace AICup
{
  class TankAttackSkillBuild: public SkillBuild {
  public:
    TankAttackSkillBuild() : SkillBuild({
      SkillBranches::armorShield,
      SkillBranches::meleeDamageFireBolt,
      SkillBranches::moveHast,
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::rangeMagicMissile,
    }) {

    }
  };

  class TankSupportSkillBuild: public SkillBuild {
  public:
    TankSupportSkillBuild() : SkillBuild({
      SkillBranches::armorShield,
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::moveHast,
      SkillBranches::meleeDamageFireBolt,
      SkillBranches::rangeMagicMissile,
    }) {

    }
  };

  /// TODO: подогнать цифры
  class TankRole: public Role {
  public:
    TankRole() {
      audacity = 0;
      linePressureWizards = 0.25;
      lineAudacityWizards = 2.0;

      importanceOfXP = 2.5;
      importanceOfBonus = 1.0;


      buildPriority = 8.0;
      minionPriority = 1.0;
      treePriority = 7.0;
      wizardPriority = 1.5;

      audacityBuild = 1.0;
      audacityMinion = 0.5;
      audacityWizard = 0.75;
      attackSkillPriority = 1.5;

      desireChangeLine = 0.98;
      changeLineForeFrontPriority = 0.5;
      changeLinePathLengthPriority = 0.5;
      changeLineWizardCountPriority = 1.5;
      changeLineWizardCountOnlyFriend = false;
      changeLineTowerBalancePriority = -0.5; // бежим туда где меньше своих вышек, при прочих равных
      changeLineLaneStrengthPriority = 1.0;
      changeLineExpiriencePriority = 0.5;

      attackMeleeWinThreshold = 0.5;

      attackWizardMeleePriority = 5.0;
      attackBuildMeleePriority = 5.0;
      attackMinionMeleePriority = 2.5;

      friendWizardConfidence = 1.0;

      repulsionDodgeFireballPriority = 1.0;
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = -3.0 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = 1.25 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 4.0 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };

  class TankAttackRole: public TankRole {
  public:
    TankAttackRole(): TankRole() {
    }
  };

  class TankSupportRole: public TankRole {
  public:
    TankSupportRole() : TankRole() {
    }
  };
}