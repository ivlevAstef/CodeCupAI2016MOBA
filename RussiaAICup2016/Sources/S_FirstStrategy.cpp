#include "S_FirstStrategy.h"
#include "E_World.h"
#include "E_Points.h"
#include "E_InfluenceMap.h"
#include "C_Math.h"

using namespace AICup;

FirstStrategy::FirstStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder) :
  CommandStrategy(fabric, pathFinder) {
  isInitialized = false;
  lastChangeLineTick = 0;
}

void FirstStrategy::init(const model::Wizard& self, model::Move& move) {
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

void FirstStrategy::update(const model::Wizard& self, model::Move& move) {
  CommandStrategy::clear();

  /// раз в 500 тиков пересматриваю линию,
  /// 500 так как у нас бонусы появляются на кратных секундах, а значит взятие бонуса может привести к смене линии
  if (World::model().getTickIndex() - lastChangeLineTick >= 500) {
    changeLane(self);
    lastChangeLineTick = World::model().getTickIndex();
  }

  if (!isInitialized) {
    init(self, move);
    isInitialized = true;
  }

  const auto moveToLineCommand = fabric.moveToLine(myLine);
  if (moveToLineCommand->check(self)) {
    moveCommands.push_back(moveToLineCommand);
  }

  const auto avoidAroundCommand = fabric.avoidAround();
  bool needAvoidAround = avoidAroundCommand->check(self);

  if (needAvoidAround && self.getLife() < 50) {
    moveCommands.push_back(avoidAroundCommand);
  } else {
    const auto getExpirienceCommand = fabric.moveGetExpirience();
    if (getExpirienceCommand->check(self)) {
      moveCommands.push_back(getExpirienceCommand);
    } else if (needAvoidAround) {
      moveCommands.push_back(avoidAroundCommand);
    }
  }

  ///

  if (nullptr == moveToBonus.get()) {
    const auto newMoveToBonus = fabric.moveToBonus();
    if (newMoveToBonus->check(self)) {
      moveToBonus = newMoveToBonus;
    }
  } else if (moveToBonus->check(self)) {
    moveCommands.push_back(moveToBonus);
  } else {
    moveToBonus = nullptr;
  }

  for (const auto& enemy : World::instance().aroundEnemies(self)) {
    const auto attackCommand = fabric.attack(enemy->getId());
    if (attackCommand->check(self)) {
      attackCommands.push_back(attackCommand);
    }
  }


  /*
  как должно быть:
  1)!! пока нету крипов, иследуем карту и бъем нейтралов, для получения опыта и очков
  3)!! при моменте появления руны (надо еще написать эту команду), идем к ней в двух случаях:
     а) это выгодно по xp (руна близко, и тут много не заработаешь)
     б) идем на ту линию где ситуация не очень
     с) идем на ту линию где своих меньше (дабы побольше оттяпать)
 */

  CommandStrategy::update(self, move);
}

void FirstStrategy::changeLane(const model::Wizard& self) {
  const auto selfPos = Position(self.getX(), self.getY());

  const auto basePosition = Points::point(Points::ACADEMY_BASE);
  const auto topPosition = InfluenceMap::instance().getForeFront(model::LANE_TOP, 0);
  const auto middlePosition = InfluenceMap::instance().getForeFront(model::LANE_MIDDLE, 0);
  const auto bottomPosition = InfluenceMap::instance().getForeFront(model::LANE_BOTTOM, 0);

  std::shared_ptr<Algorithm::Path> path;

  pathFinder.calculatePath(topPosition, path);
  double selfTopLength = path->getLength();
  pathFinder.calculatePath(middlePosition, path);
  double selfMiddleLength = path->getLength();
  pathFinder.calculatePath(bottomPosition, path);
  double selfTBottomLength = path->getLength();

  double topLength = abs((basePosition - topPosition).x) + abs((basePosition - topPosition).y);
  double middleLength = (basePosition - middlePosition).length();
  double bottomLength = abs((basePosition - bottomPosition).x) + abs((basePosition - bottomPosition).y);


  double priorityTop = (8000 - topLength) / selfTopLength;
  priorityTop *= (topLength < 1200) ? (1200 - topLength) : 1;
  double priorityMiddle = (6000 - middleLength) / selfMiddleLength;
  priorityMiddle *= (middleLength < 1200) ? (1200 - middleLength) : 1;
  double priorityBottom = (8000 - bottomLength) / selfTBottomLength;
  priorityBottom *= (bottomLength < 1200) ? (1200 - bottomLength) : 1;

  priorityTop /= MAX(0.5, World::instance().wizardCount(model::LANE_TOP, self));
  priorityMiddle /= MAX(0.5, World::instance().wizardCount(model::LANE_MIDDLE, self));
  priorityBottom /= MAX(0.5, World::instance().wizardCount(model::LANE_BOTTOM, self));


  if (priorityTop > priorityMiddle && priorityTop > priorityBottom) {
    myLine = model::LANE_TOP;
  } else if (priorityMiddle > priorityTop && priorityMiddle > priorityBottom) {
    myLine = model::LANE_MIDDLE;
  } else {
    myLine = model::LANE_BOTTOM;
  }
}