#include "E_DangerMap.h"
#include "E_World.h"
#include "E_Game.h"
#include "E_HypotheticalEnemies.h"
#include "C_Math.h"

using namespace AICup;

Position DangerMapConstants::toReal(Vector2D<int> point, double dx, double dy) {
  return Position((point.x + dx) * step, (point.y + dy) * step);
}
Vector2D<int> DangerMapConstants::toInt(Position point) {
  return Vector2D<int>(floor(point.x / step), floor(point.y / step));
}

void DangerMap::update() {
  clean();
  includeFriends();
  includeEnemies();
  includeHypotheticalEnemies();
}

void DangerMap::clean() {
  for (size_t x = 0; x < DangerMapConstants::memorySize; x++) {
    for (size_t y = 0; y < DangerMapConstants::memorySize; y++) {
      friends[x][y] = DangerMapConstants::neutral;
      enemies[x][y] = DangerMapConstants::neutral;
    }
  }
}


double minionDps(const model::Minion& minion) {
  if (model::MINION_FETISH_BLOWDART == minion.getType()) {
    return Game::model().getDartDirectDamage() / double(minion.getCooldownTicks());
  }
  return Game::model().getOrcWoodcutterDamage() / double(minion.getCooldownTicks());
}

double minionRadius(const model::Minion& minion) {
  if (model::MINION_FETISH_BLOWDART == minion.getType()) {
    return Game::model().getFetishBlowdartAttackRange();
  }
  return Game::model().getOrcWoodcutterAttackRange() + minion.getRadius();/*а то совсем маленький*/
}

double minionDanger(const model::Minion& minion) {
  return minion.getLife() * minionDps(minion);
}

double buildDps(const model::Building& build) {
  return build.getDamage() / double(build.getCooldownTicks());
}

double buildRadius(const model::Building& build, const double coef) {
  const double ticks = 1 - ((double)build.getRemainingActionCooldownTicks() / (double)build.getCooldownTicks());

  return MAX(build.getRadius(), build.getAttackRange() * ticks * coef);
}

double buildDanger(const model::Building& build, const double coef) {
  const double ticks = 1 - ((double)build.getRemainingActionCooldownTicks() / (double)build.getCooldownTicks());
  const double dps = build.getDamage() / build.getCooldownTicks();
  return build.getLife() * coef * 0.08/*иначе слишком опасная*/ * buildDps(build) * ticks;
}

void DangerMap::includeFriends() {
  for (const auto& minion : World::instance().minions()) {
    if (model::FACTION_ACADEMY == minion.getFaction()) {
      includeFriend(minion, minionRadius(minion), minionDanger(minion));
    }
  }

  for (const auto& build : World::instance().buildings()) {
    if (model::FACTION_ACADEMY == build.getFaction()) {
      for (double c = 0.1; c <= 1.0; c += 0.1) {
        includeFriend(build, buildRadius(build, c), buildDanger(build, 1.0 - c));
      }
    }
  }

}
void DangerMap::includeEnemies() {
  for (const auto& minion : World::instance().minions()) {
    if (model::FACTION_RENEGADES == minion.getFaction()) {
      includeEnemy(minion, minionRadius(minion), minionDanger(minion));
    }
  }

  for (const auto& build : World::instance().buildings()) {
    if (model::FACTION_RENEGADES == build.getFaction()) {
      for (double c = 0.1; c <= 1.0; c += 0.1) {
        includeEnemy(build, buildRadius(build, c), buildDanger(build, 1.0 - c));
      }
    }
  }

}
void DangerMap::includeHypotheticalEnemies() {
  for (const auto& minion : HypotheticalEnemies::instance().getHypotheticalMinions()) {
    includeEnemy(minion, minionRadius(minion), minionDanger(minion));
  }
}

void DangerMap::includeFriend(const model::Unit& unit, const double radius, const float danger) {
  Math::fillGrid(reinterpret_cast<float*>(friends), unit.getX(), unit.getY(), DangerMapConstants::step, radius, danger);
}
void DangerMap::includeEnemy(const model::Unit& unit, const double radius, const float danger) {
  Math::fillGrid(reinterpret_cast<float*>(enemies), unit.getX(), unit.getY(), DangerMapConstants::step, radius, danger);
}

#ifdef ENABLE_VISUALIZATOR
void DangerMap::visualization(const Visualizator& visualizator) const {
  if (Visualizator::ABS == visualizator.getStyle()) {
    for (int x = 0; x < DangerMapConstants::memorySize; x++) {
      for (int y = 0; y < DangerMapConstants::memorySize; y++) {
        const double fWeight = 100 + (friends[x][y] * 5);
        const double eWeight = 100 + (enemies[x][y] * 5);
        int32_t colorRed = MAX(100, MIN((int)(eWeight), 255));
        int32_t colorGreen = MAX(100, MIN((int)(fWeight), 255));
        visualizator.fillRect(x*5, y*5, (x+1)*5, (y+1)*5 , (colorRed << 16) | (colorGreen << 8) | (100));
      }
    }
  }
}
#endif // ENABLE_VISUALIZATOR
