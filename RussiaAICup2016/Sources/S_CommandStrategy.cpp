#include "S_CommandStrategy.h"
#include "A_Exec.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include <cassert>


using namespace AICup;

CommandStrategy::CommandStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& finder):
  fabric(fabric), pathFinder(finder) {
}

void CommandStrategy::update(const model::Wizard& self, model::Move& finalMove) {
  if (!moveCommands.empty()) {
    TurnStyle turnStyle;
    double speedLimit = -1;
    const Vector direction = move(self, turnStyle, speedLimit);

    Algorithm::execMove(self, turnStyle, direction, speedLimit, finalMove);
  }

  if (!attackCommands.empty()) {
    model::ActionType action;
    const model::LivingUnit& unit = attack(self, action);

    Algorithm::execAttack(self, action, unit, finalMove);
  }
}

void CommandStrategy::clear() {
  moveCommands.clear();
  attackCommands.clear();
}

const Vector CommandStrategy::move(const model::Wizard& self, TurnStyle& turnStyle, double& speedLimit) {
  std::vector<MoveCommand::Result> moveResults;
  moveResults.resize(moveCommands.size());

  for (size_t index = 0; index < moveCommands.size(); index++) {
    moveCommands[index]->execute(self, moveResults[index]);
    assert(moveResults[index].priority > 0);
  }

  for (const auto& move : moveResults) {
    for (const auto& tree : move.treesForRemove) {
      addTreeForRemove(self, tree);
    }
  }


  int maxPriority = -10000;
  for (const auto& move : moveResults) {
    if (move.priority > maxPriority) {
      turnStyle = move.turnStyle;
      maxPriority = move.priority;
    }
  }


  double minDeviation = 9999999;
  Vector result = Vector(0, 0);
  //Проходим по углам, и высчитываем какой вариант даст наименьшое отклонение
  for (size_t alphaI = 0; alphaI < 360; alphaI++) {
    const double alpha = AICUP_PI * double(alphaI) / 180.0;

    const auto direction = Vector(1, 0).rotate(alpha);

    double summaryDeviation = 0;
    for (const auto& move : moveResults) {
      if (move.moveDirection.length() < 0.01) {
        continue;
      }
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

void CommandStrategy::addTreeForRemove(const model::Wizard& self, const model::LivingUnit* tree) {
  assert(nullptr != tree);

  const double distance = self.getDistanceTo(*tree);
  if (distance > self.getVisionRange()) {
    return;
  }

  const auto attack = fabric.attack(tree->getId());
  if (attack->check(self)) {
    attackCommands.push_back(attack);
  }
}

const model::LivingUnit& CommandStrategy::attack(const model::Wizard& self, model::ActionType& action) {
  std::vector<AttackCommand::Result> attackResults;
  attackResults.resize(attackCommands.size());

  for (size_t index = 0; index < attackCommands.size(); index++) {
    attackCommands[index]->execute(self, attackResults[index]);
  }

  const model::LivingUnit* resultUnit = nullptr;

  int maxPriority = -10000;
  //TODO: тут как миниум нужно еще учитывать тип действия
  for (const auto& result : attackResults) {
    if (result.priority > maxPriority) {
      maxPriority = result.priority;

      action = result.action;
      resultUnit = result.unit;
    }
  }


  assert(nullptr != resultUnit);
  return *resultUnit;
}

#ifdef ENABLE_VISUALIZATOR
void CommandStrategy::visualization(const Visualizator & visualizator) const {
  for (const auto& command : moveCommands) {
    command->visualization(visualizator);
  }

  for (const auto& command : attackCommands) {
    command->visualization(visualizator);
  }
}
#endif
