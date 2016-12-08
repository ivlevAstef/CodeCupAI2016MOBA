#include "CM_MoveCommand.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include "E_World.h"

using namespace AICup;


void MoveCommand::Result::set(const Position& pos, const Wizard& self) {
  /// короче далее магия - ставим точку так, чтобы она не выходила за пределы карты, но при этом сохранив вектор движения
  const double x = INTERVAL(0, pos.x, World::size());
  const double y = INTERVAL(0, pos.y, World::size());

  const auto selfPos = EX::pos(self);
  const auto posOnArea = Position(x, y);

  if (posOnArea == pos) {
    moveDirection = pos - selfPos;
  } else {
    const auto posN = Math::point_distanceToLine(posOnArea, pos, selfPos);
    const double subLength = (posOnArea - pos).length2() / (pos - posN).length();

    const auto fullDirection = pos - EX::pos(self);
    const auto finalPos = fullDirection.normal() * (fullDirection.length() - subLength);

    moveDirection = finalPos - selfPos;
  }

  if (moveDirection.length() < 1.0e-5) {
    turnDirection = moveDirection;
  } else {
    turnDirection = moveDirection.normal();
  }
}