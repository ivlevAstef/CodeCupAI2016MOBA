//
//File: R_AntiRush.h
//Author: Ivlev Alexander. Stef
//Created: 12/12/2016
//

#pragma once

#include "R_Role.h"
#include "R_SkillBuild.h"
#include "C_Extensions.h"

namespace AICup
{
  /// TODO: подогнать цифры
  class AntiRushRole: public Role {
  public:
    AntiRushRole() {
      audacity = 0;
      linePressureWizards = 2.25;
      lineAudacityWizards = 2.0;

      importanceOfXP = 0.5;
      importanceOfBonus = 0.25;


      buildPriority = 0.1;
      minionPriority = 0.25;
      treePriority = 3.0;
      wizardPriority = 5.0;

      audacityBuild = 5.0;
      audacityMinion = 0.1;
      audacityWizard = 0.0;
      attackSkillPriority = 3.5;

      attackMeleeWinThreshold = 0.0;

      attackWizardMeleePriority = 1.0;
      attackBuildMeleePriority = 0.1;
      attackMinionMeleePriority = 0.5;

      wizardByIdPriority = 200;
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = -2.0 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = 2.25 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 4.0 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };
}