//
//File: E_Graph.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "E_Graph.h"
#include "E_World.h"
#include "C_Vector2D.h"

using namespace AICup;

Graph::Join::Join(const Point* p1, const Point* p2): p1(p1), p2(p2),
  weight((Position(p1->x, p1->y) - Position(p2->x, p2->y)).length()) {
}

Graph::Graph() {
  initDefaultGraph();
}

void Graph::update() {
}

std::vector<Position> Graph::path(const Position& from, const Position& to) {
  /// save graph
  const size_t pointMemorySaveSize = pointMemory.size();
  const size_t joinMemorySaveSize = joinMemory.size();
  const size_t pointsSaveSize = points.size();


  auto point1 = addPoint(from);
  auto point2 = addPoint(to);

  /// restore graph
  pointMemory.resize(pointMemorySaveSize);
  joinMemory.resize(joinMemorySaveSize);
  points.resize(pointsSaveSize);
}

const Graph::PointWithJoins* Graph::addPoint(const Position& pos) {

  const Join* nearJoin = nullptr;
  double minLength = DBL_MAX;
  for (const auto& join : joinMemory) {
    auto pos1 = Position(join.p1->x, join.p1->y);
    auto pos2 = Position(join.p2->x, join.p2->y);

    auto vec1 = pos2 - pos1;
    auto vec2 = pos - pos1;
    auto length = abs(vec1.normal().cross(vec2));

    if (minLength > length) {
      minLength = length;
      nearJoin = &join;
    }
  }

  assert(nullptr != nearJoin);

  pointMemory.push_back({pos.x, pos.y});
  auto pointPtr = &pointMemory[pointMemory.size()-1];

  const Point* pointsPtr = pointMemory.data();
  joinMemory.push_back(Join(nearJoin->p2, pointPtr));
  joinMemory.push_back(Join(pointPtr, nearJoin->p2));

  points.push_back({pointPtr, {&joinMemory[joinMemory.size() - 1], &joinMemory[joinMemory.size() - 2]}});

  return &points[points.size() - 1];
}

std::vector<const Graph::PointWithJoins*> Graph::path(const PointWithJoins* from, const PointWithJoins* to) {
  std::vector<const Graph::PointWithJoins*> result;

  result.push_back(from);
}

void Graph::initDefaultGraph() {
  initDefaultPointMemory();
  initDefaultJoinMemory();
  updatePointsByMemory();
}


void Graph::initDefaultPointMemory() {
  const auto model = World::instance().model();
  const auto width = model.getWidth();
  const auto height = model.getHeight();
  const auto padding = 0.05;

  const auto left = width * padding;
  const auto right = width * (1.0 - padding);

  const auto top = height * padding;
  const auto bottom = height * (1.0 - padding);

  const auto pWidth = right - left;
  const auto pHeight = bottom - top;

  pointMemory.resize(_POINT_COUNT_);
  pointMemory[ACADEMY_BASE] = {left, bottom};
  pointMemory[DEFIANT_BASE] = {right, top};

  pointMemory[ACADEMY_TOP_FIRST_TOWER] = {left, top + pHeight * 0.4};
  pointMemory[ACADEMY_TOP_SECOND_TOWER] = {left, bottom - pHeight * 0.3};
  pointMemory[DEFIANT_TOP_FIRST_TOWER] = {left + pWidth * 0.4, top};
  pointMemory[DEFIANT_TOP_SECOND_TOWER] = {right - pWidth * 0.3, top};

  pointMemory[ACADEMY_BOTTOM_FIRST_TOWER] = {right - pWidth * 0.4, bottom};
  pointMemory[ACADEMY_BOTTOM_SECOND_TOWER] = {left + pWidth * 0.3, bottom};
  pointMemory[DEFIANT_BOTTOM_FIRST_TOWER] = {right, bottom - pHeight * 0.4};
  pointMemory[DEFIANT_BOTTOM_SECOND_TOWER] = {right, top + pHeight * 0.3};

  pointMemory[ACADEMY_MIDDLE_FIRST_TOWER] = {left + pWidth * 0.45, bottom - pHeight * 0.45};
  pointMemory[ACADEMY_MIDDLE_SECOND_TOWER] = {left + pWidth * 0.25, bottom - pHeight * 0.25};
  pointMemory[DEFIANT_MIDDLE_FIRST_TOWER] = {right - pWidth * 0.45, top + pHeight * 0.45};
  pointMemory[DEFIANT_MIDDLE_SECOND_TOWER] = {right - pWidth * 0.25, top + pHeight * 0.25};


  pointMemory[BONUS_TOP] = {width * 0.3, height * 0.3};
  pointMemory[BONUS_BOTTOM] = {width * 0.7, height * 0.7};


  pointMemory[TOP_CENTER] = {left, top};
  pointMemory[BOTTOM_CENTER] = {right, bottom};
  pointMemory[MIDDLE_CENTER] = {width * 0.5, height * 0.5};
}

