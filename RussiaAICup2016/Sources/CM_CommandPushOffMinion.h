//
//File: CM_CommandPushOffMinion.h
//Author: Ivlev Alexander. Stef
//Created: 03/12/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "model\Minion.h"

namespace AICup
{
  class CommandPushOffMinion: public MoveCommand {
  public:
    CommandPushOffMinion(const model::Minion& minion);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

    double priority(const Wizard& self) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::Minion& minion;

    double distance;
    Position position;
  };
}