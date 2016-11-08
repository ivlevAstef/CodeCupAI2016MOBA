//
//File: CM_CommandManager.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandManager.h"

#include "CM_Move.h"

using namespace AICup;

std::vector<Position> _path;

void CommandManager::update(const model::Wizard& self, model::Move& move) {
  _path = path(Position(self.getX(), self.getY()), Position(1200, 200));
}

#ifdef ENABLE_VISUALIZATOR
void CommandManager::visualization(const Visualizator& visualizator) {
  for (size_t index = 1; index < _path.size(); index++) {
    visualizator.line(_path[index - 1].x, _path[index - 1].y, _path[index].x, _path[index].y, 0xff0000);
  }

}
#endif // ENABLE_VISUALIZATOR
