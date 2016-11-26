//
//File: E_World.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "E_World.h"
#include "E_Points.h"
#include "E_Game.h"
#include "C_Math.h"
#include "E_WorldObjects.h"

using namespace AICup;

World::World() {
  initTrees();
  initBuildings();
}

void World::update(const model::World& world) {
  modelWorld = &world;

  updateVisionZone();
  updateSupposedData();
}

const std::vector<model::Tree>& World::trees() const {
  return supposedTrees;
}

const std::vector<model::Building>& World::buildings() const {
  return supposedBuilding;
}

const std::vector<model::Minion>& World::minions() const {
  return modelWorld->getMinions();
}

const std::vector<model::Wizard>& World::wizards() const {
  return modelWorld->getWizards();
}

const std::vector<Looking>& World::getVisionZone() const {
  return visionZone;
}

void World::initBuildings() {
  supposedBuilding.push_back(BaseBuilding(3600, 400, Game::enemyFaction()));

  // Top
  supposedBuilding.push_back(TowerBuilding(3650, 2344, Game::enemyFaction()));
  supposedBuilding.push_back(TowerBuilding(3950, 1307, Game::enemyFaction()));
  // Middle
  supposedBuilding.push_back(TowerBuilding(2071, 1600, Game::enemyFaction()));
  supposedBuilding.push_back(TowerBuilding(3095, 1232, Game::enemyFaction()));
  // Bottom
  supposedBuilding.push_back(TowerBuilding(1688, 50, Game::enemyFaction()));
  supposedBuilding.push_back(TowerBuilding(2630, 350, Game::enemyFaction()));
}

void World::initTrees() {
  for (int x = 400; x <= 4000 - 400; x += 100) {
    for (int y = 400; y <= 4000 - 400; y += 100) {
      if (-400 < (x - y) && (x - y) < 400) {
        continue;
      }

      if (-400 < ((4000-x) - y) && ((4000-x) - y) < 400) {
        continue;
      }

      supposedTrees.push_back(Tree(x, y, 50));
    }
  }

}

void World::updateVisionZone() {
  visionZone.clear();

  for (const auto& wizard : model().getWizards()) {
    if (Game::friendFaction() == wizard.getFaction()) {
      visionZone.emplace_back(wizard);
    }
  }

  for (const auto& minion : model().getMinions()) {
    if (Game::friendFaction() == minion.getFaction()) {
      visionZone.emplace_back(minion);
    }
  }

  for (const auto& building : model().getBuildings()) {
    if (Game::friendFaction() == building.getFaction()) {
      visionZone.emplace_back(building);
    }
  }
}

template<typename Type>
static std::vector<Type> merge(const std::vector<Type>& supposed, const std::vector<Type>& real, const std::vector<Looking> visionZone) {
  //static_assert(std::tr1::is_base_of<model::CircularUnit, Type>::value, "Type not derived from CircularUnit");
  std::vector<Type> result;

  for (const auto& sIter : supposed) {
    bool found = false;
    for (const auto& vision : visionZone) {
      const double dx = sIter.getX() - vision.getX();
      const double dy = sIter.getY() - vision.getY();
      if (sqrt(dx*dx + dy*dy) < vision.getVisionRange()) {
        found = true;
        break;
      }
    }

    if (!found) {
      result.push_back(sIter);
    }
  }

  for (const auto& rIter : real) {
    result.push_back(rIter);
  }

  return result;
}


template<typename Type, typename CreateType>
static std::vector<Type> updateRadius(const std::vector<Type>& real) {
  std::vector<Type> result;
  result.reserve(real.size());

  for (const auto& obj : real) {
    result.push_back(CreateType(obj, obj.getRadius() + 0.1));
  }

  return result;
}

static std::vector<model::Building> updateBuildingTicks(const std::vector<model::Building>& real) {
  std::vector<model::Building> result;
  result.reserve(real.size());

  for (const auto& obj : real) {
    result.push_back(Building(obj, MAX(0, obj.getRemainingActionCooldownTicks() - 1)));
  }

  return result;
}


void World::updateSupposedData() {
  supposedBuilding = updateBuildingTicks(supposedBuilding);

  supposedTrees = merge(supposedTrees, updateRadius<model::Tree, Tree>(modelWorld->getTrees()), visionZone);
  supposedBuilding = merge(supposedBuilding, updateRadius<model::Building, Building>(modelWorld->getBuildings()), visionZone);
}


const model::LaneType World::positionToLine(const double x, const double y) const {
  double delta = (model().getWidth() - x) - y;

  if (delta > 1200) { // top
    return model::LANE_TOP;
  } else if (delta < -1200) {
    return model::LANE_BOTTOM;
  }

  return model::LANE_MIDDLE;
}

