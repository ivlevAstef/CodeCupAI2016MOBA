#pragma once
//
//File: R_Role.h
//Author: Ivlev Alexander. Stef
//Created: 27/11/2016
//

#include <memory>
#include "model\Wizard.h"

namespace AICup
{
  class Role {
  public:
    Role();

    /// устанавливает скилы
    virtual void update(const model::Wizard& self);


    inline const float& getAudacity() const {
      return audacity;
    }

    inline const float& getLinePressureWizards() const {
      return linePressureWizards;
    }
    inline const float& getLineAudacityWizard() const {
      return lineAudacityWizards;
    }

    inline const float& getImportanceOfXP() const {
      return importanceOfXP;
    }

    inline const float& getImportanceOfBonus() const {
      return importanceOfBonus;
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
    inline const float getDesireChangeLine() const {
      return !useStartedLinePriority ? desireChangeLine : 0.95f;
    }
    inline const float getChangeLineForeFrontPriority() const {
      return !useStartedLinePriority ? changeLineForeFrontPriority : 0.0f;
    }

    inline const float getChangeLinePathLengthPriority() const {
      return !useStartedLinePriority ? changeLinePathLengthPriority : 0.1f;
    }
    inline const float getChangeLineWizardCountPriority() const {
      return !useStartedLinePriority ? changeLineWizardCountPriority : 1.0f;
    }
    inline const bool getChangeLineWizardCountOnlyFriend() const {
      return !useStartedLinePriority ? changeLineWizardCountOnlyFriend : true;
    }
    inline const float getChangeLineTowerBalancePriority() const {
      return !useStartedLinePriority ? changeLineTowerBalancePriority : 0.0f;
    }
    inline const float getChangeLineLaneStrengthPriority() const {
      return !useStartedLinePriority ? changeLineLaneStrengthPriority : 0.0f;
    }

  protected:
    float audacity; // дерзость. влияет на расположение на линии, каждая +1 увеличивает близость к врагу на 100, -1 увеличивает на 100
    /// коэцифиенты выбираються в зависимости от ситуации на линии - если шансы на победу маленькие то боязнь, иначе прессование
    float linePressureWizards; // прессование магов на линии, влияет на расположение на линии, каждая +1 увеличивает близость к врагу на 100
    float lineAudacityWizards; // боязнь магов на линии, влияет на расположение на линии, каждая +1 увеличивает дальность от врага на 100

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
    float changeLineForeFrontPriority; /// насколько сильно влияет расположение линии
    float changeLinePathLengthPriority; /// насколько сильно влияет длина пути которую надо пройти до линии
    float changeLineWizardCountPriority; /// насколько сильно влияет количество своих магов на линии
    bool  changeLineWizardCountOnlyFriend; /// учитывать только своих магов, или своих - противника
    float changeLineTowerBalancePriority; /// насколько сильно влияет количество вышек на линии (свои - вражеские)
    float changeLineLaneStrengthPriority; /// насколько сильно влияет дизбаланс сил

  private:
    /// если только начала игры, то приоритет всегда один
    bool useStartedLinePriority;
  };

  typedef std::shared_ptr<Role> RolePtr;
}

