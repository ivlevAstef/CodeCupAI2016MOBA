//
//File: CM_CommandMoveGetExpirience.h
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "C_Vector2D.h"

namespace AICup
{
  class CommandMoveGetExpirience: public MoveCommand {
  public:
    CommandMoveGetExpirience();

    bool check(const model::Wizard& self) override;

    int priority(const model::Wizard& self) override;

    void execute(const model::Wizard& self, Result& result) override;


#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    MoveCommandPtr followCommand;

    Position expiriencePos;
  };
}