//
//File: S_StrategyType.h
//Author: Ivlev Alexander. Stef
//Created: 09/12/2016
//

#pragma once

namespace AICup
{
  enum class StrategyType {
    Base,

#ifdef ENABLE_TESTS
    Test_Move,
    Test_MoveAndAttack,
    Test_Follow,
    Test_Dodge
#endif
  };
};