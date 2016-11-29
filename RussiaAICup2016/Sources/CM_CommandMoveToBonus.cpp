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
#include "CM_TurnPriority.h"

using namespace AICup;

CommandMoveToBonus::CommandMoveToBonus(Algorithm::PathFinder& finder):
  MoveCommand(finder) {

}

bool CommandMoveToBonus::check(const Wizard& self) {
  const auto selfPos = Position(self.getX(), self.getY());

  /// если есть бонусы рядом
  for (const auto& bonus : World::instance().model().getBonuses()) {
    const auto cBonusPos = Position(bonus.getX(), bonus.getY());
    if ((selfPos - cBonusPos).length() < self.getVisionRange()) {
      moveToBonus = std::make_shared<CommandMoveToPoint>(pathFinder, cBonusPos.x, cBonusPos.y);
      return moveToBonus->check(self);
    }
  }


  const double fullRadius = self.getRadius() + Game::model().getBonusRadius();

  const auto topBonusPos = Points::point(Points::BONUS_TOP);
  const auto bottomBonusPos = Points::point(Points::BONUS_BOTTOM);

  std::shared_ptr<Algorithm::Path> path;

  pathFinder.calculatePath(topBonusPos, path);
  double ticksToTop = (path->getRealLength() - fullRadius) / self.maxSpeed();

  pathFinder.calculatePath(bottomBonusPos, path);
  double ticksToBottom = (path->getRealLength() - fullRadius) / self.maxSpeed();

  int maxTicksToBonus = Game::model().getBonusAppearanceIntervalTicks();
  int ticksToBonus = maxTicksToBonus - World::model().getTickIndex() % maxTicksToBonus;

  double minMoveTicks = MIN(ticksToTop, ticksToBottom);

  /// если бонус появился недавно
  if (ticksToBonus > maxTicksToBonus - 0) {
    ///TODO: ну тут желательна проверка на то что его не взяли
  } else {
    /// если до появления бонуса еще далеко
    if (ticksToBonus > minMoveTicks) {
      return false;
    }
  }


  /// если бежать далеко, то оно того не стоит
  if (minMoveTicks > 600) {
    return false;
  }

  /// если опыт на линии + тот опыт который может еще прийти, больше того что дают за бонус, то он не нужен
  if (potensialExpirience(self) + minMoveTicks * 0.2  > Game::model().getBonusScoreAmount()) {
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

double CommandMoveToBonus::potensialExpirience(const Wizard& self) {
  double result = 0;
  for (const auto& enemy : World::instance().aroundEnemies(self, self.getVisionRange() + 200)) {
    if (enemy->getLife() < 150) {
      result += enemy->getMaxLife() * 0.25f;
    }
  }

  return result;
}

void CommandMoveToBonus::execute(const Wizard& self, Result& result) {
  assert(nullptr != moveToBonus.get());
  moveToBonus->execute(self, result);
  result.turnPriority = TurnPriority::moveToBonus;
}

double CommandMoveToBonus::priority(const Wizard& self) {
  return MovePriorities::moveToBonus(self, bonusPos);
}


#ifdef ENABLE_VISUALIZATOR
void CommandMoveToBonus::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  assert(nullptr != moveToBonus.get());
  moveToBonus->visualization(self, visualizator);
}
#endif // ENABLE_VISUALIZATOR