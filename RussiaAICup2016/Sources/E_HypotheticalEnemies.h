//
//File: E_HypotheticalEnemies.h
//Author: Ivlev Alexander. Stef
//Created: 23/11/2016
//

#pragma once

#include "C_Singleton.h"
#include "C_Vector2D.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{
  class HypotheticalEnemies: public Singleton<HypotheticalEnemies> {
  public:
    void update();

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  private:
  };
}