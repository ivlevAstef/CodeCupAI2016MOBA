#include "S_AntiRushMath.h"

#include "E_InfluenceMap.h"
#include "E_Points.h"


using namespace AICup;

Position AntiRushMath::laneToPosition(model::LaneType lane) {
  const Position midPosition = InfluenceMap::instance().getForeFront(model::LANE_MIDDLE);

  if (model::LANE_MIDDLE == lane) {
    return midPosition;
  } else {
    const Position prePosition = (model::LANE_BOTTOM == lane) ?
      Points::point(Points::BONUS_BOTTOM) + Vector(-45, 45) :
      Points::point(Points::BONUS_TOP) + Vector(-45, 45);

    /// находимся на дистанции чтобы не палиться лишний раз
    return midPosition + (prePosition - midPosition).normal() * 640;
  }
}