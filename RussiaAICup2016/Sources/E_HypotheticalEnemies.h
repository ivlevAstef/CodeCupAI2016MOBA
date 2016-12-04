//
//File: E_HypotheticalEnemies.h
//Author: Ivlev Alexander. Stef
//Created: 23/11/2016
//

#pragma once

#include "C_Singleton.h"
#include "C_Vector2D.h"
#include "model\Minion.h"
#include "model\LaneType.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{
  class HypotheticalEnemies: public Singleton<HypotheticalEnemies> {
  public:
    struct Wave {
      int lastUpdateIndex;
      model::LaneType lane;
      Position center;
      std::vector<model::Minion> minions;
    };

  public:
    HypotheticalEnemies();

    void update();

    const std::vector<model::Minion>& getHypotheticalMinions() const;

    const std::vector<Wave> nextWaveData(double& timeToNext) const;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  private:
    void createWave(const int waveNumber);
    Wave createWave(const int waveNumber, const model::LaneType lane) const;

    void moveWaves();
    void updateWaves();

  private:
    std::vector<Wave> waves;
    std::vector<model::Minion> allMinions;

    int lastUpdateIndex;
  };
}