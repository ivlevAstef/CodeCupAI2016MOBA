//
//File: CM_CommandManager.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandManager.h"

using namespace AICup;

CommandManager::CommandManager() {
  commands.push_back(fabric.moveToPoint(1200, 1200));
  //commands.push_back(fabric.moveToLine(model::LINE_BOTTOM));
}

void CommandManager::update(const model::Wizard& self, model::Move& move) {
  for (const auto& command : commands) {
    if (command->check(self, move)) {
      command->execute(self, move);
    }
  }
}

#ifdef ENABLE_VISUALIZATOR
void CommandManager::visualization(const Visualizator& visualizator) const {
  for (const auto& command : commands) {
    command->visualization(visualizator);
  }

}
#endif // ENABLE_VISUALIZATOR
