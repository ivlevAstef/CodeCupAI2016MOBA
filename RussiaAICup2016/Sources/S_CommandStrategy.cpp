#include "S_CommandStrategy.h"
#include "A_Exec.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "C_Logger.h"
#include <cassert>


using namespace AICup;

CommandStrategy::CommandStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& finder):
  fabric(fabric), pathFinder(finder) {
}

void CommandStrategy::update(const Wizard& self, model::Move& finalMove) {
  const auto moveResults = moveCommandsToMoveResult(self);

  bool deactivateOtherTurn = false;
  if (!moveResults.empty()) {
    TurnStyle turnStyle = TurnStyle::NO_TURN;
    Vector turnDirection = Vector(1, 0);
    double speedLimit = -1;
    const Vector direction = move(moveResults, self, turnStyle, turnDirection, speedLimit, deactivateOtherTurn);

    Algorithm::execMove(self, turnStyle, turnDirection, direction, speedLimit, finalMove);
  }

  double turnSave = finalMove.getTurn();

  if (!attackCommands.empty()) {
    model::ActionType action;
    const model::LivingUnit* unit = attack(self, action);
    if (nullptr != unit) {
      Algorithm::execAttack(self, action, *unit, finalMove);
    }
  }

  if (!castCommands.empty()) {
    model::ActionType action;
    long long int id = cast(self, action);
    if (id > 0) {
      finalMove.setStatusTargetId(id);
      finalMove.setAction(action);
    }
  }

  if (deactivateOtherTurn) {
    finalMove.setTurn(turnSave);
  }
}

void CommandStrategy::clear() {
  moveCommands.clear();
  attackCommands.clear();
  castCommands.clear();
}

std::vector<MoveCommand::Result> CommandStrategy::moveCommandsToMoveResult(const Wizard& self) const {
  std::vector<MoveCommand::Result> moveResults;
  moveResults.resize(moveCommands.size());

  for (size_t index = 0; index < moveCommands.size(); index++) {
    moveResults[index].priority = moveCommands[index]->priority(self);

    if (moveResults[index].priority >= 1) {
      moveCommands[index]->execute(self, moveResults[index]);
    }
  }

  /// Удаляем вектора, которые очень короткие, или не приоритетные
  for (size_t index = 0; index < moveResults.size(); index++) {
    const size_t i = moveResults.size() - index - 1;
    if (moveResults[i].priority < 1 || moveResults[i].moveDirection.length() < 0.5) {
      moveResults.erase(moveResults.begin() + i);
    }
  }

  return moveResults;
}

const Vector CommandStrategy::move(const std::vector<MoveCommand::Result>& moveResults, const Wizard& self, TurnStyle& turnStyle, Vector& turnDirection, double& speedLimit, bool& deactivateOtherTurn) {
  for (const auto& move : moveResults) {
    for (const auto& tree : move.treesForRemove) {
      addTreeForRemove(self, tree);
    }
  }


  /// выбираем самый предпочтительный вид поворота
  int turnPriority = 0;
  double normalPriority = 0;
  for (const auto& move : moveResults) {
    if (move.turnPriority < turnPriority) {
      continue;
    }
    if (move.turnPriority > turnPriority) {
      turnPriority = move.turnPriority;
    } else if (move.priority < normalPriority) { //equals
      continue;
    }

    normalPriority = move.priority;

    turnStyle = move.turnStyle;
    turnDirection = move.moveDirection;
    deactivateOtherTurn = move.deactivateOtherTurn;
  }


  double minDeviation = 9999999;
  Vector result = Vector(0, 0);
  //Проходим по углам, и высчитываем какой вариант даст наименьшое отклонение
  for (size_t alphaI = 0; alphaI < 360; alphaI++) {
    const double alpha = AICUP_PI * double(alphaI) / 180.0;

    const auto direction = Vector(1, 0).rotate(alpha);

    double summaryDeviation = 0;
    for (const auto& move : moveResults) {
      const double deviation = 1 - move.moveDirection.normal().dot(direction);
      summaryDeviation += deviation * double(move.priority) / 1000.0;
    }

    if (summaryDeviation < minDeviation) {
      minDeviation = summaryDeviation;
      result = direction;
    }
  }


  speedLimit = EX::maxSpeed(self);
  for (const auto& move : moveResults) {
    if (move.speedLimit > 0 && move.speedLimit < speedLimit) {
      speedLimit = move.speedLimit;
    }
  }

  return result * speedLimit;
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

const model::LivingUnit* CommandStrategy::attack(const Wizard& self, model::ActionType& action) {
  AttackCommandPtr maxPriorityAttack = nullptr;
  double maxPriority = 0;

  for (const auto& attackCommand: attackCommands) {
    const double priority = attackCommand->priority(self);
    if (priority > maxPriority) {
      maxPriority = priority;
      maxPriorityAttack = attackCommand;
    }
  }

  if (nullptr == maxPriorityAttack) {
    return nullptr;
  }

  AttackCommand::Result result;
  maxPriorityAttack->execute(self, result);

  action = result.action;
  return result.unit;
}

const long long int CommandStrategy::cast(const Wizard& self, model::ActionType& action) {
  double maxPriority = 0;
  CastCommandPtr maxCastCommand = nullptr;

  for (const auto& castCommand: castCommands) {
    const double priority = castCommand->priority(self);
    if (priority > maxPriority) {
      maxPriority = priority;
      maxCastCommand = castCommand;
    }
  }

  if (nullptr == maxCastCommand) {
    return -1;
  }


  long long int id = -1;
  CastCommand::Result result;
  maxCastCommand->execute(self, result);

  action = result.action;
  return result.id;
}


#ifdef ENABLE_VISUALIZATOR
void CommandStrategy::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  for (const auto& command : moveCommands) {
    command->visualization(self, visualizator);
  }

  for (const auto& command : attackCommands) {
    command->visualization(self, visualizator);
  }
}
#endif
