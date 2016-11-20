#include "S_CommandStrategy.h"
#include "A_Exec.h"
#include "C_Math.h"


using namespace AICup;

CommandStategy::CommandStategy(const CommandFabric& fabric) : fabric(fabric) {
}

void CommandStategy::update(const model::Wizard& self, model::Move& finalMove) {
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

void CommandStategy::clear() {
  moveCommands.clear();
  attackCommands.clear();
}

const Vector CommandStategy::move(const model::Wizard& self, TurnStyle& turnStyle, double& speedLimit) {
  std::vector<MoveCommand::Result> moveResults;
  moveResults.resize(moveCommands.size());

  for (size_t index = 0; index < moveCommands.size(); index++) {
    moveCommands[index]->execute(self, moveResults[index]);
  }

  Vector summaryDirection;
  double summaryPriority = 0;

  int maxPriority = -10000;

  //TODO: адаптировать этот алгоритм, на более сильную формулу сложения векторов
  for (const auto& result : moveResults) {
    summaryPriority += result.priority / 1000.0;
    summaryDirection += result.moveDirection;

    if (result.priority > maxPriority) {
      turnStyle = result.turnStyle;
      maxPriority = result.priority;
    }

    if (result.speedLimit > 0 && (result.speedLimit < speedLimit || speedLimit < 0)) {
      speedLimit = result.speedLimit;
    }
  }

  //assert(summaryPriority > 0);
  return summaryDirection / MAX(1.0, summaryPriority);
}

const model::LivingUnit& CommandStategy::attack(const model::Wizard& self, model::ActionType& action) {
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
void CommandStategy::visualization(const Visualizator & visualizator) const {
  for (const auto& command : moveCommands) {
    command->visualization(visualizator);
  }

  for (const auto& command : attackCommands) {
    command->visualization(visualizator);
  }
}
#endif
