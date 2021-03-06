#include "E_InfluenceMap.h"
#include "E_World.h"
#include "E_Game.h"
#include "E_HypotheticalEnemies.h"
#include "C_Math.h"
#include "E_Points.h"
#include "C_Logger.h"
#include "C_Extensions.h"
#include "A_Move.h"
#include <algorithm>

using namespace AICup;

Position InfluenceMapConstants::toReal(Vector2D<int> point, double dx, double dy) {
  return Position((point.x + dx) * step, (point.y + dy) * step);
}
Vector2D<int> InfluenceMapConstants::toInt(Position point) {
  return Vector2D<int>((int)floor(point.x / step), (int)floor(point.y / step));
}

InfluenceMap::InfluenceMap() {
  topForeFront = Points::point(Points::ACADEMY_TOP_CENTER);
  middleForeFront = Points::point(Points::MIDDLE_CENTER);
  bottomForeFront = Points::point(Points::ACADEMY_BOTTOM_CENTER);
  lastCalculateTick = -10000;
  clean();
}

void InfluenceMap::update(const model::Wizard& self) {
  if (lastCalculateTick + 5 < World::model().getTickIndex()) {
    clean();
    includeFriends();
    includeEnemies();
    includeHypotheticalEnemies();

    lastCalculateTick = World::model().getTickIndex();
  }

  updateLinePosition(self);
}

const float* const InfluenceMap::getFriendsMap() const {
  return reinterpret_cast<const float* const>(friends);
}
const float* const InfluenceMap::getEnemiesMap() const {
  return reinterpret_cast<const float* const>(enemies);
}

void InfluenceMap::updateLinePosition(const model::Wizard& self) {
  const float maxSpeed = 25;
  /// �� ��� ���� �� ���� ���������� ���� ��� ����...
  auto newTopForeFront = calculateForeFront(self, model::LANE_TOP);
  auto newMiddleForeFront = calculateForeFront(self, model::LANE_MIDDLE);
  auto newBottomForeFront = calculateForeFront(self, model::LANE_BOTTOM);

  float topDistance = (float)(newTopForeFront - topForeFront).length();
  float middleDistance = (float)(newMiddleForeFront - middleForeFront).length();
  float bottomDistance = (float)(newBottomForeFront - bottomForeFront).length();

  const auto newMove1TopForeFront = getForeFront(model::LANE_TOP, MIN(maxSpeed, topDistance));
  const auto newMove1MiddleForeFront = getForeFront(model::LANE_MIDDLE, MIN(maxSpeed, middleDistance));
  const auto newMove1BottomForeFront = getForeFront(model::LANE_BOTTOM, MIN(maxSpeed, bottomDistance));
  const auto newMove2TopForeFront = getForeFront(model::LANE_TOP, -MIN(maxSpeed, topDistance));
  const auto newMove2MiddleForeFront = getForeFront(model::LANE_MIDDLE, -MIN(maxSpeed, middleDistance));
  const auto newMove2BottomForeFront = getForeFront(model::LANE_BOTTOM, -MIN(maxSpeed, bottomDistance));

  if ((newTopForeFront - newMove1TopForeFront).length() < (newTopForeFront - newMove2TopForeFront).length()) {
    topForeFront = newMove1TopForeFront;
  } else {
    topForeFront = newMove2TopForeFront;
  }

  if ((newMiddleForeFront - newMove1MiddleForeFront).length() < (newMiddleForeFront - newMove2MiddleForeFront).length()) {
    middleForeFront = newMove1MiddleForeFront;
  } else {
    middleForeFront = newMove2MiddleForeFront;
  }

  if ((newBottomForeFront - newMove1BottomForeFront).length() < (newBottomForeFront - newMove2BottomForeFront).length()) {
    bottomForeFront = newMove1BottomForeFront;
  } else {
    bottomForeFront = newMove2BottomForeFront;
  }

}


Position InfluenceMap::getForeFront(const model::LaneType lane, const float offset) const {
  switch (lane) {
    case model::LANE_TOP:
      return Algorithm::offsetPointByPath(topForeFront, -offset, getLinePoints(lane));
    case model::LANE_MIDDLE:
      return Algorithm::offsetPointByPath(middleForeFront, -offset, getLinePoints(lane));
    case model::LANE_BOTTOM:
      return Algorithm::offsetPointByPath(bottomForeFront, -offset, getLinePoints(lane));
    default:
      break;
  }
  assert(false && "Incorrect lane type");
  return Position(0,0);
}

