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

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{

  class World: public Singleton<World> {
  public:
    World();

    void update(const model::World& world);

    const model::World& model() const;

    std::vector<model::Tree>& trees();
    std::vector<model::Wizard>& wizards();

    Obstacles obstacles(const model::Wizard& unit) const;
    ObstaclesGroups obstaclesGroup(const model::Wizard& unit) const;

    const model::LivingUnit* unit(long long id) const;

    std::vector<const model::LivingUnit*> around(const model::Wizard& unit, const model::Faction faction) const;
    std::vector<const model::LivingUnit*> aroundEnemies(const model::Wizard& unit) const;

    const Position& linePosition(model::LaneType line) const;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  private:
    void init();

    void recalculateLinePositions();
    void updateSupposedData();


  private:
    bool isInitialized;

    const model::World* modelWorld;

    std::vector<model::Tree> supposedTrees;
    std::vector<model::Wizard> supposedWizards;

    Position topLinePosition;
    Position middleLinePosition;
    Position bottomLinePosition;
  };
}