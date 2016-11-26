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
  isInitial = true;
}

void World::update(const model::World& world) {
  modelWorld = &world;

  if (isInitial) {
    initTrees();
    initBuildings();
    isInitial = false;
  }

  updateVisionZone();
  updateSupposedData();
  updateMinions();
}

const std::vector<model::Tree>& World::trees() const {
  return supposedTrees;
}

const std::vector<model::Building>& World::buildings() const {
  return supposedBuilding;
}

const std::vector<model::Minion>& World::minions() const {
  return realMinions;
}

const std::vector<model::Wizard>& World::wizards() const {
  return modelWorld->getWizards();
}

const std::vector<Looking>& World::getVisionZone() const {
  return visionZone;
}

void World::initBuildings() {
  for (const auto& build : model().getBuildings()) {
    if (build.getFaction() == Game::friendFaction()) {
      const Position pos = Position(size() - build.getX(), size() - build.getY());

      if (build.getType() == model::BUILDING_FACTION_BASE) {
        supposedBuilding.push_back(BaseBuilding(pos.x, pos.y, Game::enemyFaction()));
      } else {
        supposedBuilding.push_back(TowerBuilding(pos.x, pos.y, Game::enemyFaction()));
      }
    }
  }
}

void World::initTrees() {
  for (int x = 400; x <= size() - 400; x += 100) {
    for (int y = 400; y <= size() - 400; y += 100) {
      if (-400 < (x - y) && (x - y) < 400) {
        continue;
      }

      if (-400 < ((size() -x) - y) && ((size() -x) - y) < 400) {
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

void World::updateMinions() {
  realMinions.clear();
  const auto& worldMinions = modelWorld->getMinions();
  realMinions.reserve(worldMinions.size());

  for (const auto& minion : worldMinions) {
    if (model::FACTION_NEUTRAL == minion.getFaction() && !checkMinionOnNeutral(minion)) {
      hateNeuralMinions.insert(minion.getId());
    }

    if (0 != hateNeuralMinions.count(minion.getId())) {
      realMinions.push_back(Minion(minion, Game::enemyFaction()));
      continue;
    }

    realMinions.push_back(minion);
  }

}

void World::updateSupposedData() {
  supposedTrees = merge(supposedTrees, updateRadius<model::Tree, Tree>(modelWorld->getTrees()), visionZone);

  supposedBuilding = updateBuildingTicks(supposedBuilding);
  supposedBuilding = merge(supposedBuilding, updateRadius<model::Building, Building>(modelWorld->getBuildings()), visionZone);
}


const model::LaneType World::positionToLine(const double x, const double y) const {
  double delta = (size() - x) - y;

  if (delta > size()/3.3) { // top
    return model::LANE_TOP;
  } else if (delta < -size() / 3.3) {
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


const model::CircularUnit* World::unitOrProjectile(long long id) const {
  const auto livingUnit = unit(id);
  if (nullptr != livingUnit) {
    return livingUnit;
  }

  for (const auto& projectile : model().getProjectiles()) {
    if (projectile.getId() == id) {
      return &projectile;
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

bool World::checkMinionOnNeutral(const model::Minion& neutral) const {
  assert(neutral.getFaction() == model::FACTION_NEUTRAL);

  if (0 != neutral.getSpeedX() || 0 != neutral.getSpeedY()) {
    return false;
  }

  if (0 != neutral.getRemainingActionCooldownTicks()) {
    return false;
  }

  if (neutral.getLife() != neutral.getMaxLife()) {
    return false;
  }

  for (const auto& projectile : modelWorld->getProjectiles()) {
    if (projectile.getOwnerUnitId() == neutral.getId()) {
      return false;
    }
  }

  return true;
}

#ifdef ENABLE_VISUALIZATOR
void World::visualization(const Visualizator& visualizator) const {
  if (Visualizator::PRE == visualizator.getStyle()) {
    for (const auto& build : supposedBuilding) {
      visualizator.fillCircle(build.getX(), build.getY(), build.getRadius(), 0xffff00);
    }
  }

  //if (Visualizator::POST == visualizator.getStyle()) {
  //  for (const auto& tree : supposedTrees) {
  //    visualizator.fillCircle(tree.getX(), tree.getY(), tree.getRadius(), 0xff7700);
  //  }
  //}

}
#endif // ENABLE_VISUALIZATOR
