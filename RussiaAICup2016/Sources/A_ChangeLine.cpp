#include "A_ChangeLine.h"
#include "E_InfluenceMap.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "E_Points.h"

using namespace AICup;


double Algorithm::calculateLinePrioirty(const Algorithm::PathFinder& finder, const Wizard& self, const model::LaneType lane) {
  const auto basePosition = Points::point(Points::ACADEMY_BASE);
  /// получаем точки где сейчас находяться линии
  const auto position = InfluenceMap::instance().getForeFront(lane, 0);

  /// Считаем время сколько бежать до линии от текущей точки
  std::shared_ptr<Algorithm::Path> path;
  finder.calculatePath(position, path);
  double selfLength = path->getRealLength();

  /// расчитываем как далеко линии находиться от базы, и ревертируем это значение
  double baseLength = 0;
  double baseReverseLength = 0;
  switch (lane) {
    case model::LANE_TOP:
    case model::LANE_BOTTOM:
      baseLength = abs((basePosition - position).x) + abs((basePosition - position).y);
      baseReverseLength = 7200 - baseLength;
      break;
    case model::LANE_MIDDLE:
      baseLength = sqrt(2) * (basePosition - position).length();
      baseReverseLength = 7200 - baseLength;
      break;
    default:
      break;
  }


  /// сколько своих магов на линии
  double wizardCount = World::instance().wizardCount(lane, self);

  /// баланс башен положительный - наших больше, отрицательный наших меньше
  int towerBalance = World::instance().towerCount(lane, Game::friendFaction()) - World::instance().towerCount(lane, Game::enemyFaction());

  /// баланс по силам
  /// пачка крипов около 1500, башня около 4000
  double laneStrength = InfluenceMap::instance().getLineStrength(lane);

  /// все значения от 0 до 1000
  double lengthPriority = (baseReverseLength * baseReverseLength) / (14.4 * 3600);
  double distancePriority = 1000 - selfLength / 8;// будем предполагать что максимальная длина пути 8000
  double wizardPriority = 1000 - 200 * wizardCount;
  double towerPriority = 500 - 250 * towerBalance;// если своих башен нет то приоритет 1000
  double laneStrengthPriority = 500 - (MAX(-10000, MIN(laneStrength, 10000)) / 20);

  if (laneStrength > 3000 && 0 == World::instance().towerCount(lane, Game::enemyFaction()) && baseReverseLength < 1200) {
    return 10000; /// идем на ту линию где скоро снесут базу
  }

  return lengthPriority
    + distancePriority * self.getRole().getChangeLinePathLengthPriority()
    + wizardPriority * self.getRole().getChangeLineWizardCountPriority()
    + towerPriority * self.getRole().getChangeLineTowerBalancePriority()
    + laneStrengthPriority *  self.getRole().getChangeLineLaneStrengthPriority();
}

bool Algorithm::checkChangeLine(const Algorithm::PathFinder& finder, const Wizard& self, model::LaneType& lane) {
  /// значение от 0 до 1000
  auto topPriority = calculateLinePrioirty(finder, self, model::LANE_TOP) / 5;
  auto middlePriority = calculateLinePrioirty(finder, self, model::LANE_MIDDLE) / 5;
  auto bottomPriority = calculateLinePrioirty(finder, self, model::LANE_BOTTOM) / 5;

  const auto minPriority = MIN(topPriority, MIN(middlePriority, bottomPriority));
  const auto maxPriority = MAX(topPriority, MAX(middlePriority, bottomPriority));

  // не меняем линию если нет большого перевеса
  if (maxPriority - minPriority < 1000 * (1 - self.getRole().getDesireChangeLine())) {
    return false;
  }

  if (topPriority > middlePriority && topPriority > bottomPriority) {
    lane = model::LANE_TOP;
  } else if (middlePriority > topPriority && middlePriority > bottomPriority) {
    lane = model::LANE_MIDDLE;
  } else {
    lane = model::LANE_BOTTOM;
  }

  return true;
}