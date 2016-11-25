//
//File: CM_CommandMoveToBonus.cpp
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#include "CM_CommandMoveToBonus.h"
#include "CM_CommandMoveToPoint.h"
#include "CM_CommandKeepDistance.h"
#include "E_World.h"
#include "E_Points.h"
#include "E_Game.h"
#include "C_Math.h"
#include "CM_MovePriorities.h"

using namespace AICup;

CommandMoveToBonus::CommandMoveToBonus(Algorithm::PathFinder& finder):
  MoveCommand(finder) {

}

bool CommandMoveToBonus::check(const model::Wizard& self) {
  static const double magicCoef = 0.95 ; /// ��������������� ���������� �� ������� ������� ����
  const auto selfPos = Position(self.getX(), self.getY());

  /// ���� ���� ������ �����
  for (const auto& bonus : World::instance().model().getBonuses()) {
    const auto bonusPos = Position(bonus.getX(), bonus.getY());
    if ((selfPos - bonusPos).length() < self.getVisionRange()) {
      moveToBonus = std::make_shared<CommandMoveToPoint>(pathFinder, bonusPos.x, bonusPos.y);
      return moveToBonus->check(self);
    }
  }


  const double fullRadius = self.getRadius() + Game::model().getBonusRadius();

  const auto topBonusPos = Points::point(Points::BONUS_TOP);
  const auto bottomBonusPos = Points::point(Points::BONUS_BOTTOM);

  std::shared_ptr<Algorithm::Path> path;

  pathFinder.calculatePath(topBonusPos, path);
  double ticksToTop = magicCoef * (path->getLength() - fullRadius) / Game::model().getWizardForwardSpeed();

  pathFinder.calculatePath(bottomBonusPos, path);
  double ticksToBottom = magicCoef * (path->getLength() - fullRadius) / Game::model().getWizardForwardSpeed();

  int maxTicksToBonus = Game::model().getBonusAppearanceIntervalTicks();
  int ticksToBonus = maxTicksToBonus - World::model().getTickIndex() % maxTicksToBonus;

  double minMoveTicks = MIN(ticksToTop, ticksToBottom);

  /// ���� ����� �������� �������
  if (ticksToBonus > maxTicksToBonus - 0) {
    ///TODO: �� ��� ���������� �������� �� �� ��� ��� �� �����
  } else {
    /// ���� �� ��������� ������ ��� ������
    if (ticksToBonus > minMoveTicks) {
      return false;
    }
  }


  /// ���� ������ ������, �� ��� ���� �� �����
  if (minMoveTicks > 600) {
    return false;
  }

  /// ���� ���� �� ����� + ��� ���� ������� ����� ��� ������, ������ ���� ��� ���� �� �����, �� �� �� �����
  if (potensialExpirience(self) + minMoveTicks * 0.1  > Game::model().getBonusScoreAmount()) {
    return false;
  }

  const double minDistance = self.getRadius() + Game::model().getBonusRadius();
  const double maxDistance = minDistance + self.getRadius();

  if (ticksToTop < ticksToBottom) {
    bonusPos = topBonusPos;
  } else {
    bonusPos = bottomBonusPos;
  }

  moveToBonus = std::make_shared<CommandKeepDistance>(pathFinder, bonusPos.x, bonusPos.y, minDistance, maxDistance);

  return moveToBonus->check(self);
}

double CommandMoveToBonus::potensialExpirience(const model::Wizard& self) {
  double result = 0;
  for (const auto& enemy : World::instance().aroundEnemies(self)) {
    if (enemy->getLife() < 100) {
      result += enemy->getMaxLife() * 0.25f;
    }
  }

  return result;
}

void CommandMoveToBonus::execute(const model::Wizard& self, Result& result) {
  assert(nullptr != moveToBonus.get());
  moveToBonus->execute(self, result);
  result.priority = MovePriorities::moveToBonus(self, bonusPos);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToBonus::visualization(const Visualizator& visualizator) const {
  assert(nullptr != moveToBonus.get());
  moveToBonus->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR