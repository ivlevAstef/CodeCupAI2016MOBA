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

  /// ���� ���� ������ �����
  for (const auto& bonus : World::instance().model().getBonuses()) {
    const auto bonusPos = Position(bonus.getX(), bonus.getY());
    if ((selfPos - bonusPos).length() < self.getVisionRange()) {
      moveToBonus = std::make_shared<CommandMoveToPoint>(bonusPos.x, bonusPos.y);
      return moveToBonus->check(self);
    }
  }


  const auto topBonusPos = Graph::instance().position(Graph::BONUS_TOP);
  const auto bottomBonusPos = Graph::instance().position(Graph::BONUS_BOTTOM);

  /// � ��������� ����� ���� ���������� �� ����� ��� ����� ����� ������, � �� ������ ������ ��� �������� ����
  /// ������� � ����������� �������� ������� �������� ����� ������ �����, � ����� �� �����

  double length = 99999;
  Graph::instance().path(selfPos, topBonusPos, length); // ��� ���� ��� �� ����������
  length = length * 0.4 + (selfPos - topBonusPos).length() * 0.6;
  double ticksToTop = length / Game::instance().model().getWizardForwardSpeed();

  length = 99999;
  Graph::instance().path(selfPos, bottomBonusPos, length); // ��� ���� ��� �� ����������
  length = length * 0.4 + (selfPos - topBonusPos).length() * 0.6;
  double ticksToBottom = length / Game::instance().model().getWizardForwardSpeed();

  int maxTicksToBonus = Game::instance().model().getBonusAppearanceIntervalTicks();
  int ticksToBonus = maxTicksToBonus - World::instance().model().getTickIndex() % maxTicksToBonus;

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