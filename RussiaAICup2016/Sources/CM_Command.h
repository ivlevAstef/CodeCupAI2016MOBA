//
//File: CM_Command.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include <memory>
#include "E_Wizard.h"

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
    virtual bool check(const Wizard&) = 0;

    /// значения до 1000, если оно меньше 1 то команда игронируется
    virtual double priority(const Wizard& self) = 0;

#ifdef ENABLE_VISUALIZATOR
    virtual void visualization(const model::Wizard&, const Visualizator&) const { }
#endif // ENABLE_VISUALIZATOR
  };
}

