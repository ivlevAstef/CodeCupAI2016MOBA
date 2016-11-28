#pragma once
//
//File: R_Role.h
//Author: Ivlev Alexander. Stef
//Created: 27/11/2016
//

#pragma once

#include "model\SkillType.h"
#include "model\Wizard.h"
#include "model\Move.h"
#include <vector>

namespace AICup
{
  class SkillBranches {
  public:
    const static std::vector<model::SkillType> rangeMagicMissile;
    const static std::vector<model::SkillType> magicalDamageFrostBolt;
    const static std::vector<model::SkillType> meleeDamageFireBolt;
    const static std::vector<model::SkillType> moveHast;
    const static std::vector<model::SkillType> armorShield;
  };

  typedef std::vector<model::SkillType> SkillBranch;
  typedef std::vector<SkillBranch> SkillBuild;

  class Role {
  public:
    Role(const float audacity,
      const float importanceOfXP,
      const float importanceOfBonus,
      const SkillBuild& skillBuild);

    /// устанавливает скилы
    virtual void update(const model::Wizard& self, model::Move& move);


    inline const float& getAudacity() const {
      return audacity;
    }

    inline const float& getImportanceOfXP() const {
      return importanceOfXP;
    }

    inline const float& getImportanceOfBonus() const {
      return importanceOfBonus;
    }

    inline const SkillBuild& getSkillBuild() const {
      return skillBuild;
    }

    /// приоритеты атаки
    inline const float& getBuildPriority() const {
      return buildPriority;
    }
    inline const float& getMinionPriority() const {
      return minionPriority;
    }
    inline const float& getTreePriority() const {
      return treePriority;
    }
    inline const float& getWizardPriority() const {
      return wizardPriority;
    }

    /// дерзость по типам
    inline const float& getAudacityBuild() const {
      return audacityBuild;
    }
    inline const float& getAudacityMinion() const {
      return audacityMinion;
    }
    inline const float& getAudacityWizard() const {
      return audacityWizard;
    }

  protected:
    float audacity; // дерзость. 0 не дерзкий, 1 - нормальна€, вли€ет на расположение на линии
    float importanceOfXP; // ¬ажность получени€ XP.  0 - не нужен, 1 - нормальна€
    float importanceOfBonus; // ¬ажность вз€ти€ бонуса. 0 - не нужен, 1 - нормальна€

    /// приоритеты атаки. 0 не будет бить никогда, 1 - нормальна€
    float buildPriority;
    float minionPriority;
    float treePriority;
    float wizardPriority;

    /// насколько приоритетно избегание объектов
    float audacityBuild;
    float audacityMinion;
    float audacityWizard;


    SkillBuild skillBuild; // ѕоследовательность изучени€ скилов

  private:
    int currentLevel;
  };
}

