#include "A_Exec.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include "A_Move.h"

#include "E_Game.h"
#include "E_World.h"

using namespace AICup;

double calcMaxSpeed(const Wizard& self, const Vector& direction, const double speedLimit) {
  Vector speed = Algorithm::maxSpeed(self, self.getAngle(), direction);

  const double maxSpeed = speed.length();
  const double dirLength = direction.length();

  if (speedLimit < 0) {
    return MIN(maxSpeed, dirLength);
  }
  return MIN(maxSpeed, MIN(speedLimit, dirLength));
}


bool Algorithm::execMove(const Wizard& self, const TurnStyle style, const Vector& turnDirection, const Vector& direction, const double speedLimit, model::Move& move) {
  double maxSpeed = calcMaxSpeed(self, direction, speedLimit);

  const auto selfPos = EX::pos(self);
  const auto toPos = selfPos + direction.normal() * maxSpeed;


  Vector fixDirection = direction;
  for (const auto& obstacle : World::instance().obstacles(self, self.getRadius() + maxSpeed + 100/*максимальный радиус в игре*/)) {
    const auto oPos = EX::pos(*obstacle);
    /// если мы пересекаемся за этот тик с препятствием, то меняем направление
    if (Math::distanceToSegment(oPos, selfPos, toPos) < self.getRadius() + obstacle->getRadius()) {
      const auto tangents = Math::tangetsForTwoCircle(selfPos, self.getRadius(), oPos, obstacle->getRadius());
      /// чем меньше значение тем больше угол отклонения
      if (tangents[0].dot(direction.normal()) < tangents[1].dot(direction.normal())) {
        fixDirection = tangents[1];
      } else {
        fixDirection = tangents[0];
      }

      break; /// с двумя объекта за тик пересечься нереально
    }
  }


  ///вообще я так и не понял как эта магия работает, но без двух минусов не пашет
  Vector speed = Vector(fixDirection.x, -fixDirection.y).normal().rotated(self.getAngle());
  speed.y *= -1;
  speed *= maxSpeed;


  move.setSpeed(speed.x);
  move.setStrafeSpeed(speed.y);


  switch (style) {
  case TurnStyle::TURN:
    move.setTurn(self.getAngleTo(self.getX() + turnDirection.x, self.getY() + turnDirection.y));
    break;
  case TurnStyle::NO_TURN:
    break;
  case TurnStyle::BACK_TURN:
    move.setTurn(self.getAngleTo(self.getX() - turnDirection.x, self.getY() - turnDirection.y));
    break;
  case TurnStyle::SIDE_TURN: {
    auto perDirection = turnDirection.perpendicular();
    auto diff = Math::angleDiff(turnDirection.angle(), self.getAngle());
    /// если уол близок к одному смещению, то выбираем направление чтобы идти в центр, дабы при отклонениях не упереться в сторону карты
    if (ABS(diff) < self.maxTurnSpeed()) {
      if (perDirection.dot(Position(2000, 2000) - EX::pos(self)) < 0) {
        perDirection *= -1;
      }

    } else if (perDirection.dot(Vector(1, 0).rotate(self.getAngle())) < 0) {
    /// если угол больше 90 градусов - выбран не оптимальный из двух возможных перпендикуляров
      perDirection *= -1;
    }

    move.setTurn(self.getAngleTo(self.getX() + perDirection.x, self.getY() + perDirection.y));
    break;
  }
  default:
    assert(false && "incorrect turn style");
  }

  return true;
}

bool Algorithm::execAttack(const Wizard& self, const model::ActionType action, const model::LivingUnit& unit, model::Move& move) {
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

  return true;
}