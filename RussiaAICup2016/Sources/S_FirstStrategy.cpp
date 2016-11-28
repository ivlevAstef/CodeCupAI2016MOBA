#include "S_FirstStrategy.h"
#include "E_World.h"
#include "E_Points.h"
#include "E_InfluenceMap.h"
#include "C_Math.h"
#include "C_Extensions.h"

using namespace AICup;

FirstStrategy::FirstStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder) :
  CommandStrategy(fabric, pathFinder) {
  isInitialized = false;
  lastChangeLineTick = 0;
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

  /// раз в 250 тиков пересматриваю линию,
  /// 250 так как у нас бонусы появляются на кратных секундах, а значит взятие бонуса может привести к смене линии
  if (World::model().getTickIndex() - lastChangeLineTick >= 250) {
    changeLane(self);
    lastChangeLineTick = World::model().getTickIndex();
  }

  if (!isInitialized) {
    init(self);
    isInitialized = true;
  }

  const auto avoidAroundCommands = calcAllAroundEnemies(self);
  if (!avoidAroundCommands.empty()) {
    moveCommands.insert(moveCommands.end(), avoidAroundCommands.begin(), avoidAroundCommands.end());
  }

  ///

  if (nullptr == moveToBonus.get()) {
    const auto newMoveToBonus = fabric.moveToBonus();
    if (newMoveToBonus->check(self)) {
      moveToBonus = newMoveToBonus;
      moveCommands.push_back(moveToBonus);
    }
  } else if (moveToBonus->check(self)) {
    moveCommands.push_back(moveToBonus);
  } else {
    moveToBonus = nullptr;
  }


  if (nullptr == moveToBonus) {
    const auto moveToLineCommand = fabric.moveToLine(myLine);
    if (moveToLineCommand->check(self)) {
      moveCommands.push_back(moveToLineCommand);
    }

    const auto getExpirienceCommand = fabric.moveGetExpirience();
    if (getExpirienceCommand->check(self)) {
      moveCommands.push_back(getExpirienceCommand);
    }
  }

  for (const auto& enemy : World::instance().aroundEnemies(self)) {
    const auto attackCommand = fabric.attack(*enemy);
    if (nullptr != attackCommand && attackCommand->check(self)) {
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

const std::vector<MoveCommandPtr> FirstStrategy::calcAllAroundEnemies(const Wizard& self) {
  /// гдето тут надо будет началь указывать что эти команды могуть иметь больше/меньше приоритет в зависимости от ситуации на карте
  /// возможно avoidAround обернуть еще в одну ком анду
  const auto selfPos = EX::pos(self);

  const auto aroundEnemies = World::instance().aroundEnemies(self, self.getVisionRange() + self.getRadius() * 2);

  std::vector<MoveCommandPtr> avoidAroundCommands;
  avoidAroundCommands.reserve(aroundEnemies.size());
  for (const auto& enemy : aroundEnemies) {
    const auto command = fabric.avoidEnemy(enemy->getId());
    if (command->check(self)) {
      avoidAroundCommands.push_back(command);
    }
  }


  for (const auto& projectile : World::model().getProjectiles()) {
    const auto command = fabric.avoidEnemy(projectile.getId());
    if (command->check(self)) {
      avoidAroundCommands.push_back(command);
    }
  }

  return avoidAroundCommands;
}

void FirstStrategy::changeLane(const Wizard& self) {
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
  double selfBottomLength = path->getLength();

  double topLength = abs((basePosition - topPosition).x) + abs((basePosition - topPosition).y);
  double middleLength = (basePosition - middlePosition).length();
  double bottomLength = abs((basePosition - bottomPosition).x) + abs((basePosition - bottomPosition).y);


  double priorityTop = (8000 - topLength) - selfTopLength;
  double priorityMiddle = (5657 - middleLength) - selfMiddleLength;
  double priorityBottom = (8000 - bottomLength) - selfBottomLength;
  /// если очень близко к базе то увеличиваем
  const double cTop = (1600 - topLength) / 250.0;
  const double cMiddle = (1600 - middleLength) / 250.0;
  const double cBottom = (1600 - bottomLength) / 250.0;
  priorityTop *= (topLength < 1600) ? (1 + cTop*cTop) : 1;
  priorityMiddle *= (middleLength < 1600) ? (1 + cMiddle*cMiddle) : 1;
  priorityBottom *= (bottomLength < 1600) ? (1 + cBottom*cBottom) : 1;

  // Если много своих то уменьшаем
  priorityTop /= MAX(0.75, sqrt(World::instance().wizardCount(model::LANE_TOP, self)));
  priorityMiddle /= MAX(0.75, sqrt(World::instance().wizardCount(model::LANE_MIDDLE, self)));
  priorityBottom /= MAX(0.75, sqrt(World::instance().wizardCount(model::LANE_BOTTOM, self)));

  const auto minPriority = MIN(priorityTop, MIN(priorityMiddle, priorityBottom));
  const auto maxPriority = MAX(priorityTop, MAX(priorityMiddle, priorityBottom));
  if (maxPriority - minPriority < 1000) { // не меняем линию если нет большого перевеса
    return;
  }

  if (priorityTop > priorityMiddle && priorityTop > priorityBottom) {
    myLine = model::LANE_TOP;
  } else if (priorityMiddle > priorityTop && priorityMiddle > priorityBottom) {
    myLine = model::LANE_MIDDLE;
  } else {
    myLine = model::LANE_BOTTOM;
  }
}