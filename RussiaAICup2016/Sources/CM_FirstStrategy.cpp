#include "CM_FirstStrategy.h"
#include "E_World.h"

using namespace AICup;

FirstStrategy::FirstStrategy(const CommandFabric& fabric) : CommandStategy(fabric) {
  isInitialized = false;
}

void FirstStrategy::init(const model::Wizard& self, model::Move& move) {
  // ������� ����� �������� ����� ��� � smart game, ��� ��� ��� ���� ������ ����� ���������� ���������� ��������
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

  ///

  const auto attackCommand = createMaxPriorityAttackEnemy(self);
  if (nullptr != attackCommand.get()) {
    usedCommands.push_back(attackCommand);
  }

  /*
  ��� ������ ����:
  1)!! ���� ���� ������, �������� ����� � ���� ���������, ��� ��������� ����� � �����
  2) ����� ��� ��������, ����� �� ��� ����� ��� ���� ����� ���� ���������:
     �) ������� ����� ������������ ����, ���� ����� ����, �� ��������� � ����� �����
     �) ���� ���� ����, �� ������� �
     �)!! ���� ������ ��� ���� ������� ��������� ��� ���� (���� �������� ����� �������), �� �������� �� ��� ������ (follow ����� ������������) - ���������
  3)!! ��� ������� ��������� ���� (���� ��� �������� ��� �������), ���� � ��� � ���� �������:
     �) ��� ������� �� xp (���� ������, � ��� ����� �� �����������)
     �) ���� �� �� ����� ��� �������� �� �����
     �) ���� �� �� ����� ��� ����� ������ (���� �������� ��������)
  4)
 */

  for (const auto& command : usedCommands) {
    command->execute(self, move);
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