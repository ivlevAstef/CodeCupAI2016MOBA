#include "A_Exec.h"
#include "C_Extensions.h"
#include "C_Math.h"

#include "E_Game.h"

using namespace AICup;

bool Algorithm::execMove(const model::Wizard& self, const TurnStyle style, const Vector& direction, model::Move& move) {
  ///вообще я так и не понял как эта магия работает, но без двух минусов не пашет
  Vector speed = Vector(direction.x, -direction.y).normal().rotated(self.getAngle());
  speed.y *= -1;

  const double directionLength = direction.length();
  const double maxSpeed = EX::maxSpeed(self);

  speed *= MIN(directionLength, maxSpeed);

  move.setSpeed(speed.x);
  move.setStrafeSpeed(speed.y);

  switch (style) {
  case TurnStyle::TURN:
    move.setTurn(self.getAngleTo(self.getX() + direction.x, self.getY() + direction.y));
    break;
  case TurnStyle::NO_TURN:
    break;
  case TurnStyle::BACK_TURN:
    move.setTurn(self.getAngleTo(self.getX() - direction.x, self.getY() - direction.y));
    break;
  default:
    assert(false && "incorrect turn style");
  }

  return true;
}

bool Algorithm::execAttack(const model::Wizard& self, const model::ActionType action, const model::LivingUnit& unit, model::Move& move) {
  const double angleDeviation = self.getAngleTo(unit);

  /// в любом случае поворачиваемся к врагу, для упрощения своей жизни
  move.setTurn(angleDeviation);

  /// не тот угол, чтобы атаковать
  if (abs(angleDeviation) > Game::model().getStaffSector() * 0.5) {
    return false;
  }

  move.setCastAngle(angleDeviation);

  const double distance = self.getDistanceTo(unit);

  move.setMinCastDistance(distance - unit.getRadius());
  move.setMaxCastDistance(distance + unit.getRadius() * 2); // с запасом
  move.setAction(action);
}