//
//File: T_Tactics.h
//Author: Ivlev Alexander. Stef
//Created: 11/12/2016
//

#pragma once

#include "C_Singleton.h"
#include "T_Message.h"
#include "E_Wizard.h"
#include <unordered_map>

namespace AICup
{
  class Tactics: public Singleton<Tactics> {
  public:
    std::vector<Message> update(const model::Wizard& self, const std::unordered_map<long long, int>& idMessage);

  private:
    bool useStandard(const model::Wizard& self, std::vector<Message>& result);
    bool useAntiRush(const model::Wizard& self, std::vector<Message>& result);
    bool useRush(const model::Wizard& self, std::vector<Message>& result);

    std::vector<Message> finalResult(TacticsType tactics, const std::vector<Message>& data);


  private:
    TacticsType lastType;
  };
};