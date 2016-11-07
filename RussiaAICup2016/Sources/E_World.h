//
//File: E_World.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "model\World.h"
#include "C_Vector2D.h"
#include "C_Singleton.h"

namespace AICup
{
  class World: public Singleton<World> {
  public:
    void update(const model::World& world);

    const model::World& model() const;

    std::vector<model::Tree>& trees();
    std::vector<model::Wizard>& wizards();

    Position linePosition(model::LineType line);


  private:
    void updateSupposedData();


  private:
    const model::World* modelWorld;

    std::vector<model::Tree> supposedTrees;
    std::vector<model::Wizard> supposedWizards;
  };
}