void Graph::initDefaultJoinMemory() {
  const Point* points = pointMemory.data();


  joinMemory.push_back(Join(points + ACADEMY_BASE, points + ACADEMY_TOP_SECOND_TOWER));
  joinMemory.push_back(Join(points + ACADEMY_TOP_SECOND_TOWER, points + ACADEMY_TOP_FIRST_TOWER));
  joinMemory.push_back(Join(points + ACADEMY_TOP_FIRST_TOWER, points + TOP_CENTER));

  joinMemory.push_back(Join(points + ACADEMY_BASE, points + ACADEMY_BOTTOM_SECOND_TOWER));
  joinMemory.push_back(Join(points + ACADEMY_BOTTOM_SECOND_TOWER, points + ACADEMY_BOTTOM_FIRST_TOWER));
  joinMemory.push_back(Join(points + ACADEMY_BOTTOM_FIRST_TOWER, points + BOTTOM_CENTER));

  joinMemory.push_back(Join(points + ACADEMY_BASE, points + ACADEMY_MIDDLE_SECOND_TOWER));
  joinMemory.push_back(Join(points + ACADEMY_MIDDLE_SECOND_TOWER, points + ACADEMY_MIDDLE_FIRST_TOWER));
  joinMemory.push_back(Join(points + ACADEMY_MIDDLE_FIRST_TOWER, points + MIDDLE_CENTER));

  joinMemory.push_back(Join(points + DEFIANT_BASE, points + DEFIANT_TOP_SECOND_TOWER));
  joinMemory.push_back(Join(points + DEFIANT_TOP_SECOND_TOWER, points + DEFIANT_TOP_FIRST_TOWER));
  joinMemory.push_back(Join(points + DEFIANT_TOP_FIRST_TOWER, points + TOP_CENTER));

  joinMemory.push_back(Join(points + DEFIANT_BASE, points + DEFIANT_BOTTOM_SECOND_TOWER));
  joinMemory.push_back(Join(points + DEFIANT_BOTTOM_SECOND_TOWER, points + DEFIANT_BOTTOM_FIRST_TOWER));
  joinMemory.push_back(Join(points + DEFIANT_BOTTOM_FIRST_TOWER, points + BOTTOM_CENTER));

  joinMemory.push_back(Join(points + DEFIANT_BASE, points + DEFIANT_MIDDLE_SECOND_TOWER));
  joinMemory.push_back(Join(points + DEFIANT_MIDDLE_SECOND_TOWER, points + DEFIANT_MIDDLE_FIRST_TOWER));
  joinMemory.push_back(Join(points + DEFIANT_MIDDLE_FIRST_TOWER, points + MIDDLE_CENTER));

  joinMemory.push_back(Join(points + TOP_CENTER, points + BONUS_TOP));
  joinMemory.push_back(Join(points + MIDDLE_CENTER, points + BONUS_TOP));

  joinMemory.push_back(Join(points + BOTTOM_CENTER, points + BONUS_BOTTOM));
  joinMemory.push_back(Join(points + MIDDLE_CENTER, points + BONUS_BOTTOM));
}

void Graph::updatePointsByMemory() {
  points.clear();
  points.resize(pointMemory.size());

  const Point* pointsMemoryPtr = pointMemory.data();

  for (size_t index = 0; index < points.size(); ++index) {
    points[index].point = pointsMemoryPtr + index;
  }

  for (const auto& join : joinMemory) {
    size_t p1Index = (join.p1 - pointsMemoryPtr) / sizeof(Point);
    size_t p2Index = (join.p2 - pointsMemoryPtr) / sizeof(Point);

    points[p1Index].joins.push_back(&join);
    points[p2Index].joins.push_back(&join);
  }
}