Vector InfluenceMap::getForeDirection(const model::LaneType lane, const Position foreFront) const {
  const auto& linePoints = getLinePoints(lane);

  for (size_t i = 1; i < linePoints.size(); i++) {
    if (Math::distanceToSegment(foreFront, linePoints[i - 1], linePoints[i]) < 1.0e-3) {
      return (linePoints[i - 1] - linePoints[i]).normal();
    }
  }

  assert(false && "can't found forefront line for fore front point...");
  return Vector(0, 0);
}

float InfluenceMap::getLineStrength(const model::LaneType lane) const {
  const std::vector<Position>& linePoints = getLinePoints(lane);

  float strength = 0;
  /// ���� �� ���� ������, ���� ���� �������������, ������ ��� ����� ������ ��� ��� �������� ���� �� ������
  for (size_t i = 1; i < linePoints.size(); i++) {
    auto p1 = InfluenceMapConstants::toInt(linePoints[i - 1]);
    auto p2 = InfluenceMapConstants::toInt(linePoints[i]);

    /// ������������ ����� ���� �� ����� �� ������� � �����
    p1.x = MAX(2, MIN(p1.x, InfluenceMapConstants::memorySize - 1 - 2));
    p1.y = MAX(2, MIN(p1.y, InfluenceMapConstants::memorySize - 1 - 2));
    p2.x = MAX(2, MIN(p2.x, InfluenceMapConstants::memorySize - 1 - 2));
    p2.y = MAX(2, MIN(p2.y, InfluenceMapConstants::memorySize - 1 - 2));

    const auto delta = p2 - p1;

    if (abs(delta.x) > abs(delta.y)) {
      int sign = SIGN(p2.x - p1.x);
      for (int x = p1.x; x != p2.x + sign; x += sign) {
        const int y = p1.y + ((x - p1.x) * delta.y) / delta.x;
        strength += zonePriority(x, y);
      }
    } else {
      int sign = SIGN(p2.y - p1.y);
      for (int y = p1.y; y != p2.y + sign; y += sign) {
        const int x = p1.x + ((y - p1.y) * delta.x) / delta.y;
        strength += zonePriority(x, y);
      }
    }
  }

  return strength;
}

float InfluenceMap::getStrength(const Position center, double radius) const {
  const size_t size = size_t(World::size() / InfluenceMapConstants::step);

  const float minRealX = float(MAX(0, center.x - radius) / InfluenceMapConstants::step);
  const float maxRealX = float(MIN(World::size(), center.x + radius) / InfluenceMapConstants::step);
  const float minRealY = float(MAX(0, center.y - radius) / InfluenceMapConstants::step);
  const float maxRealY = float(MIN(World::size(), center.y + radius) / InfluenceMapConstants::step);

  const size_t minX = (size_t)floor(minRealX);
  const size_t maxX = (size_t)ceil(maxRealX);
  const size_t minY = (size_t)floor(minRealY);
  const size_t maxY = (size_t)ceil(maxRealY);

  const float radius2 = radius* radius;

  float strength = 0;
  for (size_t x = minX; x < maxX; x++) {
    const float dx = ((float(x) + 0.5f) * InfluenceMapConstants::step) - center.x;

    for (size_t y = minY; y < maxY; y++) {
      const float dy = ((float(y) + 0.5f) * InfluenceMapConstants::step) - center.y;
      const float coef = (dx*dx + dy*dy) / radius2;

      strength += (1.0f - MIN(1.0f, coef)) * (friends[x][y] - enemies[x][y]);
    }
  }

  return strength;
}

