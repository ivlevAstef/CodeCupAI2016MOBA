#include "S_BaseStrategy.h"
#include "E_World.h"
#include "E_Points.h"
#include "E_InfluenceMap.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "A_ChangeLine.h"

using namespace AICup;

BaseStrategy::BaseStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder):
  CommandStrategy(fabric, pathFinder) {
}

void BaseStrategy::update(const Wizard& self, model::Move& move) {
  CommandStrategy::clear();

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

model::LaneType BaseStrategy::checkAndChangeLane(const Wizard& self) {
  static int lastChangeLineTick = 0;
  static model::LaneType lane = model::LANE_MIDDLE;

  /// раз в 250 тиков пересматриваю линию,
  /// 250 так как у нас бонусы появляются на кратных секундах, а значит взятие бонуса может привести к смене линии
  if (World::model().getTickIndex() - lastChangeLineTick >= 250 ||
    /// также проверяем вначале каждый тик, чтобы поудачней выбрать линию
    (100 <= World::model().getTickIndex() && World::model().getTickIndex() <= 500)) {

    if (Algorithm::checkChangeLine(pathFinder, self, lane)) {
      lastChangeLineTick = World::model().getTickIndex();
    }
  }

  return lane;
}

void BaseStrategy::addAroundEnemiesOrMoveMelee(const Wizard& self) {
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

void BaseStrategy::addAttackFollow(const Wizard& self) {
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

void BaseStrategy::addMoveTo(const Wizard& self, model::LaneType lane) {
  MoveCommand::Result cache;

  /// эмуляция машины состояний... кривая до невозможности

  double moveToBonusPriority = -1000;
  const auto moveToBonus = fabric.moveToBonus();
  if (moveToBonus->check(self)) {
    moveToBonus->execute(self, cache);
    moveToBonusPriority = cache.priority;

    for (const auto& bonus : World::model().getBonuses()) {
      if (self.getDistanceTo(bonus) < 350) {
        moveToBonusPriority = 10000;
      }
    }
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

void BaseStrategy::addAttacks(const Wizard& self) {
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
void BaseStrategy::addCasts(const Wizard& self) {
  const auto castHasteCommand = fabric.haste();
  if (nullptr != castHasteCommand && castHasteCommand->check(self)) {
    castCommands.push_back(castHasteCommand);
  }

  const auto castShieldCommand = fabric.shield();
  if (nullptr != castShieldCommand && castShieldCommand->check(self)) {
    castCommands.push_back(castShieldCommand);
  }
}