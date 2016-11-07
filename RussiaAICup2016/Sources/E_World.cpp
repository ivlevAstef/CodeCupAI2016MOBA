//
//File: E_World.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "E_World.h"
#include <type_traits>

using namespace AICup;

void World::update(const model::World& world) {
  modelWorld = &world;

  updateSupposedData();
}

const model::World& AICup::World::model() const {
  return *modelWorld;
}

std::vector<model::Tree>& AICup::World::trees() {
  return supposedTrees;
}

std::vector<model::Wizard>& AICup::World::wizards() {
  return supposedWizards;
}

template<typename Type>
static std::vector<Type> merge(const std::vector<Type>& supposed, const std::vector<Type>& real) {
  static_assert(std::tr1::is_base_of<model::CircularUnit, Type>::value, "Type not derived from CircularUnit");


  //TODO: need merge
  return real;
}

void World::updateSupposedData() {
  supposedTrees = merge(supposedTrees, modelWorld->getTrees());
  supposedWizards = merge(supposedWizards, modelWorld->getWizards());
}

Position World::linePosition(model::LineType line) {
  return Position(0, 0);
}