//
//File: CM_MoveCommand.h
//Author: Ivlev Alexander. Stef
//Created: 17/11/2016
//

#pragma once

#include "CM_Command.h"
#include "E_Types.h"
#include "C_Vector2D.h"

namespace AICup
{
  class MoveCommand;
  typedef std::shared_ptr<MoveCommand> MoveCommandPtr;

  class MoveCommand: public Command {
  public:
    struct Result {
      Vector moveDirection; /// вместе с длиной
      TurnStyle turnStyle;

      int priority;
    };

  public:
    /// исполнить команду
    virtual void execute(const model::Wizard& self, Result& result) = 0;
  };
}

