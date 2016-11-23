//
//File: E_HypotheticalEnemies.h
//Author: Ivlev Alexander. Stef
//Created: 23/11/2016
//

#pragma once

#include "C_Singleton.h"
#include "C_Vector2D.h"
#include "model\Minion.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{
  class HypotheticalEnemies: public Singleton<HypotheticalEnemies> {
  public:
    HypotheticalEnemies();

    void update();

    const std::vector<model::Minion>& getHypotheticalMinions() const;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  private:
    std::vector<model::Minion> hypotheticalMinions;
  };
}