//
//File: R_StandardInfo.h
//Author: Ivlev Alexander. Stef
//Created: 12/12/2016
//


#pragma once

#include "C_Singleton.h"
#include "R_Role.h"
#include "R_SkillBuild.h"
#include "R_TacticsRole.h"
#include "model\Wizard.h"
#include <unordered_map>

namespace AICup
{
  class StandardInfo: public Singleton<StandardInfo> {
  public:
    StandardInfo();

    static RolePtr role(const model::Wizard&);
    static SkillBuildPtr skillBuild(const model::Wizard&);

    static TacticsRole tacticsRole(const model::Wizard&);

  private:
    void initializeRoles();

    /// для командрой игры, чтобы быстро понимать кто на какой роли
    std::unordered_map<long long, TacticsRole> roles;
  };
};
