//
//File: CM_CommandManager.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandManager.h"

#include "CM_TestMoveStrategy.h"
#include "CM_TestFollow.h"

using namespace AICup;

CommandManager::CommandManager() {
  currentStrategy = std::make_shared<TestMoveStrategy>(fabric);
}

void CommandManager::update(const model::Wizard& self, model::Move& move) {
  if (nullptr != currentStrategy.get()) {
    currentStrategy->update(self, move);
  }
}

#ifdef ENABLE_VISUALIZATOR
void CommandManager::visualization(const Visualizator& visualizator) const {
  if (nullptr != currentStrategy.get()) {
    currentStrategy->visualization(visualizator);
  }
}
#endif // ENABLE_VISUALIZATOR