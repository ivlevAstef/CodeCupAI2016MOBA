//
//File: CM_CommandMoveToLine.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_Command.h"
#include "model\LaneType.h"
#include "CM_CommandMoveToPoint.h"

namespace AICup
{
  class CommandMoveToLine: public Command {
  public:
    CommandMoveToLine(model::LaneType line);

    bool check(const model::Wizard& self, model::Move& move);

    void execute(const model::Wizard& self, model::Move& move);

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::LaneType line;

    std::shared_ptr<CommandMoveToPoint> commandMoveToPoint;
  };
}