const std::vector<Position>& InfluenceMap::getLinePoints(const model::LaneType lane) const {
  /// ������� �����, �� ������� ���� �����
  /// �� �����������, ��� �� ����� ���� ���� � ���� �����, � ��� �� ���� �����... ����� �� ����������
  static const std::vector<Position> topLinePoints = {
    Points::point(Points::ACADEMY_BASE),
    Points::point(Points::ACADEMY_TOP_CENTER),
    Points::point(Points::RENEGADES_TOP_CENTER),
    Points::point(Points::RENEGADES_BASE_TOP_MELEE)
  };

  static const std::vector<Position> middleLinePoints = {
    Points::point(Points::ACADEMY_BASE),
    Points::point(Points::RENEGADES_BASE_MIDDLE_MELEE)
  };

  static const std::vector<Position> bottomLinePoints = {
    Points::point(Points::ACADEMY_BASE),
    Points::point(Points::ACADEMY_BOTTOM_CENTER),
    Points::point(Points::RENEGADES_BOTTOM_CENTER),
    Points::point(Points::RENEGADES_BASE_BOTTOM_MELEE)
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

Position InfluenceMap::calculateForeFront(const model::Wizard& self, const model::LaneType lane) const {
  const std::vector<Position>& linePoints = getLinePoints(lane);

  /// ���� �� ���� ������, ���� ���� �������������, ������ ��� ����� ������ ��� ��� �������� ���� �� ������
  for (size_t i = 1; i < linePoints.size(); i++) {
    auto p1 = InfluenceMapConstants::toInt(linePoints[i - 1]);
    auto p2 = InfluenceMapConstants::toInt(linePoints[i]);

    /// ������������ ����� ���� �� ����� �� ������� � �����
    p1.x = MAX(2, MIN(p1.x, InfluenceMapConstants::memorySize - 1 - 2));
    p1.y = MAX(2, MIN(p1.y, InfluenceMapConstants::memorySize - 1 - 2));
    p2.x = MAX(2, MIN(p2.x, InfluenceMapConstants::memorySize - 1 - 2));
    p2.y = MAX(2, MIN(p2.y, InfluenceMapConstants::memorySize - 1 - 2));

    const auto delta = p2 - p1;

    if (abs(delta.x) > abs(delta.y)) {
      int sign = SIGN(p2.x - p1.x);
      for (int x = p1.x; x != p2.x+sign; x += sign) {
        int y = p1.y + ((x - p1.x) * delta.y) / delta.x;
        if (isEnemyZone(x, y)) {
          return pointToForeFront(self, x, y, linePoints);
        }
      }
    } else {
      int sign = SIGN(p2.y - p1.y);
      for (int y = p1.y; y != p2.y+sign; y += sign) {
        int x = p1.x + ((y - p1.y) * delta.x) / delta.y;
        if (isEnemyZone(x, y)) {
          return pointToForeFront(self, x, y, linePoints);
        }
      }
    }
  }

  //LogAssertMsg(false, "hmmmmm... not found friend zone? really");
  // �������� ������ ���� ����� �� ����� ����
  return linePoints[linePoints.size()-1];
}


float InfluenceMap::zonePriority(const int x, const int y) const {
  float maxPriority = 0;
  for (int nx = -3; nx <= 3; nx++) {
    for (int ny = -3; ny <= 3; ny++) {
      float priority = friends[x + nx][y + ny] - enemies[x + nx][y + ny];
      if (priority > maxPriority) {
        maxPriority = priority;
      }
    }
  }

  return maxPriority;
}

float InfluenceMap::zoneEnemyPriorityAndPoint(int& x, int& y) const {
  const int xB = x;
  const int yB = y;

  float maxPriority = -0.001;
  for (int nx = -3; nx <= 3; nx++) {
    for (int ny = -3; ny <= 3; ny++) {
      float priority = enemies[xB + nx][yB + ny] - friends[xB + nx][yB + ny];
      if (priority > maxPriority) {
        x = xB + nx;
        y = yB + ny;
        maxPriority = priority;
      }
    }
  }

  return maxPriority;
}


bool InfluenceMap::isEnemyZone(int& x, int& y) const {
  int enemyX = x;
  int enemyY = y;
  if (zoneEnemyPriorityAndPoint(enemyX, enemyY) > 0) {
    x = enemyX;
    y = enemyY;
    return true;
  }

  return false;
}

Position InfluenceMap::pointToForeFront(const model::Wizard& self, const int x, const int y, const std::vector<Position>& line) const {
  const Position startCentralPosition = InfluenceMapConstants::toReal({x, y}, 0.5, 0.5);

  double minDistance = 999999;
  Position minPoint = startCentralPosition;
  for (size_t i = 1; i < line.size(); i++) {
    Position point = Math::point_distanceToSegment(startCentralPosition, line[i - 1], line[i]);
    const double distance = (point - startCentralPosition).length2();
    if (distance < minDistance) {
      minPoint = point;
      minDistance = distance;
    }
  }

  return minPoint;
}


void InfluenceMap::clean() {
  for (size_t x = 0; x < InfluenceMapConstants::memorySize; x++) {
    for (size_t y = 0; y < InfluenceMapConstants::memorySize; y++) {
      friends[x][y] = InfluenceMapConstants::neutral;
      enemies[x][y] = InfluenceMapConstants::neutral;
    }
  }
}


float minionDps(const model::Minion& minion) {
  if (model::MINION_FETISH_BLOWDART == minion.getType()) {
    return Game::model().getDartDirectDamage() / float(minion.getCooldownTicks());
  }
  return Game::model().getOrcWoodcutterDamage() / float(minion.getCooldownTicks());
}

/// �������, �� ��� ������������� ������� ������ �� ������ ������� ����� ������ ������, ��� �� ����� ��������, � �� �������� ������
double friendMinionRadius(const model::Minion& minion) {
  return Game::model().getOrcWoodcutterAttackRange();
}

double minionRadius(const model::Minion& minion) {
  if (model::MINION_FETISH_BLOWDART == minion.getType()) {
    return Game::model().getFetishBlowdartAttackRange();
  }
  return Game::model().getOrcWoodcutterAttackRange();
}

float minionDanger(const model::Minion& minion) {
  return minion.getLife() * minionDps(minion) * 0.5f;
}

float buildDps(const model::Building& build) {
  return float(build.getDamage()) / float(build.getCooldownTicks());
}

double buildRadius(const model::Building& build) {
  return build.getAttackRange();
}

float buildDanger(const model::Building& build) {
  /// ��� ��� ����� ����� ������� ������, �� ����� ��������� ����������� ������ ����������
  return 0.025f * float(MIN(100, build.getLife())) * buildDps(build);
}

double baseRadius(const model::Building& build) {
  double ticks = 1 - ((double)build.getRemainingActionCooldownTicks() / (double)build.getCooldownTicks());
  return MAX(build.getRadius(), build.getAttackRange() * ticks);
}

float baseDanger(const model::Building& build) {
  double ticks = 1 - ((double)build.getRemainingActionCooldownTicks() / (double)build.getCooldownTicks());
  ticks = sqrt(ticks);
  /// ��� ��� ���� ����� ������� ������, �� � ��������� ���������� ������ ����������
  return 0.005f * float(MIN(300, build.getLife())) * buildDps(build) * ticks;
}

float wizardDps(const model::Wizard& wizard) {
  float mmDps = EX::magicMissleAttack(wizard) / float(Game::model().getWizardActionCooldownTicks());
  float ffDps = 0;
  if (EX::availableSkill(wizard, model::ACTION_FIREBALL)) {
    ffDps = 1.5f * float(Game::model().getFireballExplosionMinDamage()) / float(Game::model().getFireballCooldownTicks());
  }

  float fbDps = 0;
  if (EX::availableSkill(wizard, model::ACTION_FROST_BOLT)) {
    fbDps = 1.5f * float(Game::model().getFrostBoltDirectDamage()) / float(Game::model().getFrostBoltCooldownTicks());
  }

  return mmDps + ffDps + fbDps;
}

double wizardRadius(const model::Wizard& wizard) {
  return EX::attackRadius(wizard);
}

/// ���� �� ������ ������ ������ �� ����� ������
float wizardDanger(const model::Wizard& wizard) {
  float ticks = 1.0f - (EX::minTimeForMagic(wizard) / float(Game::model().getWizardActionCooldownTicks()));
  ticks = MAX(0.5f, ticks);

  float life = float(wizard.getLife()) / float(wizard.getMaxLife());
  return 0.75f * life * wizardDps(wizard) * ticks;
}

void InfluenceMap::includeFriends() {
  for (const auto& minion : World::instance().minions()) {
    if (Game::friendFaction() == minion.getFaction()) {
      includeFriend(minion, friendMinionRadius(minion), minionDanger(minion));
    }
  }

  /// ����� ����� ��������, ���� ������
  for (const auto& wizard : World::instance().wizards()) {
    if (Game::friendFaction() == wizard.getFaction() && wizard.getX() > 0) {
      includeFriend(wizard, wizardRadius(wizard), wizardDanger(wizard));

    }
  }

  for (const auto& build : World::instance().buildings()) {
    if (Game::friendFaction() == build.getFaction()) {
      includeFriend(build, buildRadius(build), buildDanger(build));
    }
  }

}
void InfluenceMap::includeEnemies() {
  for (const auto& minion : World::instance().minions()) {
    if (Game::enemyFaction() == minion.getFaction()) {
      includeEnemy(minion, minionRadius(minion), minionDanger(minion));

    }/* else if (model::FACTION_NEUTRAL == minion.getFaction()) {
      includeEnemy(minion, minionRadius(minion), 0.01 * minionDanger(minion));
    }*/
  }

  for (const auto& wizard : World::instance().wizards()) {
    if (Game::enemyFaction() == wizard.getFaction() && wizard.getX() < World::size()) {
      includeEnemy(wizard, wizardRadius(wizard), wizardDanger(wizard));

    }
  }

  for (const auto& build : World::instance().buildings()) {
    if (Game::enemyFaction() == build.getFaction()) {
      if (build.getType() == model::BUILDING_GUARDIAN_TOWER) {
        includeEnemy(build, buildRadius(build), buildDanger(build));
      } else { /// ��������� ����
        includeEnemy(build, baseRadius(build), baseDanger(build));
      }
    }
  }

}

void InfluenceMap::includeHypotheticalEnemies() {
  for (const auto& minion : HypotheticalEnemies::instance().getHypotheticalMinions()) {
    includeEnemy(minion, minionRadius(minion), minionDanger(minion));
  }

  double procent = 0;
  for (const auto& wave : HypotheticalEnemies::instance().nextWaveData(procent)) {
    procent *= 0.9; ///������ ��������
    for (const auto& minion : wave.minions) {
      includeEnemy(minion, minionRadius(minion), procent * minionDanger(minion));
    }
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
        const double fWeight = 100 + (friends[x][y] * 100);
        const double eWeight = 100 + (enemies[x][y] * 100);
        int32_t colorRed = MAX(100, MIN((int)(eWeight), 255));
        int32_t colorGreen = MAX(100, MIN((int)(fWeight), 255));
        visualizator.fillRect(x*3, y*3, (x+1)*3, (y+1)*3, (colorRed << 16) | (colorGreen << 8) | (100));
      }
    }
  }*/


  /*if (Visualizator::PRE == visualizator.getStyle()) {
    const auto size = InfluenceMapConstants::memorySize;
    for (int x = 0; x < size; x++) {
      for (int y = 0; y < size; y++) {
        const auto p1 = InfluenceMapConstants::toReal(Vector2D<int>(x, y));
        const auto p2 = InfluenceMapConstants::toReal(Vector2D<int>(x, y), 1, 1);

        const double fWeight = 100 + (friends[x][y] * 100);
        const double eWeight = 100 + (enemies[x][y] * 100);
        int32_t colorRed = MAX(100, MIN((int)(eWeight), 255));
        int32_t colorGreen = MAX(100, MIN((int)(fWeight), 255));
        visualizator.fillRect(p1.x, p1.y, p2.x, p2.y, (colorRed << 16) | (colorGreen << 8) | (100));
      }
    }
  }*/

  if (Visualizator::PRE == visualizator.getStyle()) {
    const auto topLine    = getForeFront(model::LANE_TOP, 0);
    const auto middleLine = getForeFront(model::LANE_MIDDLE, 0);
    const auto bottomLine = getForeFront(model::LANE_BOTTOM, 0);

    const auto topFriendLine    = getForeFront(model::LANE_TOP, 200);
    const auto middleFriendLine = getForeFront(model::LANE_MIDDLE, 200);
    const auto bottomFriendLine = getForeFront(model::LANE_BOTTOM, 200);

    const auto topEnemyLine    = getForeFront(model::LANE_TOP, -200);
    const auto middleEnemyLine = getForeFront(model::LANE_MIDDLE, -200);
    const auto bottomEnemyLine = getForeFront(model::LANE_BOTTOM, -200);

    visualizator.fillCircle(topFriendLine.x, topFriendLine.y, 50, 0xaaffaa);
    visualizator.fillCircle(middleFriendLine.x, middleFriendLine.y, 50, 0xaaffaa);
    visualizator.fillCircle(bottomFriendLine.x, bottomFriendLine.y, 50, 0xaaffaa);

    visualizator.fillCircle(topEnemyLine.x, topEnemyLine.y, 50, 0xffaaaa);
    visualizator.fillCircle(middleEnemyLine.x, middleEnemyLine.y, 50, 0xffaaaa);
    visualizator.fillCircle(bottomEnemyLine.x, bottomEnemyLine.y, 50, 0xffaaaa);

    visualizator.fillCircle(topLine.x, topLine.y, 50, 0xffffaa);
    visualizator.fillCircle(middleLine.x, middleLine.y, 50, 0xffffaa);
    visualizator.fillCircle(bottomLine.x, bottomLine.y, 50, 0xffffaa);
  }
}
#endif // ENABLE_VISUALIZATOR
