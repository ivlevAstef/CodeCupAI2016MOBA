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

CommandStrategy::CommandStrategy(const CommandFabric& fabric, RolePtr role, SkillBuildPtr skillBuild):
  fabric(fabric), role(role), skillBuild(skillBuild) {
}

void CommandStrategy::update(const Wizard& self, model::Move& finalMove) {
  auto moveResults = moveCommandsToMoveResult(self);

  bool forceTurn = false;
  {
    Vector direction;
    if (move(moveResults, self, direction)) {
      Vector turnDirection = turn(moveResults);

      /// ���� �� � ���������, ������ ��� �����... ��� �������� ��������
      assert(direction.length() > 0.1);

      if (turnDirection.length() < 0.1) {
        turnDirection = direction;
      }

      Algorithm::execMove(self, turnDirection, direction, finalMove);
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

Wizard CommandStrategy::preUpdate(const model::Wizard& self, model::Move& move) {
  role->update(self);
  skillBuild->update(self, move);

  /// ������ ��������� ������ ����, ����� ������ � ��� ��������
  return Wizard(self, *role);
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
      maxPriority = sumPriority;
      result = direction;
    }
  }


  return result;
}

const Vector CommandStrategy::calcMoveVector(const std::vector<MoveCommand::Result>& moveResults, const Wizard& self) {
  if (moveResults.empty()) { /// ���� ��� �������� ������ ��������� ������ (P.S. ��� �������� ���� ����, ����� ��� 0 �� ���������)
    return Vector(EX::maxSpeed(self), 0).rotated(self.getAngle());
  }

  ///������� ������� ����� ������������ ������ �� force, ���� ���������� �� �������
  Vector maxVector = Vector(0, 0);
  double maxPriority = 0;

  for (const auto& moveIter : moveResults) {
    if (moveIter.force && moveIter.priority > maxPriority) {
      maxPriority = moveIter.priority;
      maxVector = moveIter.moveDirection;
    }
  }

  /// ���� ����� �� ����� �� ������ ���
  if (maxPriority > 0) {
    return maxVector;
  }

  /// ���� �� �����, �� ������ ����� ��������� �������� ���������� ������ ��� � ������ ��������
  Vector preFinalVector = calcMoveOldVector(moveResults, self);

  /// �� � ������� �� force �������, � ����� ������� ����� ������� ��� �����, ���� ������ ���� �������
  return preFinalVector * (MAX(240, preFinalVector.length()) / preFinalVector.length());

}

const Vector CommandStrategy::calcMoveOldVector(const std::vector<MoveCommand::Result>& moveResults, const Wizard& self) {
  ///������� ������� ����� ������������ ������
  double maxSumPriority = 0;
  Vector maxVector = Vector(0, 0);
  double maxPriority = 0;

  for (const auto& moveIter : moveResults) {
    const auto direction = moveIter.moveDirection.normal();

    double sumPriority = 0;
    for (const auto& move : moveResults) {
      sumPriority += move.moveDirection.normal().dot(direction) * move.priority;
    }

    if (sumPriority > maxSumPriority) {
      maxSumPriority = sumPriority;
      maxVector = moveIter.moveDirection;
      maxPriority = moveIter.priority;
    }
  }

  if (0 == maxPriority) {
    return maxVector;
  }

  double sumPriority = 0;
  Vector result = Vector(0, 0);
  //����� ���������� ���, � ������������� ���������, ������� ���������� � ������� +-60 ��������
  for (const auto& moveIter : moveResults) {
    const auto direction = moveIter.moveDirection;
    const auto dot = direction.normal().dot(maxVector.normal());

    /// ���� ���� � �������� 45 �������� - cos(45) = 0.5
    if (dot > 0.5) {
      const auto priority = dot * moveIter.priority;

      sumPriority += priority;
      result += direction * priority;
    }

  }


  return result / sumPriority;
}

bool CommandStrategy::move(std::vector<MoveCommand::Result>& moveResults, const Wizard& self, Vector& direction) {
  const auto moveVector = calcMoveVector(moveResults, self);

  addAvoidProjectiles(moveResults, self, moveVector);

  if (moveResults.empty()) {
    return false;
  }

  const auto finalMoveVector = calcMoveVector(moveResults, self);
  const auto selfPos = EX::pos(self);

  direction = calculateCollisions(self, selfPos + finalMoveVector/*���� ���� ����*/);
  /// ���� ��� ��������, ������ ���� �����, ����� ���� ����� ������ ��� ����
  if (direction.length() < 1) {
    direction = finalMoveVector;
    for (const auto& tree : World::instance().trees()) {
      if (Math::distanceToLine(EX::pos(tree), selfPos, selfPos + direction) < tree.getRadius() + self.getRadius() + 1) {
        addTreeForRemove(self, &tree);
      }
    }
  }

  return true;
}

const Vector CommandStrategy::calculateCollisions(const Wizard& self, const Position& endPoint) {
  auto mEndPoint = endPoint;
  /// ���� ����� ������� �� ������� ����� �� �� � ����������� ��������, ���� ��� ����� ����������� � ��������� �� ����
  if (endPoint.x < self.getRadius() || endPoint.x > World::size() - self.getRadius()
    || endPoint.y < self.getRadius() || endPoint.y > World::size() - self.getRadius()) {
    const auto& delta = endPoint - EX::pos(self);
    mEndPoint += delta.normal() * 100;
  }

  Algorithm::PathFinder::instance().calculatePath(mEndPoint, path);
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
    if (projectile.faction == self.getFaction()) { // ���������� ���� �������, ��� ��� ��� �������� ����� � ���
      continue;
    }
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
  path->visualization(visualizator);
  if (Visualizator::PRE == visualizator.getStyle()) {
    for (const auto& command : moveCommands) {
      command->visualization(self, visualizator);
    }
  }

  for (const auto& command : attackCommands) {
    command->visualization(self, visualizator);
  }
}
#endif
