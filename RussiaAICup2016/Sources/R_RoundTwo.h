//
//File: R_RoundTwo.h
//Author: Ivlev Alexander. Stef
//Created: 27/11/2016
//

#pragma once

#include "R_Role.h"
#include "R_SkillBuild.h"
#include "C_Extensions.h"


#ifdef FROM_FILE
#include <fstream>
#include <iostream>
#include <string>
#endif

namespace AICup
{
  class RoundTwoSkillBuild: public SkillBuild {
  public:
    RoundTwoSkillBuild() : SkillBuild({
      SkillBranches::moveHast,
      SkillBranches::magicalDamageFrostBolt,
      SkillBranches::meleeDamageFireBolt,
      SkillBranches::armorShield,
      SkillBranches::rangeMagicMissile,

    }) {

    }
  };


#ifndef FROM_FILE
  class RoundTwoRole: public Role {
  public:
    RoundTwoRole() {
      audacity = 0;
      linePressureWizards = 0.25;
      lineAudacityWizards = 2.0;

      importanceOfXP = 0.5;
      importanceOfBonus = 0.75;

      buildPriority = 3.0;
      minionPriority = 1.0;
      treePriority = 7.0;
      wizardPriority = 1.5;

      audacityBuild = 1.0;
      audacityMinion = 1.5;
      audacityWizard = 3.5;
      attackSkillPriority = 2.5;

      desireChangeLine = 0.9;
      changeLineForeFrontPriority = 0.5;
      changeLinePathLengthPriority = 1.25;
      changeLineWizardCountPriority = -0.5;
      changeLineWizardCountOnlyFriend = false;
      changeLineTowerBalancePriority = 0.5;
      changeLineLaneStrengthPriority = 0.4;
      changeLineExpiriencePriority = 1.5;

      attackMeleeWinThreshold = 0.5;
      attackWizardMeleePriority = 0.5;
      attackBuildMeleePriority = 1.0;
      attackMinionMeleePriority = 1.0;

      friendWizardConfidence = 0.5;

      repulsionDodgeFireballPriority = 3.0;
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = -3.5 * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = 0.35 * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = 0.5 + 3.5 * (1 - (float(realLife) / float(self.getMaxLife())));
    }
  };

#else

  class RoundTwoRole: public Role {
    RoundTwoRole() {
      audacity = initializer.audacity;
      linePressureWizards = initializer.linePressureWizards;
      lineAudacityWizards = initializer.lineAudacityWizards;

      importanceOfXP = initializer.importanceOfXP;
      importanceOfBonus = initializer.importanceOfBonus;

      buildPriority = initializer.buildPriority;
      minionPriority = initializer.minionPriority;
      treePriority = initializer.treePriority;
      wizardPriority = initializer.wizardPriority;

      audacityBuild = initializer.audacityBuild;
      audacityMinion = initializer.audacityMinion;
      audacityWizard = initializer.audacityWizard;
      attackSkillPriority = initializer.attackSkillPriority;

      desireChangeLine = initializer.desireChangeLine;
      changeLineForeFrontPriority = initializer.changeLineForeFrontPriority;
      changeLinePathLengthPriority = initializer.changeLinePathLengthPriority;
      changeLineWizardCountPriority = initializer.changeLineWizardCountPriority;
      changeLineWizardCountOnlyFriend = initializer.changeLineWizardCountOnlyFriend;
      changeLineTowerBalancePriority = initializer.changeLineTowerBalancePriority;
      changeLineLaneStrengthPriority = initializer.changeLineLaneStrengthPriority;
      changeLineExpiriencePriority = initializer.changeLineExpiriencePriority;

      attackMeleeWinThreshold = initializer.attackMeleeWinThreshold;
      attackWizardMeleePriority = initializer.attackWizardMeleePriority;
      attackBuildMeleePriority = initializer.attackBuildMeleePriority;
      attackMinionMeleePriority = initializer.attackMinionMeleePriority;

      friendWizardConfidence = initializer.friendWizardConfidence;

      repulsionDodgeFireballPriority = initializer.repulsionDodgeFireballPriority;

      audicityConst = initializer.audicityConst;
      pressureWizardConst = initializer.pressureWizardConst;
      audicityWizardConst = initializer.audicityWizardConst;
      audicityWizardMult = initializer.audicityWizardMult;
    }

    void update(const model::Wizard& self) override {
      Role::update(self);

      const auto realLife = self.getLife() - EX::burnResidualDamage(self);

      audacity = audicityConst * (1 - (float(realLife) / float(self.getMaxLife())));

      linePressureWizards = pressureWizardConst * float(realLife) / float(self.getMaxLife());
      lineAudacityWizards = audicityWizardConst + audicityWizardMult * (1 - (float(realLife) / float(self.getMaxLife())));
    }

  private:
    double audicityConst;
    double pressureWizardConst;
    double audicityWizardConst;
    double audicityWizardMult;

  private:
    static RoundTwoRole initializer;

    RoundTwoRole(std::string filename) {
      std::ifstream file(filename);

      audacity = 0;
      linePressureWizards = 0;
      lineAudacityWizards = 0;

      file >> importanceOfXP;
      file >> importanceOfBonus;

      file >> buildPriority;
      file >> minionPriority;
      file >> treePriority;
      file >> wizardPriority;

      file >> audacityBuild;
      file >> audacityMinion;
      file >> audacityWizard;
      file >> attackSkillPriority;

      file >> desireChangeLine;
      file >> changeLineForeFrontPriority;
      file >> changeLinePathLengthPriority;
      file >> changeLineWizardCountPriority;
      changeLineWizardCountOnlyFriend = false;
      file >> changeLineTowerBalancePriority;
      file >> changeLineLaneStrengthPriority;
      file >> changeLineExpiriencePriority;

      file >> attackMeleeWinThreshold;
      file >> attackWizardMeleePriority;
      file >> attackBuildMeleePriority;
      file >> attackMinionMeleePriority;

      file >> friendWizardConfidence;

      file >> repulsionDodgeFireballPriority;

      file >> audicityConst;
      file >> pressureWizardConst;
      file >> audicityWizardConst;
      file >> audicityWizardMult;
    }
  };

#endif
}

