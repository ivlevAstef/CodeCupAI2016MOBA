#include "E_InfluenceMap.h"
#include "E_World.h"
#include "E_Game.h"
#include "E_HypotheticalEnemies.h"
#include "C_Math.h"
#include "E_Points.h"

using namespace AICup;

Position InfluenceMapConstants::toReal(Vector2D<int> point, double dx, double dy) {
  return Position((point.x + dx) * step, (point.y + dy) * step);
}
Vector2D<int> InfluenceMapConstants::toInt(Position point) {
  return Vector2D<int>(floor(point.x / step), floor(point.y / step));
}

InfluenceMap::InfluenceMap() {
  topForeFront = Points::point(Points::TOP_CENTER);
  middleForeFront = Points::point(Points::MIDDLE_CENTER);
  bottomForeFront = Points::point(Points::BOTTOM_CENTER);
}

void InfluenceMap::update() {
  clean();
  includeFriends();
  includeEnemies();
  includeHypotheticalEnemies();

  updateLinePosition();
}

const float const* InfluenceMap::getFriendsMap() const {
  return reinterpret_cast<const float const*>(friends);
}
const float const* InfluenceMap::getEnemiesMap() const {
  return reinterpret_cast<const float const*>(enemies);
}

void InfluenceMap::updateLinePosition() {
  const double maxSpeed = 25;
  /// Да мне было не лень копировать этот код ниже...
  auto newTopForeFront = calculateForeFront(model::LANE_TOP);
  auto newMiddleForeFront = calculateForeFront(model::LANE_MIDDLE);
  auto newBottomForeFront = calculateForeFront(model::LANE_BOTTOM);

  float topDistance = (newTopForeFront - topForeFront).length();
  float middleDistance = (newMiddleForeFront - middleForeFront).length();
  float bottomDistance = (newBottomForeFront - bottomForeFront).length();

  auto newMoveTopForeFront = getForeFront(model::LANE_TOP, MIN(maxSpeed, topDistance));
  auto newMoveMiddleForeFront = getForeFront(model::LANE_MIDDLE, MIN(maxSpeed, middleDistance));
  auto newMoveBottomForeFront = getForeFront(model::LANE_BOTTOM, MIN(maxSpeed, bottomDistance));

  if ((newTopForeFront - newMoveTopForeFront).length() > topDistance) {
    topForeFront = getForeFront(model::LANE_TOP, -MIN(maxSpeed, topDistance));
  } else {
    topForeFront = newMoveTopForeFront;
  }

  if ((newMiddleForeFront - newMoveMiddleForeFront).length() > middleDistance) {
    middleForeFront = getForeFront(model::LANE_MIDDLE, -MIN(maxSpeed, middleDistance));
  } else {
    middleForeFront = newMoveMiddleForeFront;
  }

  if ((newBottomForeFront - newMoveBottomForeFront).length() > bottomDistance) {
    bottomForeFront = getForeFront(model::LANE_BOTTOM, -MIN(maxSpeed, bottomDistance));
  } else {
    bottomForeFront = newMoveBottomForeFront;
  }

}


Position InfluenceMap::getForeFront(const model::LaneType lane, const float offset) const {
  switch (lane) {
    case model::LANE_TOP:
      return offsetForeFront(topForeFront, offset, getLinePoints(lane));
    case model::LANE_MIDDLE:
      return offsetForeFront(middleForeFront, offset, getLinePoints(lane));
    case model::LANE_BOTTOM:
      return offsetForeFront(bottomForeFront, offset, getLinePoints(lane));
    default:
      break;
  }
  assert(false && "Incorrect lane type");
  return Position(0,0);
}

