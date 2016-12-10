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

    /// ������������� �����
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

    /// ���������� �����
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

    /// �������� �� �����
    inline const float& getAudacityBuild() const {
      return audacityBuild;
    }
    inline const float& getAudacityMinion() const {
      return audacityMinion;
    }
    inline const float& getAudacityWizard() const {
      return audacityWizard;
    }

    // ����� �����
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
    float audacity; // ��������. ������ �� ������������ �� �����, ������ +1 ����������� �������� � ����� �� 100, -1 ����������� �� 100
    /// ����������� ����������� � ����������� �� �������� �� ����� - ���� ����� �� ������ ��������� �� ������, ����� �����������
    float linePressureWizards; // ����������� ����� �� �����, ������ �� ������������ �� �����, ������ +1 ����������� �������� � ����� �� 100
    float lineAudacityWizards; // ������ ����� �� �����, ������ �� ������������ �� �����, ������ +1 ����������� ��������� �� ����� �� 100

    float importanceOfXP; // �������� ��������� XP.  0 - �� �����, 1 - ����������. ������ ��������� ������ ����� � ����� �����, � ��������� ������ �� ���������� ��������� ����
    float importanceOfBonus; // �������� ������ ������. 0 - �� �����, 1 - ����������

    /// ���������� �����. 0 �� ����� ���� �������, 1 - ����������
    float buildPriority;
    float minionPriority;
    float treePriority;
    float wizardPriority;
    float attackSkillPriority;

    /// ��������� ����������� ��������� ��������
    float audacityBuild;
    float audacityMinion;
    float audacityWizard;


    /// ����� �����
    float desireChangeLine; /// ��������� ������ ��� ����� ������ ������ ����� - 0 �������.
    float changeLineForeFrontPriority; /// ��������� ������ ������ ������������ �����
    float changeLinePathLengthPriority; /// ��������� ������ ������ ����� ���� ������� ���� ������ �� �����
    float changeLineWizardCountPriority; /// ��������� ������ ������ ���������� ����� ����� �� �����
    bool  changeLineWizardCountOnlyFriend; /// ��������� ������ ����� �����, ��� ����� - ����������
    float changeLineTowerBalancePriority; /// ��������� ������ ������ ���������� ����� �� ����� (���� - ���������)
    float changeLineLaneStrengthPriority; /// ��������� ������ ������ ��������� ���

  private:
    /// ���� ������ ������ ����, �� ��������� ������ ����
    bool useStartedLinePriority;
  };

  typedef std::shared_ptr<Role> RolePtr;
}

