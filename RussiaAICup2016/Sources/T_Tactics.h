//
//File: T_Tactics.h
//Author: Ivlev Alexander. Stef
//Created: 11/12/2016
//

#pragma once

#include "C_Singleton.h"
#include "T_Message.h"
#include "E_Wizard.h"

namespace AICup
{
  class Tactics: public Singleton<Tactics> {
  public:
    std::vector<Message> update(const model::Wizard& self);

  private:
    std::vector<Message> useRush(const model::Wizard& self);
  };
};