const std::vector<Position>& InfluenceMap::getLinePoints(const model::LaneType lane) const {
  /// Массивы точек, по которым идут крипы
  static const std::vector<Position> topLinePoints = {
    Points::point(Points::RENEGADES_BASE),
    Points::point(Points::TOP_CENTER),
    Points::point(Points::ACADEMY_BASE)
  };

  static const std::vector<Position> middleLinePoints = {
    Points::point(Points::RENEGADES_BASE),
    Points::point(Points::ACADEMY_BASE)
  };

  static const std::vector<Position> bottomLinePoints = {
    Points::point(Points::RENEGADES_BASE),
    Points::point(Points::BOTTOM_CENTER),
    Points::point(Points::ACADEMY_BASE)
  };


  const std::vector<Position>* linePointsPtr = nullptr;
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

Position InfluenceMap::calculateForeFront(const model::LaneType lane) const {
  const std::vector<Position>& linePoints = getLinePoints(lane);

  /// идем по всем точкам, если зона дружественная, значит это линия фронта так как начинаем идти от врагов
  for (size_t i = 1; i < linePoints.size(); i++) {
    auto p1 = InfluenceMapConstants::toInt(linePoints[i - 1]);
    auto p2 = InfluenceMapConstants::toInt(linePoints[i]);

    /// ограничивыем точки дабы не выйти за границу в цикле
    p1.x = MAX(1, MIN(p1.x, InfluenceMapConstants::memorySize - 2));
    p1.y = MAX(1, MIN(p1.y, InfluenceMapConstants::memorySize - 2));
    p2.x = MAX(1, MIN(p2.x, InfluenceMapConstants::memorySize - 2));
    p2.y = MAX(1, MIN(p2.y, InfluenceMapConstants::memorySize - 2));

    const auto delta = p2 - p1;

    if (abs(delta.x) > abs(delta.y)) {
      int sign = SIGN(p2.x - p1.x);
      for (int x = p1.x; x != p2.x; x += sign) {
        const int y = p1.y + ((x - p1.x) * delta.y) / delta.x;
        if (isFriendZone(x, y)) {
          return pointToForeFront(x, y, linePoints, i);
        }
      }
    } else {
      int sign = SIGN(p2.y - p1.y);
      for (int y = p1.y; y != p2.y; y += sign) {
        const int x = p1.x + ((y - p1.y) * delta.x) / delta.y;
        if (isFriendZone(x, y)) {
          return pointToForeFront(x, y, linePoints, i);
        }
      }
    }
  }

  assert(false && "hmmmmm... not found friend zone? really");
  return Position(0, 0);
}

bool InfluenceMap::isFriendZone(const int x, const int y) const {
  const auto friendForce = friends[x][y] + friends[x + 1][y] + friends[x][y + 1] + friends[x - 1][y] + friends[x][y - 1];
  const auto enemyForce = enemies[x][y] + enemies[x + 1][y] + enemies[x][y + 1] + enemies[x - 1][y] + enemies[x][y - 1];
  ///если в этой клетке силы сильнее на одного юнита чем у врага, то значит подходит
  return 20 < friendForce - enemyForce;
}

Position InfluenceMap::pointToForeFront(const int x, const int y, const std::vector<Position>& line, size_t index) const {
  const Position startCentralPosition = InfluenceMapConstants::toReal({x, y}, 0.5, 0.5);
  return Math::point_distanceToSegment(startCentralPosition, line[index - 1], line[index]);
}


Position InfluenceMap::offsetForeFront(const Position& foreFront, float offset, const std::vector<Position>& line) const {
  if (offset < 0) {
    std::vector<Position> reverseLine = line;
    std::reverse(reverseLine.begin(), reverseLine.end());
    return offsetForeFront(foreFront, -offset, reverseLine);
  }

  size_t index = 0;
  for (index = 1; index < line.size(); index++) {
    if (Math::distanceToLine(foreFront, line[index - 1], line[index]) < 1.0e-3) {
      break;
    }
  }

  auto iterPos = foreFront;
  while (offset > 0 && index < line.size()) {
    const auto& p1 = line[index - 1];
    const auto& p2 = line[index];
    const double length = (p2 - iterPos).length();
    if (length < offset) {
      offset -= length;
      index++;
      iterPos = p2;
      continue;
    } else {
      iterPos = iterPos + ((p2 - iterPos) * (offset / length));
      break;
    }
  }

  return iterPos;
}

void InfluenceMap::clean() {
  for (size_t x = 0; x < InfluenceMapConstants::memorySize; x++) {
    for (size_t y = 0; y < InfluenceMapConstants::memorySize; y++) {
      friends[x][y] = InfluenceMapConstants::neutral;
      enemies[x][y] = InfluenceMapConstants::neutral;
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

void InfluenceMap::includeFriends() {
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
void InfluenceMap::includeEnemies() {
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
void InfluenceMap::includeHypotheticalEnemies() {
  for (const auto& minion : HypotheticalEnemies::instance().getHypotheticalMinions()) {
    includeEnemy(minion, minionRadius(minion), minionDanger(minion));
  }
}

void InfluenceMap::includeFriend(const model::Unit& unit, const double radius, const float danger) {
  Math::fillGrid(reinterpret_cast<float*>(friends), unit.getX(), unit.getY(), InfluenceMapConstants::step, radius, danger);
}
void InfluenceMap::includeEnemy(const model::Unit& unit, const double radius, const float danger) {
  Math::fillGrid(reinterpret_cast<float*>(enemies), unit.getX(), unit.getY(), InfluenceMapConstants::step, radius, danger);
}

#ifdef ENABLE_VISUALIZATOR
void InfluenceMap::visualization(const Visualizator& visualizator) const {
  /*if (Visualizator::ABS == visualizator.getStyle()) {
    for (int x = 0; x < InfluenceMapConstants::memorySize; x++) {
      for (int y = 0; y < InfluenceMapConstants::memorySize; y++) {
        const double fWeight = 100 + (friends[x][y] * 5);
        const double eWeight = 100 + (enemies[x][y] * 5);
        int32_t colorRed = MAX(100, MIN((int)(eWeight), 255));
        int32_t colorGreen = MAX(100, MIN((int)(fWeight), 255));
        visualizator.fillRect(x*5, y*5, (x+1)*5, (y+1)*5 , (colorRed << 16) | (colorGreen << 8) | (100));
      }
    }
  }*/

  if (Visualizator::PRE == visualizator.getStyle()) {
    const auto topLine    = getForeFront(model::LANE_TOP, 0);
    const auto middleLine = getForeFront(model::LANE_MIDDLE, 0);
    const auto bottomLine = getForeFront(model::LANE_BOTTOM, 0);

    const auto topFriendLine    = getForeFront(model::LANE_TOP, 100);
    const auto middleFriendLine = getForeFront(model::LANE_MIDDLE, 100);
    const auto bottomFriendLine = getForeFront(model::LANE_BOTTOM, 100);

    const auto topEnemyLine    = getForeFront(model::LANE_TOP, -100);
    const auto middleEnemyLine = getForeFront(model::LANE_MIDDLE, -100);
    const auto bottomEnemyLine = getForeFront(model::LANE_BOTTOM, -100);

    visualizator.fillCircle(topFriendLine.x, topFriendLine.y, 150, 0xaaffaa);
    visualizator.fillCircle(middleFriendLine.x, middleFriendLine.y, 150, 0xaaffaa);
    visualizator.fillCircle(bottomFriendLine.x, bottomFriendLine.y, 150, 0xaaffaa);

    visualizator.fillCircle(topEnemyLine.x, topEnemyLine.y, 150, 0xffaaaa);
    visualizator.fillCircle(middleEnemyLine.x, middleEnemyLine.y, 150, 0xffaaaa);
    visualizator.fillCircle(bottomEnemyLine.x, bottomEnemyLine.y, 150, 0xffaaaa);

    visualizator.fillCircle(topLine.x, topLine.y, 150, 0xffffaa);
    visualizator.fillCircle(middleLine.x, middleLine.y, 150, 0xffffaa);
    visualizator.fillCircle(bottomLine.x, bottomLine.y, 150, 0xffffaa);
  }
}
#endif // ENABLE_VISUALIZATOR
