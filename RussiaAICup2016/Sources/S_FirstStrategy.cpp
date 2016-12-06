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

  /// раз в 500 тиков пересматриваю линию,
  /// 500 так как у нас бонусы появляются на кратных секундах, а значит взятие бонуса может привести к смене линии
  if (World::model().getTickIndex() - lastChangeLineTick >= 500) {
    model::LaneType lane;
    if (Algorithm::checkChangeLine(pathFinder, self, lane)) {
      myLine = lane;
    }
    lastChangeLineTick = World::model().getTickIndex();
  }

  const auto avoidAroundCommands = calcAllAroundEnemies(self);
  if (!avoidAroundCommands.empty()) {
    moveCommands.insert(moveCommands.end(), avoidAroundCommands.begin(), avoidAroundCommands.end());
  }

  /*const auto pushOffMinionsCommands = calcAllPushOffMinions(self);
  if (!pushOffMinionsCommands.empty()) {
    moveCommands.insert(moveCommands.end(), pushOffMinionsCommands.begin(), pushOffMinionsCommands.end());
  }*/

  /// бежать чтобы добить
  for (const auto& enemy : World::instance().aroundEnemies(self, self.getVisionRange() + 100)) {
    if (EX::isWizard(*enemy)) {
      const auto& wizard = EX::asWizard(*enemy);
      const auto command = fabric.followAttack(wizard);
      if (command->check(self)) {
        moveCommands.push_back(command);
      }
    }
  }

  ///////

  if (nullptr == moveToBonus.get()) {
    const auto newMoveToBonus = fabric.moveToBonus();
    if (newMoveToBonus->check(self)) {
      moveToBonus = newMoveToBonus;
    }
  } else if (!moveToBonus->check(self)) {
    moveToBonus = nullptr;
  }

  bool needMoveToBonus = true;
  const auto moveToLineCommand = fabric.moveToLine(myLine);
  if (moveToLineCommand->check(self)) {
    if (nullptr == moveToBonus || moveToLineCommand->priority(self) > moveToBonus->priority(self)) {
      moveCommands.push_back(moveToLineCommand);
      needMoveToBonus = false;
    }
  }


  if (nullptr != moveToBonus && needMoveToBonus) {
    moveCommands.push_back(moveToBonus);
  }

  const auto getExpirienceCommand = fabric.moveGetExpirience();
  if (getExpirienceCommand->check(self)) {
    moveCommands.push_back(getExpirienceCommand);
  }

  ////

  for (const auto& enemy : World::instance().aroundEnemies(self, self.getVisionRange() + 100)) {
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


  const auto castHasteCommand = fabric.haste();
  if (nullptr != castHasteCommand && castHasteCommand->check(self)) {
    castCommands.push_back(castHasteCommand);
  }

  const auto castShieldCommand = fabric.shield();
  if (nullptr != castShieldCommand && castShieldCommand->check(self)) {
    castCommands.push_back(castShieldCommand);
  }

  CommandStrategy::update(self, move);
}

const std::vector<MoveCommandPtr> FirstStrategy::calcAllAroundEnemies(const Wizard& self) {
  const auto selfPos = EX::pos(self);

  const auto aroundEnemies = World::instance().aroundEnemies(self, self.getVisionRange() + 100);

  std::vector<MoveCommandPtr> avoidAroundCommands;
  avoidAroundCommands.reserve(aroundEnemies.size());
  for (const auto& enemy : aroundEnemies) {
    const auto command = fabric.avoidEnemy(*enemy);
    if (command->check(self)) {
      avoidAroundCommands.push_back(command);
    }
  }


  for (const auto& projectile : World::model().getProjectiles()) {
    if (projectile.getFaction() == self.getFaction()) {
      continue;
    }
    const auto command = fabric.avoidProjectile(projectile);
    if (command->check(self)) {
      avoidAroundCommands.push_back(command);
    }
  }

  return avoidAroundCommands;
}

const std::vector<MoveCommandPtr> FirstStrategy::calcAllPushOffMinions(const Wizard& self) {
  const auto selfPos = EX::pos(self);

  const auto aroundUnits = World::instance().around(self, self.getFaction(), 2 * self.getRadius() + 100);

  std::vector<MoveCommandPtr> pushOffCommands;
  pushOffCommands.reserve(aroundUnits.size());
  for (const auto& unit : aroundUnits) {
    if (!EX::isMinion(*unit)) {
      continue;
    }

    const auto& minion = EX::asMinion(*unit);
    const auto command = fabric.pushOffMinion(minion);
    if (command->check(self)) {
      pushOffCommands.push_back(command);
    }
  }

  return pushOffCommands;
}