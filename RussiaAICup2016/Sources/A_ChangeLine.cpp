#include "A_ChangeLine.h"
#include "E_InfluenceMap.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "E_Points.h"

using namespace AICup;


double Algorithm::calculateLinePrioirty(const Algorithm::PathFinder& finder, const Wizard& self, const model::LaneType lane) {
  const auto basePosition = Points::point(Points::ACADEMY_BASE);
  /// �������� ����� ��� ������ ���������� �����
  const auto position = InfluenceMap::instance().getForeFront(lane, 0);

  /// ������� ����� ������� ������ �� ����� �� ������� �����
  std::shared_ptr<Algorithm::Path> path;
  finder.calculatePath(position, path);
  double selfLength = path->getRealLength();

  /// ����������� ��� ������ ����� ���������� �� ����, � ����������� ��� ��������
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


  /// ������� ����� ����� �� �����
  double wizardCount = World::instance().wizardCount(lane, self);

  /// ������ ����� ������������� - ����� ������, ������������� ����� ������
  int towerBalance = World::instance().towerCount(lane, Game::friendFaction()) - World::instance().towerCount(lane, Game::enemyFaction());

  /// ������ �� �����
  /// ����� ������ ����� 1500, ����� ����� 4000
  double laneStrength = InfluenceMap::instance().getLineStrength(lane);

  /// ��� �������� �� 0 �� 1000
  double lengthPriority = (baseReverseLength * baseReverseLength) / (14.4 * 3600);
  double distancePriority = 1000 - selfLength / 8;// ����� ������������ ��� ������������ ����� ���� 8000
  double wizardPriority = 1000 - 200 * wizardCount;
  double towerPriority = 500 - 250 * towerBalance;// ���� ����� ����� ��� �� ��������� 1000
  double laneStrengthPriority = 500 - (MAX(-10000, MIN(laneStrength, 10000)) / 20);

  if (laneStrength > 3000 && 0 == World::instance().towerCount(lane, Game::enemyFaction()) && baseReverseLength < 1200) {
    return 10000; /// ���� �� �� ����� ��� ����� ������ ����
  }

  return lengthPriority
    + distancePriority * self.getRole().getChangeLinePathLengthPriority()
    + wizardPriority * self.getRole().getChangeLineWizardCountPriority()
    + towerPriority * self.getRole().getChangeLineTowerBalancePriority()
    + laneStrengthPriority *  self.getRole().getChangeLineLaneStrengthPriority();
}

bool Algorithm::checkChangeLine(const Algorithm::PathFinder& finder, const Wizard& self, model::LaneType& lane) {
  /// �������� �� 0 �� 1000
  auto topPriority = calculateLinePrioirty(finder, self, model::LANE_TOP) / 5;
  auto middlePriority = calculateLinePrioirty(finder, self, model::LANE_MIDDLE) / 5;
  auto bottomPriority = calculateLinePrioirty(finder, self, model::LANE_BOTTOM) / 5;

  const auto minPriority = MIN(topPriority, MIN(middlePriority, bottomPriority));
  const auto maxPriority = MAX(topPriority, MAX(middlePriority, bottomPriority));

  // �� ������ ����� ���� ��� �������� ��������
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