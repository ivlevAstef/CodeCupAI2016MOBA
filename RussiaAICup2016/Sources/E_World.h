//
//File: E_World.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "model\World.h"
#include "C_Vector2D.h"
#include "C_Singleton.h"
#include "E_Types.h"
#include "E_WorldObjects.h"
#include "E_Bullet.h"
#include <unordered_set>

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{

  class World: public Singleton<World> {
  public:
    World();

    void update(const model::World& world);

    static inline const model::World& model() {
      return *instance().modelWorld;
    }

    static inline double size() {
      assert(model().getWidth() == model().getHeight());
      return model().getWidth();
    }

    const std::vector<model::Tree>& trees() const;
    const std::vector<model::Building>& buildings() const;
    const std::vector<model::Minion>& minions() const;
    const std::vector<model::Wizard>& wizards() const;

    const std::vector<Bullet>& bullets() const;

    const std::vector<Looking>& getVisionZone() const;
    bool isInVisionZone(double x, double y) const;


    Obstacles allObstacles(const model::CircularUnit& unit, const bool onlyStatic = false) const;
    Obstacles obstacles(const model::CircularUnit& unit, const double range, const Vector direction = Vector()) const;
    ObstaclesGroups createGroup(const Obstacles& obstacles, const double radius) const;

    const model::LivingUnit* unit(long long id) const;
    const model::CircularUnit* unitOrProjectile(long long id) const;

    std::vector<const model::LivingUnit*> around(const model::LivingUnit& unit, const model::Faction faction, double radius) const;
    std::vector<const model::LivingUnit*> aroundEnemies(const model::LivingUnit& unit, const double radius) const;

    const std::vector<const model::Wizard*> aroundAuraWizards(const model::Wizard& unit);

    const int towerCount(model::LaneType line, model::Faction faction) const;
    const int wizardCount(model::LaneType line, model::Faction faction) const;
    const std::vector<const model::Wizard*> wizards(model::LaneType line, model::Faction faction) const;
    const int wizardCount(model::LaneType line, model::Faction faction, const model::Wizard& excludeWizard) const;
    const model::LaneType positionToLine(const double x, const double y) const;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  private:
    void initBuildings();
    void initTrees();

    void updateVisionZone();
    void updateSupposedData();
    void updateSupposedWizards();
    void updateBullets();

    void updateMinions();

    /// проверяет, что миньон перешел на темную сторону :)
    bool checkMinionOnNeutral(const model::Minion& neutral) const;

  private:
    bool isInitialized;
    int lastUpdateTick;

    const model::World* modelWorld;

    std::vector<Looking> visionZone;

    std::vector<model::Tree> supposedTrees;
    std::vector<model::Tree> allTrees;
    std::vector<model::Tree> invisibleAreaTrees;
    std::vector<model::Building> supposedBuilding;
    std::vector<model::Minion> realMinions;
    std::vector<model::Wizard> supposedWizards;
    std::vector<Bullet> bulletsData;

    std::unordered_set<long long int> hateNeuralMinions;/// дада злые нейтральные миньоны
  };
}