//
//File: CM_CommandMoveToLine.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToLine.h"
#include "E_InfluenceMap.h"
#include "A_WinPredictor.h"
#include "E_World.h"
#include "E_Game.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "C_Extensions.h"
#include "C_Math.h"

using namespace AICup;

CommandMoveToLine::CommandMoveToLine(model::LaneType line): line(line) {

}

bool CommandMoveToLine::check(const Wizard& self) {
  const auto offset = -100 * self.getRole().getAudacity();

  const auto preForeFront = InfluenceMap::instance().getForeFront(line, 0);
  const double changeOfWinning = Algorithm::changeOfWinning(self, preForeFront.x, preForeFront.y);
  const double factor = (1 + changeOfWinning) * 0.5 * self.getRole().getLinePressureWizards()
                      - (1 - changeOfWinning) * 0.5 * self.getRole().getLineAudacityWizard();

  const auto wizardOffset = -100 * factor;

  const auto foreFront = InfluenceMap::instance().getForeFront(line, float(offset + wizardOffset));
  const auto foreFrontDirection = InfluenceMap::instance().getForeDirection(line, foreFront);

  /// 400 это ширина линии, будем ходить по 1/2 этой линии, поэтому +-100
  const auto foreFrontLeft = foreFront + foreFrontDirection.perpendicular() * 100;
  const auto foreFrontRight = foreFront - foreFrontDirection.perpendicular() * 100;

  const auto selfPos = EX::pos(self);

  toPoint = Math::point_distanceToSegment(selfPos, foreFrontLeft, foreFrontRight);

  return (toPoint-selfPos).length() > 1;
}


void CommandMoveToLine::execute(const Wizard& self, Result& result) {
  result.set(toPoint, self);
  result.turnDirection = Vector();

  result.turnPriority = TurnPriority::moveToLine;
  result.priority = priority(self);
}

double CommandMoveToLine::priority(const Wizard& self) const {
  /// если линия находиться на своей базе, и мы тоже на ней, значит на базе враги, и нужно обращать внимание на врагов, а не на линию
  if (toPoint.x < 800 && toPoint.y > World::size() - 800
    && self.getX() < 800 && self.getY() > World::size() - 800) {
    return MovePriorities::moveToLine(self, line) * 0.5;
  }

  const double length = (toPoint - EX::pos(self)).length();
  const double lenC = INTERVAL(1, sqrt(length / 200.0), 7);
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