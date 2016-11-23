#include "E_HypotheticalEnemies.h"
#include "E_World.h"
#include "C_Vector2D.h"
#include "C_Extensions.h"

using namespace AICup;

HypotheticalEnemies::HypotheticalEnemies() {
}

static Position reverse(Position pos) {
  return Position(pos.y, pos.x);
}

void HypotheticalEnemies::update() {
  //TODO: потом переделать на волны, ибо такой вариант плох по многим причинам
  hypotheticalMinions.clear();

  /*for (const auto& minion : World::instance().minions()) {
    if (model::FACTION_ACADEMY == minion.getFaction()) {
      const auto reversePos = reverse(EX::pos(minion));

      bool canVision = false;
      for (const auto& vision : World::instance().getVisionZone()) {
        if ((Position(vision.getX(), vision.getY()) - reversePos).length() < vision.getVisionRange()) {
          canVision = true;
          break;
        }
      }

      if (!canVision) {
        hypotheticalMinions.push_back(Minion(minion, reversePos.x, reversePos.y, model::FACTION_RENEGADES));
      }
    }
  }*/
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
