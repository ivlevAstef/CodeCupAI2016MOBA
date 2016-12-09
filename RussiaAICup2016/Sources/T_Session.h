
//
//File: T_Session.h
//Author: Ivlev Alexander. Stef
//Created: 09/12/2016
//

#pragma once

#include "model/Wizard.h"
#include "model/Move.h"

#include "C_Singleton.h"
#include "R_Role.h"
#include "R_SkillBuild.h"
#include "S_StrategyType.h"

namespace AICup
{
  /// хранит и обновляет роль + скилл билд + стратегию + запускает тактику
  class Session: public Singleton<Session> {
  public:
    Session();

    void update(const model::Wizard& wizard, model::Move& move);

    inline const Role& getRole() const {
      return *role;
    }

    inline const SkillBuild& getSkillBuild() const {
      return *skillBuild;
    }

    inline const StrategyType& getStrategyType() const {
      return strategyType;
    }

  private:
    RolePtr role;
    SkillBuildPtr skillBuild;
    StrategyType strategyType;
  };
};