#include "T_Session.h"

#include "E_World.h"
#include "E_Game.h"

#include "T_Message.h"
#include "T_Tactics.h"

#include <algorithm>

using namespace AICup;

/// вообще часть кода написанного тут, не относится к этому классу...


Session::Session() {
  /// стандартная будет с раунда второго, так как она способна работать и в финале и в первом раунде
  strategyDTO = {StrategyType::RoundTwo, {model::_LANE_UNKNOWN_, 0, 0}};

  initializeMessageIndex();
}

void Session::initializeMessageIndex() {
  std::vector<long long> ids;

  for (const auto& wizard : World::model().getWizards()) {
    if (wizard.isMaster() || wizard.getFaction() != Game::friendFaction()) {
      continue;
    }
    ids.push_back(wizard.getId());
  }

  std::sort(ids.begin(), ids.end());

  assert(4 == ids.size());
  for (size_t index = 0; index < ids.size(); index++) {
    messageIndex[ids[index]] = index;
  }
}

void Session::update(const model::Wizard& self, model::Move& move) {
  if (Game::model().isRawMessagesEnabled()) {
    tacticsSettingsUpdate(self, move);

  } else if (Game::model().isSkillsEnabled()) {
    strategyDTO = {StrategyType::RoundTwo, {model::_LANE_UNKNOWN_, 0, 0}};
  } else {
    strategyDTO = {StrategyType::RoundOne, {model::_LANE_UNKNOWN_, 0, 0}};
  }
}

void Session::tacticsSettingsUpdate(const model::Wizard& self, model::Move& move) {
  //strategyDTO = {StrategyType::Standart, {model::_LANE_UNKNOWN_, 0, 0}};
  //return;

  if (self.isMaster()) {
    /// тактический модуль создает 5 сообщений (или ничего), где последнее принадлежит мастеру, остальные на посылку
    std::vector<Message> messages = Tactics::instance().update(self);

    if (!messages.empty()) {
      assert(5 == messages.size());

      strategyDTO = strategyDTOByMessage(messages[4]);

      messages.pop_back();
      sendMessages(messages, move);
    }

  } else if (!self.getMessages().empty()) {
    const auto& modelMessage = self.getMessages().back();
    strategyDTO = strategyDTOByMessage(Message::read(modelMessage));
  }
}


StrategyDTO Session::strategyDTOByMessage(const Message& message) {
  //TODO: не забыть потом добавить если что сюда
  const StrategyData data = StrategyData{
    message.getLane(),
    message.getAttackTick(),
    message.getAttackedWizardId()
  };

  /// Добавить самих стратегий
  switch (message.getTacticsType()) {
    case TacticsType::Standart:
      return StrategyDTO{StrategyType::Standart, data};
    case TacticsType::AntiRush:
      return StrategyDTO{StrategyType::AntiRush, data};
    case TacticsType::Rush:
      return StrategyDTO{StrategyType::Rush5, data};
    case TacticsType::Observer:
      return StrategyDTO{StrategyType::Observer, data};
    case TacticsType::KillTop:
      return StrategyDTO{StrategyType::KillTop, data};
    case TacticsType::Push:
      return StrategyDTO{StrategyType::Push, data};
    case TacticsType::Win:
      return StrategyDTO{StrategyType::Win, data};
    case TacticsType::Attack:
      return StrategyDTO{StrategyType::Attack, data};
    case TacticsType::Defense:
      return StrategyDTO{StrategyType::Defense, data};
  }
}

void Session::sendMessages(const std::vector<Message>& messages, model::Move& move) {
  assert(4 == messages.size());

  std::vector<model::Message> modelMessages;
  for (const auto& message : messages) {
    modelMessages.push_back(message.model());
  }

  move.setMessages(modelMessages);
}