//
//File: R_Rush.h
//Author: Ivlev Alexander. Stef
//Created: 11/12/2016
//

#pragma once

#include "R_Role.h"
#include "R_SkillBuild.h"
#include "C_Extensions.h"

namespace AICup
{

  class AntiRushRole: public Role {
  public:
    AntiRushRole(const Role& parent) {

      audacity = parent.getAudacity();
      linePressureWizards = parent.getLinePressureWizards();
      lineAudacityWizards = parent.getAudacityWizard();

      importanceOfXP = parent.getImportanceOfXP();
      importanceOfBonus = parent.getImportanceOfBonus();


      buildPriority = parent.getBuildPriority();
      minionPriority = parent.getMinionPriority();
      treePriority = parent.getTreePriority();
      wizardPriority = parent.getWizardPriority();

      audacityBuild = parent.getAudacityBuild() * 2;
      audacityMinion = parent.getAudacityMinion() * 1.2;
      audacityWizard = parent.getAudacityWizard();
      attackSkillPriority = parent.getAttackSkillPriority();

      desireChangeLine = parent.getDesireChangeLine();

      changeLineForeFrontPriority = parent.getChangeLineForeFrontPriority();
      changeLinePathLengthPriority = parent.getChangeLinePathLengthPriority();
      changeLineWizardCountPriority = parent.getChangeLineWizardCountPriority();
      changeLineWizardCountOnlyFriend = false;
      changeLineTowerBalancePriority = parent.getChangeLineTowerBalancePriority(); // бежим туда где больше вражеских вышек, при прочих равных
      changeLineLaneStrengthPriority = parent.getChangeLineLaneStrengthPriority();
      changeLineExpiriencePriority = parent.getChangeLineExpiriencePriority();

      attackMeleeWinThreshold = 0.4;

      attackWizardMeleePriority = 10;
      attackMinionMeleePriority = 1;
      attackBuildMeleePriority = 0.5;

      friendWizardConfidence = 1.2;

      repulsionDodgeFireballPriority = parent.getRepulsionDodgeFireballPriority();
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = -3.5 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = -0.25 + 0.75 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 0.5 + 3.5 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };

}