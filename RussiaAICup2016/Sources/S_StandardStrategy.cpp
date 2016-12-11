#include "S_StandardStrategy.h"

#include "E_World.h"

#include "R_Tanks.h"
#include "R_Supports.h"
#include "R_Attacks.h"

using namespace AICup;

StandardStrategy::StandardStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder) :
  /// потом переопределяються
  BaseStrategyComponents(fabric, pathFinder, std::make_shared<TankAttackRole>(), std::make_shared<TankAttackSkillBuild>()) {

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


void StandardStrategy::update(const model::Wizard& model, model::Move& move) {
  role = roleByRoleType(roles[model.getId()]);
  skillBuild = skillBuildByRoleType(roles[model.getId()]);

  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model, move);

  const auto lane = checkAndChangeLane(self);

  ///////////////////////////////////

  addAroundEnemiesOrMoveMelee(self);
  addAttackFollow(self);


  ///////////////////////////////////

  addMoveTo(self, lane);

  const auto getExpirienceCommand = fabric.moveGetExpirience();
  if (getExpirienceCommand->check(self)) {
    moveCommands.push_back(getExpirienceCommand);
  }

  ///////////////////////////////////////

  addAttacks(self);
  addCasts(self);

  CommandStrategy::update(self, move);
}


void StandardStrategy::initializeRoles() {
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

/*
RolePtr roleByRoleType(TacticsRole roleType);
SkillBuildPtr skillBuildByRoleType(TacticsRole roleType);
*/