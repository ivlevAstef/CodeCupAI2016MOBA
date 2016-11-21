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

    const std::vector<model::Tree>& trees() const;
    const std::vector<model::Building>& buildings() const;
    const std::vector<model::Wizard>& wizards() const;

    Obstacles obstacles(const model::CircularUnit& unit, const double range, const bool onlyStatic = false) const;
    ObstaclesGroups obstaclesGroup(const model::CircularUnit& unit, const double range, const bool onlyStatic = false) const;

    const model::LivingUnit* unit(long long id) const;

    std::vector<const model::LivingUnit*> around(const model::Wizard& unit, const model::Faction faction, double radius = -1) const;
    std::vector<const model::LivingUnit*> aroundEnemies(const model::Wizard& unit, const double radius = -1) const;

    const Position& linePosition(model::LaneType line) const;

    const int wizardCount(model::LaneType line) const;
    const int wizardCount(model::LaneType line, const model::Wizard& excludeWizard) const;
    const model::LaneType positionToLine(const double x, const double y) const;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  private:
    void initLinePosition();
    void initTrees();

    void recalculateLinePositions();
    void updateVisionZone();
    void updateSupposedData();


  private:
    bool isInitialized;

    const model::World* modelWorld;

    std::vector<Looking> visionZone;
    std::vector<model::Tree> supposedTrees;
    std::vector<model::Building> supposedBuilding;
    std::vector<model::Wizard> supposedWizards;

    Position topLinePosition;
    Position middleLinePosition;
    Position bottomLinePosition;
  };
}