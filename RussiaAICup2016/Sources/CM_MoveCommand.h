//
//File: CM_MoveCommand.h
//Author: Ivlev Alexander. Stef
//Created: 17/11/2016
//

#pragma once

#include "CM_Command.h"
#include "E_Types.h"
#include "C_Vector2D.h"
#include "A_PathFinder.h"

namespace AICup
{
  class MoveCommand: public Command {
  public:
    struct Result {
      Vector moveDirection; /// вместе с длиной
      TurnStyle turnStyle;

      double speedLimit;

      int priority;

      Obstacles treesForRemove;
    };

  public:
    MoveCommand(Algorithm::PathFinder& finder) : pathFinder(finder) {
    }

    /// исполнить команду
    virtual void execute(const Wizard& self, Result& result) = 0;

  protected:
    Algorithm::PathFinder& pathFinder;
  };

  typedef std::shared_ptr<MoveCommand> MoveCommandPtr;
}

