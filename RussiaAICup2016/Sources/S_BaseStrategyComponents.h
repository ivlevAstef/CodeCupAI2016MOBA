//
//File: S_BaseStrategyComponents.h
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//

#pragma once

#include "S_CommandStrategy.h"

namespace AICup
{
  class BaseStrategyComponents: public CommandStrategy {
  public:
    BaseStrategyComponents(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder, RolePtr role, SkillBuildPtr skillBuild);

  protected:
    model::LaneType checkAndChangeLane(const Wizard& self);

    void addAroundEnemiesOrMoveMelee(const Wizard& self);
    void addAttackFollow(const Wizard& self);

    void addMoveTo(const Wizard& self, model::LaneType lane);

    void addAttacks(const Wizard& self);
    void addCasts(const Wizard& self);
  };
};