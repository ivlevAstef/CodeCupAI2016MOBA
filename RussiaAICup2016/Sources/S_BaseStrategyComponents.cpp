#include "S_BaseStrategyComponents.h"
#include "E_World.h"
#include "E_Points.h"
#include "E_InfluenceMap.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "A_ChangeLine.h"

using namespace AICup;

BaseStrategyComponents::BaseStrategyComponents(const CommandFabric& fabric, RolePtr role, SkillBuildPtr skillBuild):
  CommandStrategy(fabric, role, skillBuild) {
  currentLane = model::LANE_MIDDLE;
  lastChangeLineTick = 0;
}

bool BaseStrategyComponents::changeLane(const Wizard& self) {
  return Algorithm::checkChangeLine(Algorithm::PathFinder::instance(), self, currentLane);
}

bool BaseStrategyComponents::changeLane(const Wizard& self, int period) {
  if (World::model().getTickIndex() - lastChangeLineTick >= period) {
    lastChangeLineTick = World::model().getTickIndex();
    return Algorithm::checkChangeLine(Algorithm::PathFinder::instance(), self, currentLane);
  }

  return false;
}

void BaseStrategyComponents::addAroundEnemiesOrMoveMelee(const Wizard& self) {
  const auto selfPos = EX::pos(self);

  const auto aroundEnemies = World::instance().aroundEnemies(self, self.getVisionRange() + 100);
  for (const auto& enemy : aroundEnemies) {
    const auto avoidCommand = fabric.avoidEnemy(*enemy);
    const auto moveMeleeCommand = fabric.moveMeleeAttack(*enemy);

    const bool avoid = avoidCommand->check(self);
    const bool moveMelee = moveMeleeCommand->check(self);

    if (avoid && !moveMelee) {
      moveCommands.push_back(avoidCommand);
    } else if (moveMelee) {
      moveCommands.push_back(moveMeleeCommand);
    }
  }
}

void BaseStrategyComponents::addAttackFollow(const Wizard& self) {
  for (const auto& enemy : World::instance().aroundEnemies(self, self.getVisionRange() + 100)) {
    if (EX::isWizard(*enemy)) {
      const auto& wizard = EX::asWizard(*enemy);
      const auto command = fabric.followAttack(wizard);
      if (command->check(self)) {
        moveCommands.push_back(command);
      }
    }
  }
}

void BaseStrategyComponents::addMoveTo(const Wizard& self, model::LaneType lane) {
  MoveCommand::Result cache;

  /// эмуляция машины состояний... кривая до невозможности

  double moveToBonusPriority = -1000;
  const auto moveToBonus = fabric.moveToBonus();
  if (moveToBonus->check(self)) {
    moveToBonus->execute(self, cache);
    moveToBonusPriority = cache.priority;
  }


  double moveToLinePriority = -1000;
  const auto moveToLine = fabric.moveToLine(lane);
  if (moveToLine->check(self)) {
    moveToLine->execute(self, cache);
    moveToLinePriority = cache.priority;
  }

  if (moveToLinePriority < 0 && moveToBonusPriority < 0) {
    return;
  }


  if (moveToLinePriority >= moveToBonusPriority) {
    moveCommands.push_back(moveToLine);
  } else {
    moveCommands.push_back(moveToBonus);
  }
}

void BaseStrategyComponents::addAttacks(const Wizard& self) {
  for (const auto& enemy : World::instance().aroundEnemies(self, self.getVisionRange() + 200)) {
    const auto attackCommand = fabric.attack(*enemy);
    if (nullptr != attackCommand && attackCommand->check(self)) {
      attackCommands.push_back(attackCommand);
    }
  }

  const auto attackFrostboltCommand = fabric.attackUseFrostbolt();
  if (nullptr != attackFrostboltCommand && attackFrostboltCommand->check(self)) {
    attackCommands.push_back(attackFrostboltCommand);
  }

  const auto attackFireballCommand = fabric.attackUseFireball();
  if (nullptr != attackFireballCommand && attackFireballCommand->check(self)) {
    attackCommands.push_back(attackFireballCommand);
  }

}
void BaseStrategyComponents::addCasts(const Wizard& self) {
  const auto castHasteCommand = fabric.haste();
  if (nullptr != castHasteCommand && castHasteCommand->check(self)) {
    castCommands.push_back(castHasteCommand);
  }

  const auto castShieldCommand = fabric.shield();
  if (nullptr != castShieldCommand && castShieldCommand->check(self)) {
    castCommands.push_back(castShieldCommand);
  }
}