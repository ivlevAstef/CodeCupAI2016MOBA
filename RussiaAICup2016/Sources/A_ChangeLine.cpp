#include "A_ChangeLine.h"
#include "E_InfluenceMap.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "E_Points.h"
#include "C_Extensions.h"

using namespace AICup;


double Algorithm::calculateLinePriority(const Algorithm::PathFinder& finder, const Wizard& self, const model::LaneType lane) {
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
  double priority = 1;
  switch (lane) {
    case model::LANE_TOP:
    case model::LANE_BOTTOM:
      baseLength = abs((basePosition - position).x) + abs((basePosition - position).y);
      baseReverseLength = 7200 - baseLength;
      break;
    case model::LANE_MIDDLE:
      baseLength = sqrt(2) * (basePosition - position).length();
      baseReverseLength = 7200 - baseLength;
      priority = 0.95;
      break;
    default:
      break;
  }


  /// на сколько перевес магов на линии
  double wizardCount = World::instance().wizardCount(lane, Game::friendFaction(), self);
  if (!self.getRole().getChangeLineWizardCountOnlyFriend()) {
    wizardCount -= World::instance().wizardCount(lane, Game::enemyFaction());
  }

  /// баланс башен положительный - наших больше, отрицательный наших меньше
  int towerBalance = World::instance().towerCount(lane, Game::friendFaction()) - World::instance().towerCount(lane, Game::enemyFaction());

  /// баланс по силам
  /// пачка крипов около 1500, башня около 4000
  const double laneStrength = InfluenceMap::instance().getLineStrength(lane);

  /// текущая линия может быть более приоритетной, если на ней есть тот кого в ближайшее время можно убить и получить опыт
  double expirience = 0;
  if (World::instance().positionToLine(self.getX(), self.getY()) == lane) {
    expirience = potensialExpirience(self);
  }


  /// Множители из роли
  const auto lengthRole = self.getRole().getChangeLineForeFrontPriority();
  const auto distanceRole = self.getRole().getChangeLinePathLengthPriority();
  const auto wizardRole = self.getRole().getChangeLineWizardCountPriority();
  const auto towerRole = self.getRole().getChangeLineTowerBalancePriority();
  const auto laneStrengthRole = self.getRole().getChangeLineLaneStrengthPriority();
  const auto expirienceRole = self.getRole().getChangeLineExpiriencePriority();

  /// все значения от 0 до 1000
  double lengthPriority = (baseReverseLength * baseReverseLength) / (14.4 * 3600);
  double distancePriority = 1000 - selfLength / 8;// будем предполагать что максимальная длина пути 8000
  double wizardPriority = wizardRole > 0 ? (500 - 100 * wizardCount) : (500 + 100 * wizardCount);
  double towerPriority = towerRole > 0 ? (500 - 250 * towerBalance) : (500 + 250 * towerBalance);
  double laneStrengthPriority = 500 - (MAX(-1000, MIN(laneStrength, 1000)) / 2);
  double expiriencePriority = MIN(1000, expirience / 500);

  return priority * (lengthPriority * lengthRole
    + distancePriority * distanceRole
    + wizardPriority * abs(wizardRole)
    + towerPriority * abs(towerRole)
    + laneStrengthPriority * laneStrengthRole
    + expiriencePriority * expirienceRole)
    / (lengthRole + distanceRole + abs(wizardRole) + abs(towerRole) + laneStrengthRole + expirienceRole);
}

bool equal(double a, double b) {
  return a - 1.0e-5 < b && b < a + 1.0e-5;
}


bool Algorithm::checkChangeLine(const Algorithm::PathFinder& finder, const Wizard& self, model::LaneType& lane) {
  /// значение от 0 до 1000
  auto topPriority = calculateLinePriority(finder, self, model::LANE_TOP);
  auto middlePriority = calculateLinePriority(finder, self, model::LANE_MIDDLE);
  auto bottomPriority = calculateLinePriority(finder, self, model::LANE_BOTTOM);


  const auto minPriority = MIN(topPriority, MIN(middlePriority, bottomPriority));
  const auto maxPriority = MAX(topPriority, MAX(middlePriority, bottomPriority));

  // не меняем линию если нет большого перевеса
  if (maxPriority - minPriority < 1000 * (1 - self.getRole().getDesireChangeLine())) {
    return false;
  }

  if (equal(topPriority, maxPriority)) {
    lane = model::LANE_TOP;
  } else if (equal(middlePriority, maxPriority)) {
    lane = model::LANE_MIDDLE;
  } else {
    lane = model::LANE_BOTTOM;
  }

  return true;
}

double Algorithm::potensialExpirience(const Wizard& self) {
  double result = 0;
  for (const auto& enemy : World::instance().aroundEnemies(self, self.getVisionRange() + 150)) {
    double lifeMult = 1.0 - (double(enemy->getLife()) / double(enemy->getMaxLife()));
    lifeMult *= lifeMult; // квадратичная зависимость

    if (EX::isBuilding(*enemy)) {
      result += enemy->getMaxLife() * 0.5f * lifeMult;
    } if (EX::isMinion(*enemy)) {
      result += enemy->getMaxLife() * 0.25f * lifeMult;
    } else if (EX::isWizard(*enemy)) {
      double distance = enemy->getDistanceTo(self);
      double distanceMult = (1.0 - distance / self.getCastRange());
      distanceMult = MAX(distanceMult, 0);
      distanceMult *= distanceMult; // квадратичная зависимость

      result += enemy->getMaxLife() * lifeMult * distanceMult;
    }
  }

  return result;
}