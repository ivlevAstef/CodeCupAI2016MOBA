//
//File: S_StrategyType.h
//Author: Ivlev Alexander. Stef
//Created: 09/12/2016
//

#pragma once

#include "model/LaneType.h"

namespace AICup
{
  enum class StrategyType {
    Base,

    /// коммандные
    Standart,
    AntiRush,
    Observer,
    KillTop,
    Push,
    Win,
    Attack,
    Defense,

#ifdef ENABLE_TESTS
    Test_Move,
    Test_MoveAndAttack,
    Test_Follow,
    Test_Dodge
#endif
  };

  /// не все поля используются, для всех типов стратегий
  struct StrategyData {
    model::LaneType lane;
    int attactTick;
    long long attackedWizardId;
  };

  struct StrategyDTO {
    StrategyType type;
    StrategyData data;
  };
};