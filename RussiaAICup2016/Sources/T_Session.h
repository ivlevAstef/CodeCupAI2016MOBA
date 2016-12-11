
//
//File: T_Session.h
//Author: Ivlev Alexander. Stef
//Created: 09/12/2016
//

#pragma once

#include "model/Wizard.h"
#include "model/Move.h"

#include "C_Singleton.h"
#include "S_StrategyType.h"
#include "T_Message.h"
#include <unordered_map>

namespace AICup
{
  /// хранит и обновляет роль + скилл билд + стратегию + запускает тактику
  class Session: public Singleton<Session> {
  public:
    Session();

    void update(const model::Wizard& self, model::Move& move);

    inline const StrategyDTO& getStrategyDTO() const {
      return strategyDTO;
    }

  private:
    void initializeMessageIndex();

    void tacticsSettingsUpdate(const model::Wizard& self, model::Move& move);

    StrategyDTO strategyDTOByMessage(const Message& message);
    void sendMessages(const std::vector<Message>& messages, model::Move& move);

  private:
    StrategyDTO strategyDTO;

    /// мапинг id игрока, в номер сообщения
    std::unordered_map<long long, int> messageIndex;
  };
};