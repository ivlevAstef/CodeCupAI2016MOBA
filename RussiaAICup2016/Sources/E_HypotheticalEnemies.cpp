#include "E_HypotheticalEnemies.h"
#include "E_World.h"
#include "C_Vector2D.h"
#include "C_Extensions.h"

using namespace AICup;

HypotheticalEnemies::HypotheticalEnemies() {
}

//static Position reverse(Position pos) {
//  return Position(pos.y, pos.x);
//}

void HypotheticalEnemies::update() {
  //TODO: потом переделать на волны, ибо такой вариант плох по многим причинам
  hypotheticalMinions.clear();
}

const std::vector<model::Minion>& HypotheticalEnemies::getHypotheticalMinions() const {
  return hypotheticalMinions;
}

#ifdef ENABLE_VISUALIZATOR
void HypotheticalEnemies::visualization(const Visualizator& visualizator) const {
  //if (Visualizator::POST == visualizator.getStyle()) {
  //  for (const auto& minion : hypotheticalMinions) {
  //    visualizator.fillCircle(minion.getX(), minion.getY(), minion.getRadius(), 0xff0066);
  //  }
  //}
}
#endif // ENABLE_VISUALIZATOR
