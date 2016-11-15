#include "CM_FirstStrategy.h"
#include "E_World.h"
#include "E_Graph.h"
#include "C_Math.h"

using namespace AICup;

FirstStrategy::FirstStrategy(const CommandFabric& fabric) : CommandStategy(fabric) {
  isInitialized = false;
}

void FirstStrategy::init(const model::Wizard& self, model::Move& move) {
  // вначале линию выбираем также как в smart game, так как это даст скорей всего наименьшее количество коллизий
  switch ((int)self.getId()) {
    case 1:
    case 2:
    case 6:
    case 7:
      myLine = model::LANE_TOP;
      break;
    case 3:
    case 8:
      myLine = model::LANE_MIDDLE;
      break;
    case 4:
    case 5:
    case 9:
    case 10:
      myLine = model::LANE_BOTTOM;
      break;
    default:
      myLine = model::LANE_MIDDLE;
  }
}

void FirstStrategy::update(const model::Wizard& self, model::Move& move) {
  /// если мы умерали и воскресли, то подумать снова куда лучше пойти
  if (World::instance().model().getTickIndex() - lastControlTick >= 1200) {
    changeLane();
  }

  lastControlTick = World::instance().model().getTickIndex();

  if (!isInitialized) {
    init(self, move);
    isInitialized = true;

    myLine = model::LANE_MIDDLE;
  }

  usedCommands.clear();

  const auto moveToLineCommand = fabric.moveToLine(myLine);
  if (moveToLineCommand->check(self)) {
    usedCommands.push_back(moveToLineCommand);
  }

  const auto avoidAroundCommand = fabric.avoidAround();
  bool needAvoidAround = avoidAroundCommand->check(self);

  if (needAvoidAround && self.getLife() < 50) {
    usedCommands.push_back(avoidAroundCommand);
  } else {
    const auto getExpirienceCommand = fabric.moveGetExpirience();
    if (getExpirienceCommand->check(self)) {
      usedCommands.push_back(getExpirienceCommand);
    } else if (needAvoidAround) {
      usedCommands.push_back(avoidAroundCommand);
    }
  }

  ///

  const auto attackCommand = createMaxPriorityAttackEnemy(self);
  if (nullptr != attackCommand.get()) {
    usedCommands.push_back(attackCommand);
  }

  if (nullptr == moveToBonus.get()) {
    const auto newMoveToBonus = fabric.moveToBonus();
    if (newMoveToBonus->check(self)) {
      moveToBonus = newMoveToBonus;
    }
  } else if (moveToBonus->check(self)) {
    usedCommands.push_back(moveToBonus);

    changeLane();
  } else {
    moveToBonus = nullptr;
  }

  /*
  как должно быть:
  1)!! пока нету крипов, иследуем карту и бъем нейтралов, для получения опыта и очков
  3)!! при моменте появления руны (надо еще написать эту команду), идем к ней в двух случаях:
     а) это выгодно по xp (руна близко, и тут много не заработаешь)
     б) идем на ту линию где ситуация не очень
     с) идем на ту линию где своих меньше (дабы побольше оттяпать)
 */

  for (const auto& command : usedCommands) {
    command->execute(self, move);
  }
}

void FirstStrategy::changeLane() {
  const auto basePosition = Graph::instance().position(Graph::ACADEMY_BASE);

  double topLength = 0;
  double middleLength = 0;
  double bottomLength = 0;

  Graph::instance().path(basePosition, World::instance().linePosition(model::LANE_TOP), topLength);
  Graph::instance().path(basePosition, World::instance().linePosition(model::LANE_MIDDLE), middleLength);
  Graph::instance().path(basePosition, World::instance().linePosition(model::LANE_BOTTOM), bottomLength);

  double min = MIN(topLength, MIN(middleLength, bottomLength));
  double max = MAX(topLength, MAX(middleLength, bottomLength));

  /// если линии сильно не отличаються то не будем менять
  if (max - min < 1000) {
    return;
  }

  if (topLength < middleLength && topLength < bottomLength) {
    myLine = model::LANE_TOP;
  } else if (middleLength < topLength && middleLength < bottomLength) {
    myLine = model::LANE_MIDDLE;
  } else {
    myLine = model::LANE_BOTTOM;
  }
}

CommandPtr FirstStrategy::createMaxPriorityAttackEnemy(const model::Wizard& self) {
  CommandPtr maxPriorityAttackCommand = nullptr;
  int maxPriority = -1;
  for (const auto& enemy : World::instance().aroundEnemies(self)) {
    const auto attackCommand = fabric.attack(enemy->getId());
    if (attackCommand->check(self)) {
      const int priority = attackCommand->priority(self);
      if (priority > maxPriority) {
        maxPriorityAttackCommand = attackCommand;
        maxPriority = priority;
      }
    }
  }

  return maxPriorityAttackCommand;
}

#ifdef ENABLE_VISUALIZATOR
void FirstStrategy::visualization(const Visualizator& visualizator) const {
  for (const auto& command : usedCommands) {
    command->visualization(visualizator);
  }
}

#endif