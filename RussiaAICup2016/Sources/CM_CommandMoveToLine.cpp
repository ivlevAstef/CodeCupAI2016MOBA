//
//File: CM_CommandMoveToLine.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToLine.h"
#include "E_InfluenceMap.h"
#include "E_World.h"
#include "E_Game.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "C_Extensions.h"
#include "C_Math.h"

using namespace AICup;

CommandMoveToLine::CommandMoveToLine(model::LaneType line): line(line) {

}

bool CommandMoveToLine::check(const Wizard&) {
  const auto foreFront = InfluenceMap::instance().getForeFront(line, 0.0f);

  /// если линия находиться на своей базе, значит на базе враги, и очень не выгодно к ней бежать
  if (foreFront.x < 1200 && foreFront.y > World::size() - 1200) {
    return false;
  }

  return true;
}


void CommandMoveToLine::execute(const Wizard& self, Result& result) {
  const auto offset = -100 * self.getRole().getAudacity();
  const auto wizardOffset = -100 * self.getRole().getAudacityWithWizards() * World::instance().wizardCount(line, Game::enemyFaction());
  const auto foreFront = InfluenceMap::instance().getForeFront(line, float(offset + wizardOffset));
  const auto foreFrontDirection = InfluenceMap::instance().getForeDirection(line, foreFront);

  /// 400 это ширина линии, поэтому +-200
  const auto foreFrontLeft = foreFront + foreFrontDirection.perpendicular() * 200;
  const auto foreFrontRight = foreFront - foreFrontDirection.perpendicular() * 200;

  const auto selfPos = EX::pos(self);

  toPoint = Math::point_distanceToSegment(selfPos, foreFrontLeft, foreFrontRight);

  result.set(toPoint, self);
  result.turnStyle = TurnStyle::TURN;
  result.turnPriority = TurnPriority::moveToLine;
}

double CommandMoveToLine::priority(const Wizard& self) {
  const double length = (toPoint - EX::pos(self)).length();
  const double lenC = INTERVAL(1, sqrt(length / 200.0), 1.5);
  return MovePriorities::moveToLine(self, line) * lenC;
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToLine::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, toPoint.x, toPoint.y, 0x00ff00);
  }
}
#endif // ENABLE_VISUALIZATOR