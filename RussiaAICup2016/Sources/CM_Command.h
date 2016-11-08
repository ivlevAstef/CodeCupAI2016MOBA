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
  class CommandFabric;
  typedef std::shared_ptr<Command> CommandPtr;

  class Command {
  public:
    /// исполнить команду, возвращает false если команду исполнить невозможно
    virtual bool execute(const model::Wizard& self, model::Move& move) = 0;

    /// команды которые можно исполнить, чтобы стало возможным исполнить текущую команду
    virtual const std::vector<CommandPtr>& neededCommands(const CommandFabric& fabric) = 0;


#ifdef ENABLE_VISUALIZATOR
    virtual void visualization(const Visualizator& visualizator) const { }
#endif // ENABLE_VISUALIZATOR
  };
}

