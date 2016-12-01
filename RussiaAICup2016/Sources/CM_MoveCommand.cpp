#include "CM_MoveCommand.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include "E_World.h"

using namespace AICup;


void MoveCommand::Result::set(const Position& pos, const Wizard& self) {
  const auto position = Position(INTERVAL(0, pos.x, World::size()), INTERVAL(0, pos.y, World::size()));

  moveDirection = position - EX::pos(self);
  speedLimit = -1;
  deactivateOtherTurn = false;
}