//
//File: CM_CommandMoveToBonus.cpp
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#include "CM_CommandMoveToBonus.h"
#include "E_World.h"
#include "E_Points.h"
#include "E_Game.h"
#include "C_Math.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "C_Extensions.h"

using namespace AICup;

bool hasNearestWizard(const Wizard& self, const Position& bonusPos) {
  const auto selfPos = EX::pos(self);

  const auto distance2 = (selfPos - bonusPos).length2();

  /// проверяем что нет магoв который ближе к бонусу
  for (const auto& wizard : World::model().getWizards()) {
    if (wizard.getId() == self.getId()) {
      continue;
    }

    if (distance2 > (EX::pos(wizard) - bonusPos).length2() + Game::model().getBonusRadius()) {
      return true;
    }
  }

  return false;
}

CommandMoveToBonus::CommandMoveToBonus(Algorithm::PathFinder& finder): pathFinder(finder) {
}

bool CommandMoveToBonus::check(const Wizard& self) {
  /// не стоит бежать за бонусом в конце игры
  if (World::model().getTickIndex() > 19000) {
    return false;
  }

  static const Position topBonusPos = Points::point(Points::BONUS_TOP);
  static const Position bottomBonusPos = Points::point(Points::BONUS_BOTTOM);
  const auto selfPos = EX::pos(self);

  /// есть маг которому ближе чем мне
  bool ignoreTop = (selfPos - topBonusPos).length() < self.getVisionRange() && hasNearestWizard(self, topBonusPos);

  /// есть маг которому ближе чем мне
  bool ignoreBottom = (selfPos - bottomBonusPos).length() < self.getVisionRange() && hasNearestWizard(self, bottomBonusPos);

  if (ignoreTop && ignoreBottom) {
    return false;
  }

  /// если есть бонусы рядом
  for (const auto& bonus : World::model().getBonuses()) {
    const auto cBonusPos = EX::pos(bonus);
    if ((selfPos - cBonusPos).length() < self.getVisionRange() && !hasNearestWizard(self, cBonusPos)) {
      bonusPos = cBonusPos;
      return true;
    }
  }


  const double fullRadius = self.getRadius() + Game::model().getBonusRadius();

  std::shared_ptr<Algorithm::Path> path;

  pathFinder.calculatePath(topBonusPos, path);
  double ticksToTop = (path->getRealLength() - fullRadius) / self.maxSpeed();

  pathFinder.calculatePath(bottomBonusPos, path);
  double ticksToBottom = (path->getRealLength() - fullRadius) / self.maxSpeed();

  ticksToTop = ignoreTop ? 99999 : ticksToTop;
  ticksToBottom = ignoreBottom ? 99999 : ticksToBottom;

  int maxTicksToBonus = Game::model().getBonusAppearanceIntervalTicks();
  int ticksToBonus = maxTicksToBonus - World::model().getTickIndex() % maxTicksToBonus;

  double minMoveTicks = MIN(ticksToTop, ticksToBottom);

  /// если до появления бонуса еще далеко
  if (ticksToBonus > minMoveTicks) {
    return false;
  }

  /// если бежать далеко, то оно того не стоит
  if (minMoveTicks > 280) {
    return false;
  }

  /// если опыт на линии + тот опыт который может еще прийти, больше того что дают за бонус, то он не нужен
  if (potensialExpirience(self) + minMoveTicks * 0.1  > Game::model().getBonusScoreAmount()) {
    return false;
  }

  if (ticksToTop < ticksToBottom) {
    bonusPos = topBonusPos;
  } else {
    bonusPos = bottomBonusPos;
  }



  const double minDistance = self.getRadius() + Game::model().getBonusRadius();
  const auto delta = bonusPos - selfPos;
  bonusPos = selfPos + delta.normal() * (delta.length() - minDistance);

  return true;
}

double CommandMoveToBonus::potensialExpirience(const Wizard& self) {
  double result = 0;
  for (const auto& enemy : World::instance().aroundEnemies(self, self.getVisionRange() + 100)) {
    if (enemy->getLife() < 150) {
      result += enemy->getMaxLife() * 0.25f;
    }
  }

  return result;
}

void CommandMoveToBonus::execute(const Wizard& self, Result& result) {
  result.set(bonusPos, self);

  result.turnPriority = TurnPriority::moveToBonus;
  result.priority = MovePriorities::moveToBonus(self, bonusPos);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToBonus::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, bonusPos.x, bonusPos.y, 0x00ff77);
  }
}
#endif // ENABLE_VISUALIZATOR