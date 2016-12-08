#include "S_FirstStrategy.h"
#include "E_World.h"
#include "E_Points.h"
#include "E_InfluenceMap.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "A_ChangeLine.h"

using namespace AICup;

FirstStrategy::FirstStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder) :
  CommandStrategy(fabric, pathFinder) {
  isInitialized = false;
  lastChangeLineTick = 0;
  myLine = model::LANE_MIDDLE;
}

void FirstStrategy::init(const Wizard& self) {
  // вначале линию выбираем также как в smart game, так как это даст скорей всего наименьшее количество коллизий
  switch ((int)self.getId()) {
    case 1:
    case 2:
    case 6:
    case 7:
      myLine = model::LANE_TOP;
      break;
    case 3:
    case 8:
      myLine = model::LANE_MIDDLE;
      break;
    case 4:
    case 5:
    case 9:
    case 10:
      myLine = model::LANE_BOTTOM;
      break;
    default:
      myLine = model::LANE_MIDDLE;
  }
}

void FirstStrategy::update(const Wizard& self, model::Move& move) {
  CommandStrategy::clear();

  if (!isInitialized) {
    init(self);
    isInitialized = true;
  }

  checkAndChangeLane(self);

  ///////////////////////////////////

  addAroundEnemies(self);
  addAttackFollow(self);


  ///////////////////////////////////

  addMoveTo(self);

  const auto getExpirienceCommand = fabric.moveGetExpirience();
  if (getExpirienceCommand->check(self)) {
    moveCommands.push_back(getExpirienceCommand);
  }

  ///////////////////////////////////////

  addAttacks(self);
  addCasts(self);

  CommandStrategy::update(self, move);
}

void FirstStrategy::checkAndChangeLane(const Wizard& self) {
  /// раз в 500 тиков пересматриваю линию,
  /// 500 так как у нас бонусы появляются на кратных секундах, а значит взятие бонуса может привести к смене линии
  if (World::model().getTickIndex() - lastChangeLineTick >= 500 ||
    /// также проверяем вначале каждый тик, чтобы поудачней выбрать линию
    (200 <= World::model().getTickIndex() && World::model().getTickIndex() <= 750)) {
    model::LaneType lane;
    if (Algorithm::checkChangeLine(pathFinder, self, lane)) {
      myLine = lane;
    }
    lastChangeLineTick = World::model().getTickIndex();
  }
}

void FirstStrategy::addAroundEnemies(const Wizard& self) {
  const auto selfPos = EX::pos(self);

  const auto aroundEnemies = World::instance().aroundEnemies(self, self.getVisionRange() + 100);
  for (const auto& enemy : aroundEnemies) {
    const auto command = fabric.avoidEnemy(*enemy);
    if (command->check(self)) {
      moveCommands.push_back(command);
    }
  }
}

void FirstStrategy::addAttackFollow(const Wizard& self) {
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

void FirstStrategy::addMoveTo(const Wizard& self) {
  MoveCommand::Result cache;

  /// эмуляция машины состояний... кривая до невозможности

  double moveToBonusPriority = -1000;
  if (nullptr == moveToBonus) {
    moveToBonus = fabric.moveToBonus();
  }

  if (moveToBonus->check(self)) {
    moveToBonus->execute(self, cache);
    moveToBonusPriority = cache.priority;
  } else {
    moveToBonus = nullptr;
  }


  double moveToLinePriority = -1000;
  const auto moveToLine = fabric.moveToLine(myLine);
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

void FirstStrategy::addAttacks(const Wizard& self) {
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
void FirstStrategy::addCasts(const Wizard& self) {
  const auto castHasteCommand = fabric.haste();
  if (nullptr != castHasteCommand && castHasteCommand->check(self)) {
    castCommands.push_back(castHasteCommand);
  }

  const auto castShieldCommand = fabric.shield();
  if (nullptr != castShieldCommand && castShieldCommand->check(self)) {
    castCommands.push_back(castShieldCommand);
  }
}