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
    Role(const SkillBuild& skillBuild);

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

    inline const float& getAttackSkillPriority() const {
      return attackSkillPriority;
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

    // смена линии
    inline const float& getDesireChangeLine() const {
      return desireChangeLine;
    }
    inline const float& getChangeLinePathLengthPriority() const {
      return changeLinePathLengthPriority;
    }
    inline const float& getChangeLineWizardCountPriority() const {
      return changeLineWizardCountPriority;
    }
    inline const float& getChangeLineTowerBalancePriority() const {
      return changeLineTowerBalancePriority;
    }
    inline const float& getChangeLineLaneStrengthPriority() const {
      return changeLineLaneStrengthPriority;
    }

  protected:
    float audacity; // дерзость. влияет на расположение на линии, каждая +1 увеличивает близость к врагу на 100, -1 увеличивает на 100
    float importanceOfXP; // Важность получения XP.  0 - не нужен, 1 - нормальная. влияет насколько сильно тянет к линии героя, и насколько сильно он приследует умирающие цели
    float importanceOfBonus; // Важность взятия бонуса. 0 - не нужен, 1 - нормальная

    /// приоритеты атаки. 0 не будет бить никогда, 1 - нормальная
    float buildPriority;
    float minionPriority;
    float treePriority;
    float wizardPriority;
    float attackSkillPriority;

    /// насколько приоритетно избегание объектов
    float audacityBuild;
    float audacityMinion;
    float audacityWizard;


    /// смена линии
    float desireChangeLine; /// насколько сильно бот будет хотеть менять линию - 0 никогда.
    float changeLinePathLengthPriority; /// насколько сильно влияет длина пути которую надо пройти до линии
    float changeLineWizardCountPriority; /// насколько сильно влияет количество своих магов на линии
    float changeLineTowerBalancePriority; /// насколько сильно влияет количество вышек на линии (свои - вражеские)
    float changeLineLaneStrengthPriority; /// насколько сильно влияет дизбаланс сил

    SkillBuild skillBuild; // Последовательность изучения скилов

  private:
    int currentLevel;
  };
}

