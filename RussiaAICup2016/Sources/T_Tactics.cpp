#include "T_Tactics.h"
#include "E_World.h"

using namespace AICup;

std::vector<Message> Tactics::update(const model::Wizard& self) {
  return useRush(self);
}

std::vector<Message> Tactics::useRush(const model::Wizard& self) {
  /// первых два тика шлем только информацию что хотим использовать раш
  if (World::model().getTickIndex() >= 2) {
    return{ };
  }

  return{
    Message(TacticsType::Rush),
    Message(TacticsType::Rush),
    Message(TacticsType::Rush),
    Message(TacticsType::Rush),
    Message(TacticsType::Rush),
  };
}