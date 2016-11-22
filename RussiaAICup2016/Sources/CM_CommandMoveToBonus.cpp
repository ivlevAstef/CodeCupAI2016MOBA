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

using namespace AICup;

CommandMoveToBonus::CommandMoveToBonus(Algorithm::PathFinder& finder):
  MoveCommand(finder) {

}

bool CommandMoveToBonus::check(const model::Wizard& self) {
  const auto selfPos = Position(self.getX(), self.getY());

  /// если есть бонусы рядом
  for (const auto& bonus : World::instance().model().getBonuses()) {
    const auto bonusPos = Position(bonus.getX(), bonus.getY());
    if ((selfPos - bonusPos).length() < self.getVisionRange()) {
      moveToBonus = std::make_shared<CommandMoveToPoint>(pathFinder, bonusPos.x, bonusPos.y);
      return moveToBonus->check(self);
    }
  }


  const auto topBonusPos = Points::point(Points::BONUS_TOP);
  const auto bottomBonusPos = Points::point(Points::BONUS_BOTTOM);

  Algorithm::Path* path = nullptr;

  pathFinder.calculatePath(topBonusPos, &path);
  double ticksToTop = path->getLength() / Game::instance().model().getWizardForwardSpeed();

  pathFinder.calculatePath(bottomBonusPos, &path);
  double ticksToBottom = path->getLength() / Game::instance().model().getWizardForwardSpeed();

  int maxTicksToBonus = Game::instance().model().getBonusAppearanceIntervalTicks();
  int ticksToBonus = maxTicksToBonus - World::instance().model().getTickIndex() % maxTicksToBonus;

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
  if (potensialExpirience(self) + minMoveTicks * 0.1  > Game::instance().model().getBonusScoreAmount()) {
    return false;
  }

  const double minDistance = self.getRadius() + Game::instance().model().getBonusRadius();
  const double maxDistance = minDistance + self.getRadius();

  if (ticksToTop < ticksToBottom) {
    moveToBonus = std::make_shared<CommandKeepDistance>(pathFinder, topBonusPos.x, topBonusPos.y, minDistance, maxDistance);
  } else {
    moveToBonus = std::make_shared<CommandKeepDistance>(pathFinder, topBonusPos.x, topBonusPos.y, minDistance, maxDistance);
  }

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

int CommandMoveToBonus::priority(const model::Wizard& self) {
  return 2000;
}


void CommandMoveToBonus::execute(const model::Wizard& self, Result& result) {
  assert(nullptr != moveToBonus.get());
  moveToBonus->execute(self, result);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToBonus::visualization(const Visualizator& visualizator) const {
  assert(nullptr != moveToBonus.get());
  moveToBonus->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR