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

    /// ������������� �����
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

  protected:
    float audacity; // ��������. 0 �� �������, 1 - ����������
    float importanceOfXP; // �������� ��������� XP.  0 - �� �����, 1 - ����������
    float importanceOfBonus; // �������� ������ ������. 0 - �� �����, 1 - ����������
    SkillBuild skillBuild; // ������������������ �������� ������

  private:
    size_t currentLevel;
  };
}

