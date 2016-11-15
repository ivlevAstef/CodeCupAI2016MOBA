//
//File: CM_CommandMoveToBonus.cpp
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#include "CM_CommandMoveToBonus.h"
#include "CM_CommandMoveToPoint.h"
#include "CM_CommandKeepDistance.h"
#include "E_World.h"
#include "E_Graph.h"
#include "E_Game.h"
#include "C_Math.h"

using namespace AICup;

CommandMoveToBonus::CommandMoveToBonus() {

}

bool CommandMoveToBonus::check(const model::Wizard& self) {
  const auto selfPos = Position(self.getX(), self.getY());

  /// если есть бонусы рядом
  for (const auto& bonus : World::instance().model().getBonuses()) {
    const auto bonusPos = Position(bonus.getX(), bonus.getY());
    if ((selfPos - bonusPos).length() < self.getVisionRange()) {
      moveToBonus = std::make_shared<CommandMoveToPoint>(bonusPos.x, bonusPos.y);
      return moveToBonus->check(self);
    }
  }


  const auto topBonusPos = Graph::instance().position(Graph::BONUS_TOP);
  const auto bottomBonusPos = Graph::instance().position(Graph::BONUS_BOTTOM);

  /// к сожалению длина пути считаеться по графу где можно точно пройти, а он обычно длинее чем реальный путь
  /// поэтому в вычислениях береться среднее значение между прямым путем, и путем по графу

  double length = 99999;
  Graph::instance().path(selfPos, topBonusPos, length); // сам путь нас не интерисует
  length = length * 0.4 + (selfPos - topBonusPos).length() * 0.6;
  double ticksToTop = length / Game::instance().model().getWizardForwardSpeed();

  length = 99999;
  Graph::instance().path(selfPos, bottomBonusPos, length); // сам путь нас не интерисует
  length = length * 0.4 + (selfPos - topBonusPos).length() * 0.6;
  double ticksToBottom = length / Game::instance().model().getWizardForwardSpeed();

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

  if (topBonusPos < bottomBonusPos) {
    moveToBonus = std::make_shared<CommandKeepDistance>(topBonusPos.x, topBonusPos.y, minDistance, maxDistance);
  } else {
    moveToBonus = std::make_shared<CommandKeepDistance>(topBonusPos.x, topBonusPos.y, minDistance, maxDistance);
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
  return 0;
}


void CommandMoveToBonus::execute(const model::Wizard& self, model::Move& move) {
  assert(nullptr != moveToBonus.get());
  moveToBonus->execute(self, move);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToBonus::visualization(const Visualizator& visualizator) const {
  assert(nullptr != moveToBonus.get());
  moveToBonus->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR