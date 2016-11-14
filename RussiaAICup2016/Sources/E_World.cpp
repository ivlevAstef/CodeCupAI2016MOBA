//
//File: E_World.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "E_World.h"
#include "E_Graph.h"
#include "E_Game.h"
#include "C_Math.h"

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
  /// а мы всегда одна фракция... 
  topLinePosition = Graph::instance().position(Graph::TOP_CENTER);
  middleLinePosition = Graph::instance().position(Graph::CENTER_ACADEMY);
  bottomLinePosition = Graph::instance().position(Graph::BOTTOM_CENTER);
}

template<typename Type>
static std::vector<Type> merge(const std::vector<Type>& supposed, const std::vector<Type>& real) {
  //static_assert(std::tr1::is_base_of<model::CircularUnit, Type>::value, "Type not derived from CircularUnit");


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

const Position& World::linePosition(model::LaneType line) const {
  switch (line) {
    case model::LANE_TOP:
      return topLinePosition;
    case model::LANE_MIDDLE:
      return middleLinePosition;
    case model::LANE_BOTTOM:
      return bottomLinePosition;
    default:
      assert(false);
  }
  return middleLinePosition;
}

Obstacles World::obstacles(const model::Wizard& unit) const {
  Obstacles aroundObstacles;
  aroundObstacles.reserve(24); // Приблизительно сколько объектов вокруг мага в среднем

  for (const auto& tree : model().getTrees()) {
    if (unit.getDistanceTo(tree) < unit.getVisionRange()) {
      aroundObstacles.push_back(tree);
    }
  }

  for (const auto& minion : model().getMinions()) {
    if (unit.getDistanceTo(minion) < unit.getVisionRange()) {
      aroundObstacles.push_back(minion);
    }
  }

  for (const auto& build : model().getBuildings()) {
    if (unit.getDistanceTo(build) < unit.getVisionRange()) {
      aroundObstacles.push_back(build);
    }
  }

  for (const auto& wizard : model().getWizards()) {
    if (unit.getId() != wizard.getId() && unit.getDistanceTo(wizard) < unit.getVisionRange()) {
      aroundObstacles.push_back(wizard);
    }
  }

  //after model().getProjectiles()

  return aroundObstacles;
}

ObstaclesGroups World::obstaclesGroup(const model::Wizard& unit) const {
  const auto aroundObstacles = obstacles(unit);

  ObstaclesGroups result;

  std::vector<size_t> groupsForMerge;
  for (const auto& obstacle : aroundObstacles) {
    groupsForMerge.clear();

    for (size_t index = 0; index < result.size(); ++index) {
      for (const auto& obstacleInGroup : result[index]) {
        if (obstacle.getDistanceTo(obstacleInGroup) < obstacle.getRadius() + obstacleInGroup.getRadius() + unit.getRadius()*2) {
          groupsForMerge.push_back(index);
          break;
        }
      }
    }

    /// create new group
    if (0 == groupsForMerge.size()) {
      result.push_back({obstacle});
    } else {
      auto& firstGroup = result[groupsForMerge[0]];
      /// merge
      for (size_t index = 1; index < groupsForMerge.size(); ++index) {
        const auto& iterGroup = result[groupsForMerge[index]];
        firstGroup.insert(firstGroup.end(), iterGroup.begin(), iterGroup.end());
      }

      /// remove groups
      for (size_t index = groupsForMerge.size() - 1; index > 0; index--) {
        result.erase(result.begin() + groupsForMerge[index]);
      }

      /// add
      firstGroup.push_back(obstacle);
    }
  }

  return result;
}

const model::LivingUnit* World::unit(long long id) const {
  for (const auto& wizard : model().getWizards()) {
    if (wizard.getId() == id) {
      return &wizard;
    }
  }

  for (const auto& minion : model().getMinions()) {
    if (minion.getId() == id) {
      return &minion;
    }
  }

  for (const auto& build : model().getBuildings()) {
    if (build.getId() == id) {
      return &build;
    }
  }

  return nullptr;
}

std::vector<const model::LivingUnit*> World::around(const model::Wizard& unit, const model::Faction faction) const {
  std::vector<const model::LivingUnit*> result;

  const auto unitPos = Position(unit.getX(), unit.getY());
  const auto radius2 = unit.getVisionRange() * unit.getVisionRange();

  for (const auto& wizard : model().getWizards()) {
    if (wizard.getFaction() == faction && wizard.getId() != unit.getId()) {
      const auto wizardPos = Position(wizard.getX(), wizard.getY());
      if ((wizardPos - unitPos).length2() < radius2) {
        result.push_back(&wizard);
      }
    }
  }

  for (const auto& minion : model().getMinions()) {
    if (minion.getFaction() == faction) {
      const auto minionPos = Position(minion.getX(), minion.getY());
      if ((minionPos - unitPos).length2() < radius2) {
        result.push_back(&minion);
      }
    }
  }

  for (const auto& build : model().getBuildings()) {
    if (build.getFaction() == faction) {
      const auto buildPos = Position(build.getX(), build.getY());
      if ((buildPos - unitPos).length2() < radius2) {
        result.push_back(&build);
      }
    }
  }

  return result;
}

std::vector<const model::LivingUnit*> World::aroundEnemies(const model::Wizard& unit) const {
  return around(unit, Game::instance().enemyFaction(unit.getFaction()));
}

#ifdef ENABLE_VISUALIZATOR
void World::visualization(const Visualizator& visualizator) const {
  visualizator.circle(topLinePosition.x, topLinePosition.y, 200, 0xff0000);
  visualizator.circle(middleLinePosition.x, middleLinePosition.y, 200, 0xff0000);
  visualizator.circle(bottomLinePosition.x, bottomLinePosition.y, 200, 0xff0000);
}
#endif // ENABLE_VISUALIZATOR
