//
//File: CM_CommandManager.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "model\Wizard.h"
#include "model\Move.h"

#include "C_Singleton.h"

#include "CM_Command.h"
#include "CM_CommandFabric.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{
  class CommandManager: public Singleton<CommandManager> {
  public:
    CommandManager();

    void update(const model::Wizard& self, model::Move& move);

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR


  private:
    std::vector<CommandPtr> commands;

    CommandFabric fabric;
  };
}