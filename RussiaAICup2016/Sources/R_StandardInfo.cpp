#include "R_StandardInfo.h"

#include "E_World.h"
#include "E_Game.h"

#include "R_Tanks.h"
#include "R_Supports.h"
#include "R_Attacks.h"
#include <assert.h>
#include <algorithm>

using namespace AICup;

StandardInfo::StandardInfo() {
  initializeRoles();
}


RolePtr roleByRoleType(TacticsRole roleType) {
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

SkillBuildPtr skillBuildByRoleType(TacticsRole roleType) {
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

TacticsRole StandardInfo::tacticsRole(const model::Wizard& self) {
  assert(0 != instance().roles.count(self.getId()));
  return instance().roles[self.getId()];
}

RolePtr StandardInfo::role(const model::Wizard& self) {
  return roleByRoleType(tacticsRole(self));
}

SkillBuildPtr StandardInfo::skillBuild(const model::Wizard& self) {
  return skillBuildByRoleType(tacticsRole(self));
}

void StandardInfo::initializeRoles() {
  const static TacticsRole allRoles[4] = {
    TacticsRole::SupportStan,
    TacticsRole::AttackAOE,
    TacticsRole::SupportHaste,
    TacticsRole::AttackStan
  };

  size_t roleIndex = 0;

  std::vector<model::Wizard> wizards;

  for (const auto& wizard : World::model().getWizards()) {
    if (wizard.getFaction() == Game::friendFaction()) {
      wizards.push_back(wizard);
    }
  }

  std::sort(wizards.begin(), wizards.end(), [] (const model::Wizard& w1, const model::Wizard& w2) {
    return w1.getId() < w2.getId();
  });

  for (const auto& wizard : wizards) {
    if (wizard.isMaster()) {
      //TODO: потом или random(), или какнибудь по ситуации учиться до пятого уровня определять что лучше
      roles[wizard.getId()] = TacticsRole::TankStan;
      continue;
    }

    roles[wizard.getId()] = allRoles[roleIndex++];
  }
}

