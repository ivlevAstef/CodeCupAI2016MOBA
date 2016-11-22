//
//File: S_TestFollowStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 13/11/2016
//

#pragma once

#include "S_CommandStrategy.h"

namespace AICup
{
  class TestFollowStrategy: public CommandStrategy {
  public:
    TestFollowStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder);
  };
};