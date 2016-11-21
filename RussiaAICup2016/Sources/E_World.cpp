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
  initLinePosition();
  initTrees();
}

void World::update(const model::World& world) {
  modelWorld = &world;

  updateVisionZone();
  updateSupposedData();
  recalculateLinePositions();
}

const std::vector<model::Tree>& World::trees() const {
  return supposedTrees;
}

const std::vector<model::Building>& World::buildings() const {
  return supposedBuilding;
}

const std::vector<model::Wizard>& World::wizards() const {
  return supposedWizards;
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

void World::initLinePosition() {
  /// а мы всегда одна фракция...
  topLinePosition = Points::point(Points::TOP_CENTER);
  middleLinePosition = Points::point(Points::CENTER_ACADEMY);
  bottomLinePosition = Points::point(Points::BOTTOM_CENTER);
}

template<typename Type, typename CreateType>
static std::vector<Type> updateRadius(const std::vector<Type>& real) {
  std::vector<Type> result;
  result.reserve(real.size());

  for (const auto& obj : real) {
    result.push_back(CreateType(obj, obj.getRadius() + 0.5));
  }

  return result;
}

template<typename Type>
static std::vector<Type> merge(const std::vector<Type>& supposed, const std::vector<Type>& real, const std::vector<Looking> visionZone) {
  //static_assert(std::tr1::is_base_of<model::CircularUnit, Type>::value, "Type not derived from CircularUnit");
  std::vector<Type> result;

  for (const auto& sIter : supposed) {
    bool found = false;
    for (const auto& vision : visionZone) {
      double dx = sIter.getX() - vision.getX();
      double dy = sIter.getY() - vision.getY();
      if (sqrt(dx*dx + dy*dy) < vision.getVisionRange() + sIter.getRadius()) {
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

void World::updateVisionZone() {
  visionZone.clear();

  for (const auto& wizard : model().getWizards()) {
    if (model::FACTION_ACADEMY == wizard.getFaction()) {
      visionZone.emplace_back(wizard);
    }
  }

  for (const auto& minion : model().getMinions()) {
    if (model::FACTION_ACADEMY == minion.getFaction()) {
      visionZone.emplace_back(minion);
    }
  }

  for (const auto& building : model().getBuildings()) {
    if (model::FACTION_ACADEMY == building.getFaction()) {
      visionZone.emplace_back(building);
    }
  }
}

void World::updateSupposedData() {

  supposedTrees = merge(supposedTrees, updateRadius<model::Tree, Tree>(modelWorld->getTrees()), visionZone);
  supposedWizards = modelWorld->getWizards();
  supposedBuilding = updateRadius<model::Building, Building>(modelWorld->getBuildings());
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


    const auto line = positionToLine(center.x, center.y);

    // center
    size_t* maxCount = &maxCounts[1];
    Position* position = &middleLinePosition;

    if (line == model::LANE_TOP) {
      maxCount = &maxCounts[0];
      position = &topLinePosition;
    } else if (line == model::LANE_BOTTOM) {
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
    if (model::FACTION_ACADEMY == wizard.getFaction() && line == positionToLine(wizard.getX(), wizard.getY())) {
      result++;
    }
  }

  return result;
}

const int World::wizardCount(model::LaneType line, const model::Wizard& excludeWizard) const {
  int result = 0;
  for (const auto& wizard : model().getWizards()) {
    if (wizard.getId() != excludeWizard.getId()
      && model::FACTION_ACADEMY == wizard.getFaction() && line == positionToLine(wizard.getX(), wizard.getY())) {
      result++;
    }
  }

  return result;
}

Obstacles World::obstacles(const model::CircularUnit& unit, const double range, const bool onlyStatic) const {
  Obstacles aroundObstacles;
  aroundObstacles.reserve(64); // Приблизительно сколько объектов в среднем

  for (const auto& tree : trees()) {
    if (unit.getDistanceTo(tree) < range) {
      aroundObstacles.push_back(tree);
    }
  }

  for (const auto& build : buildings()) {
    if (unit.getDistanceTo(build) < range) {
      aroundObstacles.push_back(build);
    }
  }

  if (!onlyStatic) {
    for (const auto& minion : model().getMinions()) {
      if (unit.getId() != minion.getId() && unit.getDistanceTo(minion) < range) {
        aroundObstacles.push_back(minion);
      }
    }

    for (const auto& wizard : model().getWizards()) {
      if (unit.getId() != wizard.getId() && unit.getDistanceTo(wizard) < range) {
        aroundObstacles.push_back(wizard);
      }
    }
  }

  return aroundObstacles;
}

ObstaclesGroups World::obstaclesGroup(const model::CircularUnit& unit, const double range, const bool onlyStatic) const {
  const auto aroundObstacles = obstacles(unit, range, onlyStatic);

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
  return around(unit, Game::instance().enemyFaction(unit.getFaction()), radius);
}

#ifdef ENABLE_VISUALIZATOR
void World::visualization(const Visualizator& visualizator) const {
  visualizator.circle(topLinePosition.x, topLinePosition.y, 200, 0xff0000);
  visualizator.circle(middleLinePosition.x, middleLinePosition.y, 200, 0xff0000);
  visualizator.circle(bottomLinePosition.x, bottomLinePosition.y, 200, 0xff0000);
}
#endif // ENABLE_VISUALIZATOR
