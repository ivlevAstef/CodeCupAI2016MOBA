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

      desireChangeLine = 0.91;
      changeLineForeFrontPriority = 0.25;
      changeLinePathLengthPriority = 0.75;
      changeLineWizardCountPriority = 1.5;
      changeLineWizardCountOnlyFriend = false;
      changeLineTowerBalancePriority = 1.0; // бежим туда где больше вражеских вышек, при прочих равных
      changeLineLaneStrengthPriority = 1.0;
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = -3.5 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = 0.75 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 0.5 + 3.5 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };

  class TankAttackRole: public TankRole {
  public:
    TankAttackRole() {
    }

    void update(const model::Wizard& self) override {
    }
  };

  class TankSupportRole: public TankRole {
  public:
    TankSupportRole() {
    }

    void update(const model::Wizard& self) override {
    }
  };
}