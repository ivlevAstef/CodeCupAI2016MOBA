//
//File: S_BaseStrategyComponents.h
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//

#pragma once

#include "S_CommandStrategy.h"
#include "model\LaneType.h"

namespace AICup
{
  class BaseStrategyComponents: public CommandStrategy {
  public:
    BaseStrategyComponents(const CommandFabric& fabric, RolePtr role, SkillBuildPtr skillBuild);

  protected:
    bool changeLane(const Wizard& self);
    bool changeLane(const Wizard& self, int period);

    void addAroundEnemiesOrMoveMelee(const Wizard& self);
    void addAttackFollow(const Wizard& self);

    void addMoveTo(const Wizard& self, model::LaneType lane);

    void addAttacks(const Wizard& self);
    void addCasts(const Wizard& self);

    void repulsionDodgeFireball(const Wizard& self);

  protected:
    model::LaneType currentLane;

  private:
    int lastChangeLineTick;

    enum class LastOperation {
      LANE,
      BONUS
    };
    LastOperation lastOperation;

  };
};