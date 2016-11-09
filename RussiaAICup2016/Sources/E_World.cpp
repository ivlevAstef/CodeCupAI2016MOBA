//
//File: E_World.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "E_World.h"
#include "E_Graph.h"
#include "E_Game.h"
#include "C_Extensions.h"

using namespace AICup;

World::World(): isInitialized(false) {

}

void World::update(const model::World& world) {
  modelWorld = &world;

  if (!isInitialized) {
    init();
    isInitialized = true;
  }

  updateSupposedData();
  recalculateLinePositions();
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

void World::init() {
  const auto faction = model().getMyPlayer().getFaction();

  if (model::FACTION_ACADEMY == faction) {
    topLinePosition = Graph::instance().position(Graph::ACADEMY_TOP_FIRST_TOWER);
    middleLinePosition = Graph::instance().position(Graph::ACADEMY_MIDDLE_FIRST_TOWER);
    bottomLinePosition = Graph::instance().position(Graph::ACADEMY_BOTTOM_FIRST_TOWER);
  } else if (model::FACTION_RENEGADES == faction) {
    topLinePosition = Graph::instance().position(Graph::RENEGADES_TOP_FIRST_TOWER);
    middleLinePosition = Graph::instance().position(Graph::RENEGADES_MIDDLE_FIRST_TOWER);
    bottomLinePosition = Graph::instance().position(Graph::RENEGADES_BOTTOM_FIRST_TOWER);
  } else {
    assert(false && "player not have faction??");
  }
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

void World::recalculateLinePositions() {
  const auto& minions = model().getMinions();

  const auto faction = model().getMyPlayer().getFaction();

  std::vector<model::Minion> myMinions;
  myMinions.reserve(minions.size());

  std::vector<model::Minion> enemyMinions;
  enemyMinions.reserve(minions.size());


  /// separate minions
  for (const auto& minion : minions) {
    const auto& mFaction = minion.getFaction();
    if (mFaction == model::FACTION_ACADEMY || mFaction == model::FACTION_RENEGADES) {
      if (mFaction == faction) {
        myMinions.push_back(minion);
      } else {
        enemyMinions.push_back(minion);
      }
    }
  }


  /// find all hassles
  std::vector<Position> hassles;
  hassles.reserve(myMinions.size());

  const double attackDistance = MAX(
    Game::instance().model().getFetishBlowdartAttackRange(),
    Game::instance().model().getOrcWoodcutterAttackRange()
  );


  for (const auto& myMinion : myMinions) {
    for (const auto& enemyMinion : enemyMinions) {
      if (myMinion.getDistanceTo(enemyMinion) < attackDistance) {
        hassles.push_back(Position(myMinion.getX(), myMinion.getY()));
      }
    }
  }

  /// calculate hassles count
  std::vector<std::vector<Position>> hassleGroups;
  hassleGroups.resize(hassles.size());
  const double attackDistance2 = attackDistance * attackDistance;

  for (size_t index = 0; index < hassles.size(); index++) {
    const auto& cHassle = hassles[index];
    for (const auto& hassle : hassles) {
      if ((hassle - cHassle).length2() < attackDistance2) {
        hassleGroups[index].push_back(hassle);
      }
    }
  }

  /// calculate line centers
  size_t maxCounts[3] = {1, 1, 1};

  for (size_t index = 0; index < hassles.size(); index++) {
    const auto& group = hassleGroups[index];

    Position center(0, 0);
    for (const auto& hassle : group) {
      center += hassle;
    }
    center /= group.size();


    double delta = (model().getWidth() - center.x) - center.y;

    // center
    size_t* maxCount = &maxCounts[1];
    Position* position = &middleLinePosition;

    if (delta > 400) { // top
      maxCount = &maxCounts[0];
      position = &topLinePosition;
    } else if (delta < -400) { // bottom
      maxCount = &maxCounts[2];
      position = &bottomLinePosition;
    }

    if (*maxCount < group.size()) {
      *position = center;
      *maxCount = group.size();
    }
  }

}

const Position& World::linePosition(model::LineType line) const {
  switch (line) {
    case model::LINE_TOP:
      return topLinePosition;
    case model::LINE_MIDDLE:
      return middleLinePosition;
    case model::LINE_BOTTOM:
      return bottomLinePosition;
    default:
      assert(false);
  }
  return middleLinePosition;
}


#ifdef ENABLE_VISUALIZATOR
void World::visualization(const Visualizator& visualizator) const {
  visualizator.circle(topLinePosition.x, topLinePosition.y, 100, 0xff0000);
  visualizator.circle(middleLinePosition.x, middleLinePosition.y, 100, 0xff0000);
  visualizator.circle(bottomLinePosition.x, bottomLinePosition.y, 100, 0xff0000);
}
#endif // ENABLE_VISUALIZATOR
