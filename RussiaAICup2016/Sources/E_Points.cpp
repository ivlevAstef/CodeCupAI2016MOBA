
#include "E_Points.h"
#include "E_World.h"

using namespace AICup;

Points::Points() {
  initPoints();
}

const Position& Points::point(PointType type) {
  assert(0 <= type && type < Points::instance().points.size());
  return Points::instance().points[type];
}

void Points::initPoints() {
  const auto width = World::size();
  const auto height = World::size();
  const auto padding = 0.05;

  const auto left = width * padding;
  const auto right = width * (1.0 - padding);

  const auto top = height * padding;
  const auto bottom = height * (1.0 - padding);

  const auto pWidth = right - left;
  const auto pHeight = bottom - top;

  points.resize(_POINT_COUNT_);

  points[ACADEMY_BASE] = {left, bottom};
  points[ACADEMY_BASE_TOP] = {left, bottom - height*0.15};
  points[ACADEMY_BASE_MIDDLE] = {left + width*0.12, bottom - height*0.12};
  points[ACADEMY_BASE_BOTTOM] = {left + width*0.15, bottom};

  points[RENEGADES_BASE] = {right, top};
  points[RENEGADES_BASE_TOP] = {right - width*0.15, top};
  points[RENEGADES_BASE_MIDDLE] = {right - width*0.12, top + height*0.12};
  points[RENEGADES_BASE_BOTTOM] = {right, top + height*0.15};

  points[ACADEMY_TOP_FIRST_TOWER] = {left, top + pHeight * 0.44};
  points[ACADEMY_TOP_SECOND_TOWER] = {left, bottom - pHeight * 0.3};
  points[RENEGADES_TOP_FIRST_TOWER] = {left + pWidth * 0.44, top};
  points[RENEGADES_TOP_SECOND_TOWER] = {right - pWidth * 0.3, top};

  points[ACADEMY_BOTTOM_FIRST_TOWER] = {right - pWidth * 0.44, bottom};
  points[ACADEMY_BOTTOM_SECOND_TOWER] = {left + pWidth * 0.3, bottom};
  points[RENEGADES_BOTTOM_FIRST_TOWER] = {right, bottom - pHeight * 0.44};
  points[RENEGADES_BOTTOM_SECOND_TOWER] = {right, top + pHeight * 0.3};

  points[ACADEMY_MIDDLE_FIRST_TOWER] = {left + pWidth * 0.42, bottom - pHeight * 0.42};
  points[ACADEMY_MIDDLE_SECOND_TOWER] = {left + pWidth * 0.23, bottom - pHeight * 0.23};
  points[RENEGADES_MIDDLE_FIRST_TOWER] = {right - pWidth * 0.42, top + pHeight * 0.42};
  points[RENEGADES_MIDDLE_SECOND_TOWER] = {right - pWidth * 0.23, top + pHeight * 0.23};


  points[BONUS_TOP] = {width * 0.3, height * 0.3};
  points[BONUS_BOTTOM] = {width * 0.7, height * 0.7};


  points[TOP_CENTER] = {left, top};
  points[MIDDLE_CENTER] = {width * 0.5, height * 0.5};
  points[BOTTOM_CENTER] = {right, bottom};

  points[CENTER_BONUS_TOP] = {width*0.5 - width*0.1, height*0.5 - height*0.1};
  points[CENTER_ACADEMY] = {width*0.5 - width*0.05, height*0.5 + height*0.05};
  points[CENTER_BONUS_BOTTOM] = {width*0.5 + width*0.1, height*0.5 + height*0.1};
  points[CENTER_RENEGADES] = {width*0.5 + width*0.05, height*0.5 - height*0.05};

  points[ACADEMY_TOP_CENTER] = {left, top + height * 0.12};
  points[BONUS_TOP_CENTER] = {left + width*0.13, top + height * 0.13};
  points[RENEGADES_TOP_CENTER] = {left + width*0.12, top};

  points[ACADEMY_BOTTOM_CENTER] = {right - width*0.12, bottom};
  points[BONUS_BOTTOM_CENTER] = {right - width*0.13, bottom - height*0.13};
  points[RENEGADES_BOTTOM_CENTER] = {right, bottom - height*0.12};
}

#ifdef ENABLE_VISUALIZATOR
void Points::visualization(const Visualizator& visualizator) const {
  if (Visualizator::PRE == visualizator.getStyle()) {
    for (const auto& point : points) {
      visualizator.fillCircle(point.x, point.y, 10, 0x0000aa);
    }
  }
}
#endif // ENABLE_VISUALIZATOR
