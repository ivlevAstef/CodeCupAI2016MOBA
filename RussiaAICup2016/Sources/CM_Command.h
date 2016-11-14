//
//File: CM_Command.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include <vector>
#include <memory>
#include "model\Wizard.h"
#include "model\Move.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{
  class Command;
  typedef std::shared_ptr<Command> CommandPtr;

  class Command {
  public:

    /// проверяет возможность исполнения команды
    virtual bool check(const model::Wizard& self) = 0;

    virtual int priority(const model::Wizard& self) = 0;

    /// исполнить команду
    virtual void execute(const model::Wizard& self, model::Move& move) = 0;


#ifdef ENABLE_VISUALIZATOR
    virtual void visualization(const Visualizator&) const { }
#endif // ENABLE_VISUALIZATOR
  };
}

