//
//File: CM_CommandManager.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "C_Singleton.h"

#include "CM_Command.h"
#include "CM_CommandFabric.h"

namespace AICup
{
  class CommandManager: public Singleton<CommandManager> {
  public:

  private:
    CommandFabric fabric;
  };
}