#include "E_WorldObjects.h"
#include <cmath>

using namespace AICup;


/// будет так, искреннне надеюсь что я не создаю за 20к тиков объектов превышающих long long
static long long calcId() {
  static long long idIndex = 100000;
  idIndex++;
  return 100000 + (idIndex - 100000) % 100000000;
}


Tree::Tree(const model::Tree& tree, double radius):
  model::Tree(tree.getId(),
    tree.getX(), tree.getY(), 0, 0, 0,
    model::FACTION_OTHER, radius, tree.getLife(), tree.getMaxLife(), std::vector<model::Status>())
{
}

Tree::~Tree() {
}


int lifeByRadius(double radius) {
  double a = (radius - 20.0) / 30.0;
  return 6 + int(round(30.0 * a * a));
}

//свое дерево, дабы чтобы было в неизвестных зонах
Tree::Tree(double x, double y, double radius):
  model::Tree(calcId(),
    x, y, 0, 0, 0,
    model::FACTION_OTHER, radius, lifeByRadius(radius), lifeByRadius(radius), std::vector<model::Status>())
{
}

Building::Building(const model::Building& building, double radius):
  model::Building(building.getId(),
    building.getX(), building.getY(), 0, 0, 0,
    building.getFaction(), radius,
    building.getLife(), building.getMaxLife(),
    std::vector<model::Status>(),
    building.getType(), building.getVisionRange(), building.getAttackRange(),
    building.getDamage(), building.getCooldownTicks(), building.getRemainingActionCooldownTicks()) {

}

//копия строения но c уменьшенным временем удара
Building::Building(const model::Building& building, int ticks):
  model::Building(building.getId(),
    building.getX(), building.getY(), 0, 0, 0,
    building.getFaction(), building.getRadius(),
    building.getLife(), building.getMaxLife(),
    std::vector<model::Status>(),
    building.getType(), building.getVisionRange(), building.getAttackRange(),
    building.getDamage(), building.getCooldownTicks(), ticks) {

}

Building::Building(double x, double y, model::Faction faction, const model::Building& building):
  model::Building(calcId(),
    x, y, 0, 0, 0,
    faction, building.getRadius(),
    building.getLife(), building.getMaxLife(),
    std::vector<model::Status>(),
    building.getType(), building.getVisionRange(), building.getAttackRange(),
    building.getDamage(), building.getCooldownTicks(), building.getRemainingActionCooldownTicks()) {

}

Minion::Minion(const model::Minion& minion, model::Faction faction):
  model::Minion(minion.getId(),
    minion.getX(), minion.getY(), 0, 0, 0,
    faction, minion.getRadius(),
    minion.getMaxLife(), minion.getMaxLife(),
    std::vector<model::Status>(),
    minion.getType(), minion.getVisionRange(),
    minion.getDamage(), minion.getCooldownTicks(), 0) {

}

Looking::Looking(const model::Wizard& unit) {
  x = unit.getX();
  y = unit.getY();
  visionRange = unit.getVisionRange();
}

Looking::Looking(const model::Minion& unit) {
  x = unit.getX();
  y = unit.getY();
  visionRange = unit.getVisionRange();
}

Looking::Looking(const model::Building& unit) {
  x = unit.getX();
  y = unit.getY();
  visionRange = unit.getVisionRange();
}