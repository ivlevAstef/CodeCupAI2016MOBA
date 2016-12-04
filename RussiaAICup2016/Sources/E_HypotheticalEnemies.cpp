#include "E_HypotheticalEnemies.h"
#include "E_World.h"
#include "E_Game.h"
#include "A_Move.h"
#include "C_Vector2D.h"
#include "C_Extensions.h"
#include "E_Points.h"
#include "E_WorldObjects.h"

using namespace AICup;

HypotheticalEnemies::HypotheticalEnemies() {
  lastUpdateIndex = 0;
}

//static Position reverse(Position pos) {
//  return Position(pos.y, pos.x);
//}

const std::vector<Position>& minionPath(const model::LaneType lane) {
  /// Массивы точек, по которым идут крипы
  static const std::vector<Position> topLinePoints = {
    Points::point(Points::RENEGADES_BASE_TOP),
    Points::point(Points::TOP_CENTER),
    Points::point(Points::ACADEMY_BASE)
  };

  static const std::vector<Position> middleLinePoints = {
    Points::point(Points::RENEGADES_BASE_MIDDLE),
    Points::point(Points::ACADEMY_BASE)
  };

  static const std::vector<Position> bottomLinePoints = {
    Points::point(Points::RENEGADES_BASE_BOTTOM),
    Points::point(Points::BOTTOM_CENTER),
    Points::point(Points::ACADEMY_BASE)
  };


  switch (lane) {
    case model::LANE_TOP:
      return topLinePoints;
    case model::LANE_MIDDLE:
      return middleLinePoints;
    case model::LANE_BOTTOM:
      return bottomLinePoints;
    default:
      break;
  }
  assert(false && "Incorrect lane type");
  return middleLinePoints;
}


/// первая точка это fetish, следующие три это orc, и последняя центр
const std::vector<Position>& minionStartPositions(const model::LaneType lane) {
  static const Position topLine = Points::point(Points::RENEGADES_BASE_TOP);
  static const Position middleLine = Points::point(Points::RENEGADES_BASE_MIDDLE);
  static const Position bottomLine = Points::point(Points::RENEGADES_BASE_BOTTOM);

  static const std::vector<Position> topLinePoints = {
    topLine + Vector(25, 0),
    topLine + Vector(0, 100),
    topLine + Vector(0, -100),
    topLine + Vector(-125, 0),
    topLine
  };

  static const std::vector<Position> middleLinePoints = {
    middleLine + Vector(75, -75),
    middleLine + Vector(0, -100),
    middleLine + Vector(100, 0),
    middleLine + Vector(-50, 50),
    middleLine
  };

  static const std::vector<Position> bottomLinePoints = {
    bottomLine + Vector(0, -25),
    bottomLine + Vector(100, 0),
    bottomLine + Vector(-100, 0),
    bottomLine + Vector(0, 125),
    bottomLine
  };


  switch (lane) {
    case model::LANE_TOP:
      return topLinePoints;
    case model::LANE_MIDDLE:
      return middleLinePoints;
    case model::LANE_BOTTOM:
      return bottomLinePoints;
    default:
      break;
  }
  assert(false && "Incorrect lane type");
  return middleLinePoints;
}

void HypotheticalEnemies::update() {
  /// номер волны
  int WaveNumberIter = lastUpdateIndex / Game::model().getFactionMinionAppearanceIntervalTicks();
  const int waveNumber = World::model().getTickIndex() / Game::model().getFactionMinionAppearanceIntervalTicks();
  while(WaveNumberIter != waveNumber) {
    WaveNumberIter++; //не напутано, вначале повышаем
    createWave(WaveNumberIter);

  }
  lastUpdateIndex = World::model().getTickIndex();

  moveWaves();
  updateWaves();
}

void HypotheticalEnemies::createWave(const int waveNumber) {
  waves.push_back(createWave(waveNumber, model::LANE_TOP));
  waves.push_back(createWave(waveNumber, model::LANE_MIDDLE));
  waves.push_back(createWave(waveNumber, model::LANE_BOTTOM));
}

HypotheticalEnemies::Wave HypotheticalEnemies::createWave(const int waveNumber, const model::LaneType lane) const {
  const auto& mPoss = minionStartPositions(lane);
  assert(5 == mPoss.size());

   std::vector<model::Minion> minions = {
    FetishMinion(mPoss[0].x, mPoss[0].y, Game::enemyFaction()),
    OrcMinion(mPoss[1].x, mPoss[1].y, Game::enemyFaction()),
    OrcMinion(mPoss[2].x, mPoss[2].y, Game::enemyFaction()),
    OrcMinion(mPoss[3].x, mPoss[3].y, Game::enemyFaction())
  };

  return Wave{
    waveNumber * Game::model().getFactionMinionAppearanceIntervalTicks(),
    lane,
    mPoss[4],
    minions
  };
}

const std::vector<HypotheticalEnemies::Wave> HypotheticalEnemies::nextWaveData(double& timeToNext) const {
  const int waveNumber = 1 + (World::model().getTickIndex() / Game::model().getFactionMinionAppearanceIntervalTicks());

  std::vector<HypotheticalEnemies::Wave> result;
  result.push_back(createWave(waveNumber, model::LANE_TOP));
  result.push_back(createWave(waveNumber, model::LANE_MIDDLE));
  result.push_back(createWave(waveNumber, model::LANE_BOTTOM));

  timeToNext = World::model().getTickIndex() % Game::model().getFactionMinionAppearanceIntervalTicks();
  timeToNext = timeToNext / double(Game::model().getFactionMinionAppearanceIntervalTicks());
  return result;
}

void HypotheticalEnemies::moveWaves() {
  const auto currentTickIndex = World::model().getTickIndex();

  for (auto& wave : waves) {
    double offset = (currentTickIndex - wave.lastUpdateIndex) * Game::model().getMinionSpeed();

    const auto newCenter = Algorithm::offsetPointByPath(wave.center, offset, minionPath(wave.lane));
    const auto delta = newCenter - wave.center;

    std::vector<model::Minion> newMinions;
    for (const auto& minion : wave.minions) {
      newMinions.push_back(Minion(minion.getX() + delta.x, minion.getY() + delta.y, minion));
    }

    wave.center = newCenter;
    wave.minions = newMinions;
    wave.lastUpdateIndex = currentTickIndex;
  }
}

void HypotheticalEnemies::updateWaves() {
  /// удаляет миньонов которые должны были появиться на карте, и заодно заполняет список всех миньонов
  allMinions.clear();

  for (int waveIndex = waves.size() - 1; waveIndex >= 0; waveIndex--) {
    auto& wave = waves[waveIndex];

    for (int minionIndex = wave.minions.size() - 1; minionIndex >= 0; minionIndex--) {
      const auto& minion = wave.minions[minionIndex];

      if (World::instance().isInVisionZone(minion.getX(), minion.getY())) {
        wave.minions.erase(wave.minions.begin() + minionIndex);
      } else {
        allMinions.push_back(minion);
      }
    }

    if (0 == wave.minions.size()) {
      waves.erase(waves.begin() + waveIndex);
    }

  }
}

const std::vector<model::Minion>& HypotheticalEnemies::getHypotheticalMinions() const {
  return allMinions;
}

#ifdef ENABLE_VISUALIZATOR
void HypotheticalEnemies::visualization(const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    for (const auto& minion : allMinions) {
      visualizator.fillCircle(minion.getX(), minion.getY(), minion.getRadius(), 0xff0066);
    }
  }
}
#endif // ENABLE_VISUALIZATOR