const int World::wizardCount(model::LaneType line) const {
  int result = 0;
  for (const auto& wizard : model().getWizards()) {
    if (Game::friendFaction() == wizard.getFaction() && line == positionToLine(wizard.getX(), wizard.getY())) {
      result++;
    }
  }

  return result;
}

const int World::wizardCount(model::LaneType line, const model::Wizard& excludeWizard) const {
  int result = 0;
  for (const auto& wizard : model().getWizards()) {
    if (wizard.getId() != excludeWizard.getId()
      && Game::friendFaction() == wizard.getFaction() && line == positionToLine(wizard.getX(), wizard.getY())) {
      result++;
    }
  }

  return result;
}

Obstacles World::allObstacles(const model::CircularUnit& unit, const bool onlyStatic) const {
  Obstacles obstacles;
  obstacles.reserve(500); // Приблизительно сколько объектов в среднем

  for (const auto& tree : trees()) {
    obstacles.push_back(&tree);
  }

  for (const auto& build : buildings()) {
    obstacles.push_back(&build);
  }

  for (const auto& minion : model().getMinions()) {
    if (unit.getId() != minion.getId() && (!onlyStatic || model::FACTION_NEUTRAL == minion.getFaction())) {
      obstacles.push_back(&minion);
    }
  }

  if (!onlyStatic) {
    for (const auto& wizard : model().getWizards()) {
      if (unit.getId() != wizard.getId()) {
        obstacles.push_back(&wizard);
      }
    }
  }

  return obstacles;
}

Obstacles World::obstacles(const model::CircularUnit& unit, const double range) const {
  Obstacles aroundObstacles;
  aroundObstacles.reserve(64); // Приблизительно сколько объектов в среднем

  for (const auto& tree : trees()) {
    if (unit.getDistanceTo(tree) < range) {
      aroundObstacles.push_back(&tree);
    }
  }

  for (const auto& build : buildings()) {
    if (unit.getDistanceTo(build) < range) {
      aroundObstacles.push_back(&build);
    }
  }

  for (const auto& minion : model().getMinions()) {
    if (unit.getId() != minion.getId() && unit.getDistanceTo(minion) < range) {
      aroundObstacles.push_back(&minion);
    }
  }

  for (const auto& wizard : model().getWizards()) {
    if (unit.getId() != wizard.getId() && unit.getDistanceTo(wizard) < range) {
      aroundObstacles.push_back(&wizard);
    }
  }

  return aroundObstacles;
}


ObstaclesGroups World::createGroup(const Obstacles& obstacles, const double radius) const {
  ObstaclesGroups result;

  std::vector<size_t> groupsForMerge;
  for (const auto& obstacle : obstacles) {
    groupsForMerge.clear();

    for (size_t index = 0; index < result.size(); ++index) {
      for (const auto& obstacleInGroup : result[index]) {
        if (obstacle->getDistanceTo(*obstacleInGroup) < obstacle->getRadius() + obstacleInGroup->getRadius() + radius * 2) {
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

  for (const auto& tree : trees()) {
    if (tree.getId() == id) {
      return &tree;
    }
  }

  for (const auto& build : buildings()) {
    if (build.getId() == id) {
      return &build;
    }
  }

  return nullptr;
}

std::vector<const model::LivingUnit*> World::around(const model::Wizard& unit, const model::Faction faction, double radius) const {
  std::vector<const model::LivingUnit*> result;

  const auto unitPos = Position(unit.getX(), unit.getY());
  radius = (radius < 0) ? unit.getVisionRange() : radius;
  const auto radius2 = radius * radius;

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

  for (const auto& build : buildings()) {
    if (build.getFaction() == faction) {
      const auto buildPos = Position(build.getX(), build.getY());
      if ((buildPos - unitPos).length2() < radius2) {
        result.push_back(&build);
      }
    }
  }

  return result;
}

std::vector<const model::LivingUnit*> World::aroundEnemies(const model::Wizard& unit, const double radius) const {
  return around(unit, Game::reverseFaction(unit.getFaction()), radius);
}

#ifdef ENABLE_VISUALIZATOR
void World::visualization(const Visualizator& visualizator) const {
  //if (Visualizator::PRE == visualizator.getStyle()) {
  //  visualizator.circle(topLinePosition.x, topLinePosition.y, 200, 0xff0000);
  //  visualizator.circle(middleLinePosition.x, middleLinePosition.y, 200, 0xff0000);
  //  visualizator.circle(bottomLinePosition.x, bottomLinePosition.y, 200, 0xff0000);

  //  for (const auto& build : supposedBuilding) {
  //    visualizator.fillCircle(build.getX(), build.getY(), build.getRadius(), 0xffff00);
  //  }
  //}

  //if (Visualizator::POST == visualizator.getStyle()) {
  //  for (const auto& tree : supposedTrees) {
  //    visualizator.fillCircle(tree.getX(), tree.getY(), tree.getRadius(), 0xff7700);
  //  }
  //}

}
#endif // ENABLE_VISUALIZATOR
