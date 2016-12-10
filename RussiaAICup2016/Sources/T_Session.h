
//
//File: T_Session.h
//Author: Ivlev Alexander. Stef
//Created: 09/12/2016
//

#pragma once

#include "model/Wizard.h"
#include "model/Move.h"

#include "C_Singleton.h"
#include "R_Role.h"
#include "R_SkillBuild.h"
#include "S_StrategyType.h"
#include "R_TacticsRole.h"
#include "T_Message.h"
#include <unordered_map>

namespace AICup
{
  /// хранит и обновляет роль + скилл билд + стратегию + запускает тактику
  class Session: public Singleton<Session> {
  public:
    Session();

    void update(const model::Wizard& self, model::Move& move);

    inline const Role& getRole() const {
      return *role;
    }

    inline const SkillBuild& getSkillBuild() const {
      return *skillBuild;
    }

    inline const StrategyDTO& getStrategyDTO() const {
      return strategyDTO;
    }

  private:
    void initialize();
    void initializeMessageIndex();
    void initializeRoles();

    void tacticsSettingsUpdate(const model::Wizard& self, model::Move& move);

    RolePtr roleByRoleType(TacticsRole roleType);
    SkillBuildPtr skillBuildByRoleType(TacticsRole roleType);
    StrategyDTO strategyDTOByMessage(const Message& message);
    void sendMessages(const std::vector<Message>& messages, model::Move& move);

    /// для игры в одного - чтобы менять скилл билд в зависимости от ситуации на линии
    void dynamicChangeSkillBuild(const model::Wizard& self);


  private:
    bool isInitialized;

    RolePtr role;
    SkillBuildPtr skillBuild;
    StrategyDTO strategyDTO;

    /// для командрой игры, чтобы быстро понимать кто на какой роли
    std::unordered_map<long long, TacticsRole> roles;
    /// для командной игры - мапинг id игрока, в номер сообщения
    std::unordered_map<long long, int> messageIndex;
  };
};