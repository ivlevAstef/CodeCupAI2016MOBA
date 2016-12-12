//
//File: E_World.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "E_World.h"
#include "E_Points.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "E_WorldObjects.h"

using namespace AICup;

/// Support
template<typename Type>
static std::vector<Type> merge(const std::vector<Type>& supposed, const std::vector<Type>& real) {
  //static_assert(std::tr1::is_base_of<model::CircularUnit, Type>::value, "Type not derived from CircularUnit");
  std::vector<Type> result;
  result.reserve(supposed.size() + real.size());

  for (const auto& sIter : supposed) {
    if (!World::instance().isInVisionZone(sIter.getX(), sIter.getY())) {
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

/// есть у такого способа один минус - если миньона убьет, кокраз в тот момент когда он подошел к вышке, то может быть кос€к
static std::vector<model::Building> updateBuildingsTicksByAttack(const std::vector<model::Building>& real) {
  std::vector<model::Building> result;
  result.reserve(real.size());
  for (const auto& build : real) {
    /// если вражеское строение
    /// у него нет кд
    /// и р€дом есть враги (то бишь дл€ нас союзники)

    if ( build.getFaction() == Game::enemyFaction()
      && 0 == build.getRemainingActionCooldownTicks()
      && !World::instance().aroundEnemies(build, build.getAttackRange()).empty()) {
      result.push_back(Building(build, build.getCooldownTicks()));
    } else {
      result.push_back(build);
    }
  }

  return result;

}

static std::vector<model::Building> updateBuildingTicks(const std::vector<model::Building>& real, int tickDt) {
  assert(tickDt >= 0);
  std::vector<model::Building> result;
  result.reserve(real.size());

  for (const auto& obj : real) {
    result.push_back(Building(obj, MAX(0, obj.getRemainingActionCooldownTicks() - tickDt)));
  }

  return result;
}


/// World
World::World(): isInitialized(false), lastUpdateTick(0), modelWorld(nullptr) {
}

void World::update(const model::World& world) {
  modelWorld = &world;

  if (!isInitialized) {
    initTrees();
    initBuildings();
    isInitialized = true;
  }

  updateVisionZone();
  updateSupposedWizards();
  updateSupposedData();
  updateBullets();
  updateMinions();

  lastUpdateTick = world.getTickIndex();
}

const std::vector<model::Tree>& World::trees() const {
  return allTrees;
}

const std::vector<model::Building>& World::buildings() const {
  return supposedBuilding;
}

const std::vector<model::Minion>& World::minions() const {
  return realMinions;
}

const std::vector<model::Wizard>& World::wizards() const {
  return supposedWizards;
}

const std::vector<Bullet>& World::bullets() const {
  return bulletsData;
}

const std::vector<Looking>& World::getVisionZone() const {
  return visionZone;
}

void World::initBuildings() {
  for (const auto& build : model().getBuildings()) {
    if (build.getFaction() == Game::friendFaction()) {
      const Position pos = Position(size() - build.getX(), size() - build.getY());

      supposedBuilding.push_back(Building(pos.x, pos.y, Game::enemyFaction(), build));
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

  for (int i = 150; i <= size() - 150; i += 100) {
    invisibleAreaTrees.push_back(Tree(i, -50, 50/*радиус*/));
    invisibleAreaTrees.push_back(Tree(i, size() + 50, 50/*радиус*/));
    invisibleAreaTrees.push_back(Tree(-50, i, 50/*радиус*/));
    invisibleAreaTrees.push_back(Tree(size() + 50, i, 50/*радиус*/));
  }
  /// + 4 дерева по углам, сделал так чтобы на углах плавнее был проход
  invisibleAreaTrees.push_back(Tree(50, 50, 50/*радиус*/));
  invisibleAreaTrees.push_back(Tree(size() - 50, 50, 50/*радиус*/));
  invisibleAreaTrees.push_back(Tree(size() - 50, size() - 50, 50/*радиус*/));
  invisibleAreaTrees.push_back(Tree(50, size() - 50, 50/*радиус*/));

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

bool World::isInVisionZone(double x, double y) const {
  for (const auto& vision : visionZone) {
    const double dx = x - vision.getX();
    const double dy = y - vision.getY();
    if (dx*dx + dy*dy < vision.getVisionRange() * vision.getVisionRange()) {
      return true;
    }
  }
  return false;
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
  supposedTrees = merge(supposedTrees, updateRadius<model::Tree, Tree>(modelWorld->getTrees()));

  allTrees = supposedTrees;
  allTrees.insert(allTrees.end(), invisibleAreaTrees.begin(), invisibleAreaTrees.end());

  supposedBuilding = updateBuildingTicks(supposedBuilding, model().getTickIndex() - lastUpdateTick);
  supposedBuilding = updateBuildingsTicksByAttack(supposedBuilding);
  supposedBuilding = merge(supposedBuilding, updateRadius<model::Building, Building>(modelWorld->getBuildings()));
}

void World::updateSupposedWizards() {
  std::vector<model::Wizard> newWizardData;

  for (const auto& wizard : model().getWizards()) {
    newWizardData.push_back(wizard);

    /// удал€ем мага из старых данных
    for (size_t i = 0; i < supposedWizards.size(); i++) {
      if (supposedWizards[i].getId() == wizard.getId()) {
        supposedWizards.erase(supposedWizards.begin() + i);
        break;
      }
    }
  }

  const auto enemyRessurection = Points::point(Points::RENEGADES_BASE);
  const double dt = model().getTickIndex() - lastUpdateTick;

  /// после чего остались маги, о которых была утер€на информаци€, но на прошлом тике они были
  for (const auto& wizard : supposedWizards) {
    /// если раньше дружественный маг был, а сейчас нет, значит он мертв
    if (wizard.getFaction() == Game::friendFaction()) {
      continue;
    }

    /// иначе это вражеский маг, и его надо просто сместить, или причислить к убитым
    /// если мы можем его видеть, значит скорей всего он мертв
    if (isInVisionZone(wizard.getX(), wizard.getY())) {
      newWizardData.push_back(EnemyWizard(wizard, enemyRessurection.x, enemyRessurection.y));
    } else {
      /// сдвигаем мага по направлении базы, с максимальной его скоростью
      auto delta = enemyRessurection - EX::pos(wizard);
      double dLength = delta.length();
      if (dLength > 1) {
        double length = EX::maxSpeed(wizard) * dt;
        length = MIN(dLength, length);
        delta = delta.normal() * length;
      }

      newWizardData.push_back(EnemyWizard(wizard, wizard.getX() + delta.x, wizard.getY() + delta.y));
    }
  }

  supposedWizards = newWizardData;
}

void World::updateBullets() {
  std::vector<Bullet> newBullets;

  for (const auto& projectile : model().getProjectiles()) {
    bool found = false;
    /// провер€ем нет ли у нас уже такой пули, и если есть то берем данные из нее и обновл€ем позицию
    for (auto& bullet : bulletsData) {
      if (projectile.getId() == bullet.id) {
        bullet.pos = EX::pos(projectile);
        newBullets.push_back(bullet);
        found = true;
        break;
      }
    }

    if (found) {
      continue;
    }

    /// находим максимальную дистанцию полета
    double castRange = 500;
    if (model::PROJECTILE_DART == projectile.getType()) {
      castRange = Game::model().getFetishBlowdartAttackRange();
    } else {
      for (const auto& wizard : supposedWizards) {
        if (projectile.getOwnerUnitId() == wizard.getId()) {
          castRange = wizard.getCastRange();
          break;
        }
      }
    }

    const auto pos = EX::pos(projectile);
    const auto speed = Vector(projectile.getSpeedX(), projectile.getSpeedY());
    /// ≈сли пули не нашлось, значит это нова€
    newBullets.push_back(Bullet(
      projectile.getId(),
      speed,
      projectile.getRadius(),
      pos - speed, /// так как пули мы видим только после его запуска, то еЄ позици€ запуска всегда смещена как миниум на тик назад
      pos,
      castRange,
      projectile.getType(),
      projectile.getFaction()
    ));
  }

  bulletsData = newBullets;
}


const model::LaneType World::positionToLine(const double x, const double y) const {
  double delta = (size() - x) - y;

  /// если база сво€
  if (x < 600 && (size() - y) < 600) {
    return model::_LANE_UNKNOWN_;
  }

  /// если база противника
  if ((size()-x) < 600 && y < 600) {
    return model::_LANE_UNKNOWN_;
  }

  double ds = size() / 5;
  /// лень писать нормальное вычисление пр€мых которые бы давали хорошую оценку где находитьс€ маг
  if ((x < 1200 && (size() - y) < 1200) || ((size() - x) < 1200 && y < 1200)) {
    ds = size() / 10;
  }

  if (delta > ds) {
    return model::LANE_TOP;
  } else if (delta < -ds) {
    return model::LANE_BOTTOM;
  }

  return model::LANE_MIDDLE;
}

const int World::towerCount(model::LaneType lane, model::Faction faction) const {
  int count = 0;
  for (const auto& build : buildings()) {
    if (faction == build.getFaction()
      && model::BUILDING_GUARDIAN_TOWER == build.getType()
      && lane == positionToLine(build.getX(), build.getY())) {
      count++;
    }
  }
  return count;
}

const int World::wizardCount(model::LaneType line, model::Faction faction) const {
  int result = 0;
  for (const auto& wizard : model().getWizards()) {
    if (faction == wizard.getFaction() && line == positionToLine(wizard.getX(), wizard.getY())) {
      result++;
    }
  }

  return result;
}

const int World::wizardCount(model::LaneType line, model::Faction faction, const model::Wizard& excludeWizard) const {
  int result = 0;
  for (const auto& wizard : wizards()) {
    if (wizard.getId() != excludeWizard.getId()
      && faction == wizard.getFaction() && line == positionToLine(wizard.getX(), wizard.getY())) {
      result++;
    }
  }

  return result;
}

const std::vector<const model::Wizard*> World::wizards(model::LaneType line, model::Faction faction) const {
  std::vector<const model::Wizard*> result;
  for (const auto& wizard : wizards()) {
    if (faction == wizard.getFaction() && line == positionToLine(wizard.getX(), wizard.getY())) {
      result.push_back(&wizard);
    }
  }

  return result;
}

const std::vector<const model::Wizard*> World::aroundAuraWizards(const model::Wizard& unit) {
  std::vector<const model::Wizard*> result;

  for (const auto& wizard : model().getWizards()) {
    if (wizard.getId() != unit.getId() && unit.getFaction() == wizard.getFaction()
      && unit.getDistanceTo(wizard) < Game::model().getAuraSkillRange()) {
      result.push_back(&wizard);
    }
  }

  return result;
}

Obstacles World::allObstacles(const model::CircularUnit& unit, const bool onlyStatic) const {
  Obstacles obstacles;
  obstacles.reserve(500); // ѕриблизительно сколько объектов в среднем

  for (const auto& tree : trees()) {
    obstacles.push_back(&tree);
  }

  for (const auto& build : buildings()) {
    obstacles.push_back(&build);
  }

  for (const auto& minion : model().getMinions()) {
    if (unit.getId() != minion.getId() && (!onlyStatic || model::FACTION_NEUTRAL == minion.getFaction() || Game::enemyFaction() == minion.getFaction())) {
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

bool checkObstacle(const model::CircularUnit& unit, const model::CircularUnit& obstacle, const double range, const Vector direction) {
  const auto unitPos = Position(unit.getX(), unit.getY());
  const auto obstaclePos = Position(obstacle.getX(), obstacle.getY());
  const auto delta = obstaclePos - unitPos;

  if ((obstaclePos - unitPos).length2() > range * range) {
    return false;
  }

  if (direction.length2() > 0.01) {
    /// спина юнита
    const auto unitBackPos = unitPos - direction.normal() * (unit.getRadius() + obstacle.getRadius());
    const auto delta = obstaclePos - unitBackPos;

    ///объект за спиной
    if (delta.dot(direction) < 0) {
      return false;
    }
  }

  return true;
}

Obstacles World::obstacles(const model::CircularUnit& unit, const double range, const Vector direction) const {
  Obstacles aroundObstacles;
  aroundObstacles.reserve(64); // ѕриблизительно сколько объектов в среднем

  for (const auto& tree : trees()) {
    if (checkObstacle(unit, tree, range, direction)) {
      aroundObstacles.push_back(&tree);
    }
  }

  for (const auto& build : buildings()) {
    if (checkObstacle(unit, build, range, direction)) {
      aroundObstacles.push_back(&build);
    }
  }

  for (const auto& minion : model().getMinions()) {
    if (unit.getId() != minion.getId() && checkObstacle(unit, minion, range, direction)) {
      aroundObstacles.push_back(&minion);
    }
  }

  for (const auto& wizard : model().getWizards()) {
    if (unit.getId() != wizard.getId() && checkObstacle(unit, wizard, range, direction)) {
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

std::vector<const model::LivingUnit*> World::around(const model::LivingUnit& unit, const model::Faction faction, double radius) const {
  std::vector<const model::LivingUnit*> result;

  const auto unitPos = Position(unit.getX(), unit.getY());
  const auto radius2 = radius * radius;

  for (const auto& wizard : wizards()) {
    if (wizard.getFaction() == faction && wizard.getId() != unit.getId()) {
      const auto wizardPos = Position(wizard.getX(), wizard.getY());
      if ((wizardPos - unitPos).length2() < radius2) {
        result.push_back(&wizard);
      }
    }
  }

  for (const auto& minion : minions()) {
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

std::vector<const model::LivingUnit*> World::aroundEnemies(const model::LivingUnit& unit, const double radius) const {
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
  /*if (Visualizator::PRE == visualizator.getStyle()) {
    for (const auto& build : supposedBuilding) {
      visualizator.fillCircle(build.getX(), build.getY(), build.getRadius(), 0xffff00);
    }
  }*/

  if (Visualizator::POST == visualizator.getStyle()) {
    for (const auto& wizard : supposedWizards) {
      visualizator.fillCircle(wizard.getX(), wizard.getY(), wizard.getRadius() * 0.75, 0xffff00);
    }
  }

  //if (Visualizator::POST == visualizator.getStyle()) {
  //  for (const auto& tree : supposedTrees) {
  //    visualizator.fillCircle(tree.getX(), tree.getY(), tree.getRadius(), 0xff7700);
  //  }
  //}

}
#endif // ENABLE_VISUALIZATOR
