#include "S_CommandStrategy.h"
#include "A_Exec.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "C_Logger.h"

#include "E_World.h"
#include "A_Move.h"
#include <cassert>
#include <algorithm>


using namespace AICup;

CommandStrategy::CommandStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& finder):
  fabric(fabric), pathFinder(finder) {
}

void CommandStrategy::update(const Wizard& self, model::Move& finalMove) {
  auto moveResults = moveCommandsToMoveResult(self);

  {
    Vector direction;
    if (move(moveResults, self, direction)) {
      Vector turnDirection = turn(moveResults);

      /// ���� �� � ���������, ������ ��� �����... ��� �������� ��������
      if (direction.length() < 1.0e-5) {
        Algorithm::execAroundMove(self, finalMove);
      } else {
        if (turnDirection.length() < 0.1) {
          turnDirection = direction;
        }

        Algorithm::execMove(self, turnDirection, direction, finalMove);
      }
    }
  }

  if (!attackCommands.empty()) {
    model::ActionType action;
    const model::LivingUnit* unit = attack(self, action);
    if (nullptr != unit) {
      Algorithm::execAttack(self, action, *unit, finalMove);
    }
  }

  if (!castCommands.empty()) {
    model::ActionType action;
    const auto unit = cast(self, action);
    if (nullptr != unit) {
      Algorithm::execCast(self, action, *unit, finalMove);
    }
  }
}

void CommandStrategy::clear() {
  moveCommands.clear();
  attackCommands.clear();
  castCommands.clear();
}

std::vector<MoveCommand::Result> CommandStrategy::moveCommandsToMoveResult(const Wizard& self) const {
  std::vector<MoveCommand::Result> moveResults;
  moveResults.reserve(moveCommands.size());

  for (size_t index = 0; index < moveCommands.size(); index++) {
    MoveCommand::Result res;
    moveCommands[index]->execute(self, res);

    if (res.priority >= 1 && res.moveDirection.length() > 0.1) {
      moveResults.push_back(res);
    }
  }

  return moveResults;
}

const Vector CommandStrategy::turn(const std::vector<MoveCommand::Result>& moveResults) {
  assert(!moveResults.empty());

  /// �������� ����� ���������������� ��� ��������
  double maxPriority = 0;
  Vector result = Vector(0, 0);
  for (const auto& moveIter : moveResults) {
    const auto direction = moveIter.turnDirection;

    double sumPriority = 0;
    for (const auto& move : moveResults) {
      if (direction.length() < 0.1 || move.turnDirection.length() < 0.1) {
        sumPriority += move.turnPriority * move.priority;
      } else {
        sumPriority += move.turnDirection.normal().dot(direction) * move.turnPriority * move.priority;
      }
    }

    if (sumPriority > maxPriority) {
      sumPriority = maxPriority;
      result = direction;
    }
  }

  return result;
}

const Vector CommandStrategy::calcMoveVector(const std::vector<MoveCommand::Result>& moveResults, const Wizard& self) {
  if (moveResults.empty()) { /// ���� ��� �������� ������ ��������� ������ (P.S. ��� �������� ���� ����, ����� ��� 0 �� ���������)
    return Vector(EX::maxSpeed(self), 0).rotated(self.getAngle());
  }

  double maxPriority = 0;
  Vector result = Vector(0, 0);
  for (const auto& moveIter : moveResults) {
    const auto direction = moveIter.moveDirection.normal();

    double sumPriority = 0;
    for (const auto& move : moveResults) {
      sumPriority += move.moveDirection.normal().dot(direction) * move.priority;
    }

    if (sumPriority > maxPriority) {
      sumPriority = maxPriority;
      result = moveIter.moveDirection;
    }
  }

  return result;
}

bool CommandStrategy::move(std::vector<MoveCommand::Result>& moveResults, const Wizard& self, Vector& direction) {
  const auto moveVector = calcMoveVector(moveResults, self);

  addAvoidProjectiles(moveResults, self, moveVector);

  if (moveResults.empty()) {
    return false;
  }

  const auto finalMoveVector = calcMoveVector(moveResults, self);

  direction = calculateCollisions(self, EX::pos(self) + finalMoveVector/*���� ���� ����*/);

  return true;
}

