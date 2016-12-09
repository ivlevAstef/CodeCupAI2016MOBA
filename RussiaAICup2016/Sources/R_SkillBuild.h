#pragma once
//
//File: R_SkillBuild.h
//Author: Ivlev Alexander. Stef
//Created: 09/12/2016
//

#include "model\SkillType.h"
#include "model\Move.h"
#include "model\Wizard.h"
#include <vector>
#include <memory>

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

  class SkillBuild {
  public:
    //static SkillBuild skillBuildByMessage();

    SkillBuild(std::vector<SkillBranch> build);

    /// устанавливает скилы
    void update(const model::Wizard& self, model::Move& move);

  private:
    int currentLevel;
    std::vector<SkillBranch> build;
  };

  typedef std::shared_ptr<SkillBuild> SkillBuildPtr;
}