#include "T_Session.h"

#include "E_World.h"
#include "E_Game.h"

#include "R_Standard.h"
#include "R_DynamicSkillBuilds.h"

#include "R_Attacks.h"
#include "R_Supports.h"
#include "R_Tanks.h"

#include "T_Message.h"

#include <algorithm>

using namespace AICup;


Session::Session() {
  role = std::make_shared<StandardRole>();
  skillBuild = std::make_shared<StandardSkillBuild>();
  strategyDTO = {StrategyType::Base, {model::_LANE_UNKNOWN_, 0, 0}};

  isInitialized = false;
}

void Session::initialize() {
  initializeMessageIndex();
  initializeRoles();
}

void Session::initializeMessageIndex() {
  std::vector<long long> ids;

  for (const auto& wizard : World::model().getWizards()) {
    if (wizard.isMaster()) {
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

void Session::initializeRoles() {
  const static TacticsRole allRoles[4] = {
    TacticsRole::SupportStan,
    TacticsRole::AttackAOE,
    TacticsRole::SupportHaste,
    TacticsRole::AttackStan
  };

  size_t roleIndex = 0;
  for (const auto& wizard : World::model().getWizards()) {
    if (wizard.isMaster()) {
      //TODO: потом или random(), или какнибудь по ситуации учиться до пятого уровня определять что лучше
      roles[wizard.getId()] = TacticsRole::TankStan;
      continue;
    }

    roles[wizard.getId()] = allRoles[roleIndex++];
  }
}

void Session::update(const model::Wizard& self, model::Move& move) {
  if (!isInitialized) {
    initialize();
    isInitialized = false;
  }

  /// если командный режим
  if (Game::model().isRawMessagesEnabled()) {
    tacticsSettingsUpdate(self, move);

  } else {
    dynamicChangeSkillBuild(self);
  }

  role->update(self);
  skillBuild->update(self, move);
}

void Session::tacticsSettingsUpdate(const model::Wizard& self, model::Move& move) {
  if (self.isMaster()) {
    /// тактический модуль создает 5 сообщений (или ничего), где последнее принадлежит мастеру, остальные на посылку
    std::vector<Message> messages;

    if (!messages.empty()) {
      assert(5 == messages.size());

      strategyDTO = strategyDTOByMessage(messages[4]);

      messages.pop_back();
      sendMessages(messages, move);
    }

  } else if (!self.getMessages().empty()) {
    assert(4 == self.getMessages().size());

    const auto& modelMessage = self.getMessages()[messageIndex[self.getId()]]; //жесть :)
    strategyDTO = strategyDTOByMessage(Message::read(modelMessage));
  }

  role = roleByRoleType(roles[self.getId()]);
  skillBuild = skillBuildByRoleType(roles[self.getId()]);
}

RolePtr Session::roleByRoleType(TacticsRole roleType) {
  switch (roleType) {
    case AICup::TacticsRole::TankStan:
      return std::make_shared<TankRole>();
    case AICup::TacticsRole::TankAttack:
      return std::make_shared<TankRole>();
    case AICup::TacticsRole::SupportStan:
      return std::make_shared<SupportStanRole>();
    case AICup::TacticsRole::AttackAOE:
      return std::make_shared<AttackAOERole>();
    case AICup::TacticsRole::SupportHaste:
      return std::make_shared<SupportHasteRole>();
    case AICup::TacticsRole::AttackStan:
      return std::make_shared<AttackStanRole>();
  }
}

SkillBuildPtr Session::skillBuildByRoleType(TacticsRole roleType) {
  switch (roleType) {
    case AICup::TacticsRole::TankStan:
      return std::make_shared<TankSupportSkillBuild>();
    case AICup::TacticsRole::TankAttack:
      return std::make_shared<TankAttackSkillBuild>();
    case AICup::TacticsRole::SupportStan:
      return std::make_shared<SupportStanSkillBuild>();
    case AICup::TacticsRole::AttackAOE:
      return std::make_shared<AttackAOESkillBuild>();
    case AICup::TacticsRole::SupportHaste:
      return std::make_shared<SupportHasteSkillBuild>();
    case AICup::TacticsRole::AttackStan:
      return std::make_shared<AttackStanSkillBuild>();
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
      return StrategyDTO{StrategyType::Base, data};
    case TacticsType::AntiRush:
      return StrategyDTO{StrategyType::Base, data};
    case TacticsType::Observer:
      return StrategyDTO{StrategyType::Base, data};
    case TacticsType::KillTop:
      return StrategyDTO{StrategyType::Base, data};
    case TacticsType::Push:
      return StrategyDTO{StrategyType::Base, data};
    case TacticsType::Win:
      return StrategyDTO{StrategyType::Base, data};
    case TacticsType::Attack:
      return StrategyDTO{StrategyType::Base, data};
    case TacticsType::Defense:
      return StrategyDTO{StrategyType::Base, data};
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

//// для одиночкой игры
void Session::dynamicChangeSkillBuild(const model::Wizard& self) {
  /// если уже был сделан выбор по ветке, то её не меняем
  if (self.getLevel() > 0) {
    return;
  }

  const auto myLine = World::instance().positionToLine(self.getX(), self.getY());

  /// пока линия не известна, говорить что либо о том, что выбирать сложно
  if (myLine == model::_LANE_UNKNOWN_) {
    skillBuild = std::make_shared<StandardSkillBuild>();
    role = std::make_shared<StandardRole>();
    return;
  }

  const auto friendWizard = World::instance().wizardCount(myLine, Game::friendFaction());
  const auto enemyWizard = World::instance().wizardCount(myLine, Game::enemyFaction());
  const auto wizardImbalance = friendWizard - enemyWizard;


  if (wizardImbalance == 0) { /// если силы равны то оставляем обычный
    skillBuild = std::make_shared<StandardSkillBuild>();
    role = std::make_shared<StandardRole>();

  } else if (wizardImbalance >= 1) { /// наших больше
    skillBuild = std::make_shared<FastPushSkillBuild>();
    role = std::make_shared<FastPushRole>();

  } else if (wizardImbalance <= -1) { /// наших меньше
    skillBuild = std::make_shared<HardLineSkillBuild>();
    role = std::make_shared<HardLineRole>();

  }
}