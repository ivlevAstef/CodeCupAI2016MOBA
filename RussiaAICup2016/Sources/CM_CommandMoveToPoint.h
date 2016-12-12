//
//File: CM_CommandMoveToPoint.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "C_Vector2D.h"
#include "E_Types.h"
#include "A_Move.h"
#include "A_PathFinder.h"

namespace AICup
{
  class CommandMoveToPoint: public MoveCommand {
  public:
    CommandMoveToPoint(const double x, const double y, const double priority);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const Position point;
    const double priority;
  };
}