const Vector CommandStrategy::calculateCollisions(const Wizard& self, const Position& endPoint) {
  movePosition = endPoint;
  pathFinder.calculatePath(endPoint, path);
  assert(nullptr != path);

  const Position preEndPoint = path->calculateNearestCurvaturePoint(self.getVisionRange() / 2);

  /// need remove obstacles
  auto obstacles = World::instance().obstacles(self, self.getVisionRange(), preEndPoint - EX::pos(self));
  const auto treesForRemove = path->removeObstacles(obstacles);
  const auto obstaclesGroups = World::instance().createGroup(obstacles, self.getRadius());


  for (const auto& tree : treesForRemove) {
    addTreeForRemove(self, tree);
  }

  return Algorithm::move(self, preEndPoint, obstaclesGroups, self.getVisionRange());
}


void CommandStrategy::addTreeForRemove(const Wizard& self, const model::LivingUnit* tree) {
  LogAssert(nullptr != tree);

  const double distance = self.getDistanceTo(*tree);
  if (distance > self.getVisionRange()) {
    return;
  }

  const auto attack = fabric.attack(*tree);
  if (attack->check(self)) {
    attackCommands.push_back(attack);
  }
}


void CommandStrategy::addAvoidProjectiles(std::vector<MoveCommand::Result>& moveResults, const Wizard& self, const Vector& moveDirection) {

  for (const auto& projectile : World::instance().bullets()) {
    //if (projectile.faction == self.getFaction()) { // ���������� ���� �������, ��� ��� ��� �������� ����� � ���
    //  continue;
    //}
    const auto command = fabric.avoidProjectile(projectile, moveDirection);

    MoveCommand::Result moveCommandResult;
    if (command->check(self)) {
      command->execute(self, moveCommandResult);

      if (moveCommandResult.priority >= 1 && moveCommandResult.moveDirection.length() > 0.1) {
        moveResults.push_back(moveCommandResult);
      }
    }
  }
}

const model::LivingUnit* CommandStrategy::attack(const Wizard& self, model::ActionType& action) {
  AttackCommand::Result result;
  result.priority = 0;

  for (const auto& attackCommand: attackCommands) {
    AttackCommand::Result attackResult;
    attackCommand->execute(self, attackResult);

    if (attackResult.priority > result.priority) {
      result = attackResult;
    }
  }

  if (result.priority < 1) {
    return nullptr;
  }

  action = result.action;
  return result.unit;
}

const model::LivingUnit* CommandStrategy::cast(const Wizard& self, model::ActionType& action) {
  CastCommand::Result result;
  result.priority = 0;

  for (const auto& castCommand: castCommands) {
    CastCommand::Result castResult;
    castCommand->execute(self, castResult);

    if (castResult.priority > result.priority) {
      result = castResult;
    }
  }

  if (result.priority < 1) {
    return nullptr;
  }

  action = result.action;
  return result.unit;
}


#ifdef ENABLE_VISUALIZATOR
void CommandStrategy::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  /*for (const auto& command : moveCommands) {
    command->visualization(self, visualizator);
  }*/

  if (Visualizator::PRE == visualizator.getStyle()) {
    /*for (const auto& move : moveCommandsToMoveResult(self)) {
      visualizator.line(self.getX(), self.getY(), self.getX() + move.moveDirection.x, self.getY() + move.moveDirection.y, 0xff0000);
    }*/
  }

  for (const auto& command : attackCommands) {
    command->visualization(self, visualizator);
  }

  if (Visualizator::POST == visualizator.getStyle()) {
    if (nullptr != path) {
      path->visualization(visualizator);

      visualizator.line(self.getX(), self.getY(), movePosition.x, movePosition.y, 0x000000);
    }
  }
}
#endif
