//
//File: CM_CommandManager.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandManager.h"

#ifdef ENABLE_TESTS
#include "CM_TestMoveStrategy.h"
#include "CM_TestMoveAndAttackStrategy.h"
#include "CM_TestFollowStrategy.h"
#endif
#include "CM_FirstStrategy.h"

using namespace AICup;

CommandManager::CommandManager() {
  currentStrategy = std::make_shared<FirstStrategy>(fabric);
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