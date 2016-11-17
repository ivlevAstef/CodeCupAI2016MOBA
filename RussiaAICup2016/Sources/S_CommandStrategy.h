//
//File: CM_CommandStategy.h
//Author: Ivlev Alexander. Stef
//Created: 13/11/2016
//

#pragma once

#include "model\Wizard.h"
#include "model\Move.h"

#include "CM_Command.h"
#include "CM_CommandFabric.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif


namespace AICup
{
  class CommandStategy {
  public:
    CommandStategy(const CommandFabric& fabric);

    virtual void update(const model::Wizard& self, model::Move& move) = 0;

#ifdef ENABLE_VISUALIZATOR
    virtual void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR


  protected:
    const CommandFabric& fabric;
  };

  typedef std::shared_ptr<CommandStategy> CommandStategyPtr;
};