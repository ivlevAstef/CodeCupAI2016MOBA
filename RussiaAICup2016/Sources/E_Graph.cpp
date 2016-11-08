//
//File: E_Graph.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "E_Graph.h"
#include "E_World.h"
#include "C_Vector2D.h"
#include "C_Extensions.h"

using namespace AICup;

#define CreateJoin(I1, I2) Join{I1, I2, (pointMemory[I1] - pointMemory[I2]).length()}

Graph::Graph() {
  initDefaultGraph();
}

void Graph::update() {
}

std::vector<Position> Graph::path(const Position& from, const Position& to) {
  assert(joinsForPoints.size() == pointMemory.size());
  /// save graph
  const auto pointMemorySave = pointMemory;
  const auto joinMemorySave = joinMemory;
  const auto joinsForPointsSave = joinsForPoints;

  // update graph
  const auto& pointFrom = addPoint(from);
  const auto& pointTo = addPoint(to);

  // calc path
  const auto& reversedPrivatePath = dijkstraPath(pointFrom, pointTo);

  /// Create result
  std::vector<Position> result;
  result.reserve(reversedPrivatePath.size());

  for (size_t index = 0; index < reversedPrivatePath.size(); index++) {
    const auto& pIndex = reversedPrivatePath[reversedPrivatePath.size() - index - 1];
    const auto& point = pointMemory[pIndex];
    result.push_back(point);
  }

  /// restore graph
  pointMemory = pointMemorySave;
  joinMemory = joinMemorySave;
  joinsForPoints = joinsForPointsSave;

  return result;
}

const size_t Graph::addPoint(const Position& pos) {
  /// find nearest join
  const Join* nearJoin = nullptr;
  double minLength = DBL_MAX;
  for (const auto& join : joinMemory) {
    const auto& pos1 = pointMemory[join.p1Index];
    const auto& pos2 = pointMemory[join.p2Index];


    auto length = Extensions::distance(pos, pos1, pos2);

    if (minLength > length) {
      minLength = length;
      nearJoin = &join;
    }
  }

  assert(nullptr != nearJoin);

  /// added point for join
  const auto& pointIndex = pointMemory.size();
  pointMemory.push_back({pos.x, pos.y});

  joinMemory.push_back(CreateJoin(nearJoin->p1Index, pointIndex));
  joinMemory.push_back(CreateJoin(nearJoin->p2Index, pointIndex));

  joinsForPoints.push_back({joinMemory.size() - 2, joinMemory.size() - 1});
  joinsForPoints[nearJoin->p1Index].push_back(joinMemory.size() - 2);
  joinsForPoints[nearJoin->p2Index].push_back(joinMemory.size() - 1);

  return joinsForPoints.size() - 1;
}

struct DijkstraData {
  double weight;
  bool passed;
};

std::vector<size_t> Graph::dijkstraPath(const size_t& fromPIndex, const size_t& toPIndex) {
  assert(pointMemory.size() == joinsForPoints.size());

  const auto& COUNT = pointMemory.size();

  /// Initialize
  std::vector<DijkstraData> data;
  data.resize(COUNT, {DBL_MAX, false});

  size_t currentIndex = fromPIndex;
  data[currentIndex].weight = 0;

  /// Step

  do {
    const auto& currentWeight = data[currentIndex].weight;
    for (const auto& joinIndex : joinsForPoints[currentIndex]) {
      const auto& join = joinMemory[joinIndex];
      const auto index = (currentIndex == join.p1Index) ? join.p2Index : join.p1Index;

      data[index].weight = MIN(data[index].weight, currentWeight + join.weight);
    }
    data[currentIndex].passed = true;

    auto minWeight = DBL_MAX;
    currentIndex = UINT16_MAX;
    for (size_t index = 0; index < COUNT; index++) {
      if (!data[index].passed && data[index].weight < minWeight) {
        minWeight = data[index].weight;
        currentIndex = index;
      }
    }
  } while (currentIndex != UINT16_MAX);

  /// reverse path
  std::vector<size_t> result;

  currentIndex = toPIndex;
  do {
    result.push_back(currentIndex);
    const auto& currentWeight = data[currentIndex].weight;

    for (const auto& joinIndex : joinsForPoints[currentIndex]) {
      const auto& join = joinMemory[joinIndex];
      const auto index = (currentIndex == join.p1Index) ? join.p2Index : join.p1Index;

      if (abs(currentWeight - data[index].weight - join.weight) < 1.0e-3) {
        currentIndex = index;
        goto DIJKSTRA_REVERSE_PATH_GOTO;
      }
    }

    break; // exit
    DIJKSTRA_REVERSE_PATH_GOTO:;
  } while (true);

  /// if found path
  if (result[result.size() - 1] == fromPIndex) {
    return result; // reversed
  }

  return std::vector<size_t>();
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
  joinMemory.push_back(CreateJoin(ACADEMY_BASE, ACADEMY_TOP_SECOND_TOWER));
  joinMemory.push_back(CreateJoin(ACADEMY_TOP_SECOND_TOWER, ACADEMY_TOP_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(ACADEMY_TOP_FIRST_TOWER, TOP_CENTER));

  joinMemory.push_back(CreateJoin(ACADEMY_BASE, ACADEMY_BOTTOM_SECOND_TOWER));
  joinMemory.push_back(CreateJoin( ACADEMY_BOTTOM_SECOND_TOWER, ACADEMY_BOTTOM_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(ACADEMY_BOTTOM_FIRST_TOWER, BOTTOM_CENTER));

  joinMemory.push_back(CreateJoin(ACADEMY_BASE, ACADEMY_MIDDLE_SECOND_TOWER));
  joinMemory.push_back(CreateJoin(ACADEMY_MIDDLE_SECOND_TOWER, ACADEMY_MIDDLE_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(ACADEMY_MIDDLE_FIRST_TOWER, MIDDLE_CENTER));

  joinMemory.push_back(CreateJoin(DEFIANT_BASE, DEFIANT_TOP_SECOND_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_TOP_SECOND_TOWER, DEFIANT_TOP_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_TOP_FIRST_TOWER, TOP_CENTER));

  joinMemory.push_back(CreateJoin(DEFIANT_BASE, DEFIANT_BOTTOM_SECOND_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_BOTTOM_SECOND_TOWER, DEFIANT_BOTTOM_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_BOTTOM_FIRST_TOWER, BOTTOM_CENTER));

  joinMemory.push_back(CreateJoin(DEFIANT_BASE, DEFIANT_MIDDLE_SECOND_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_MIDDLE_SECOND_TOWER, DEFIANT_MIDDLE_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_MIDDLE_FIRST_TOWER, MIDDLE_CENTER));

  joinMemory.push_back(CreateJoin(TOP_CENTER, BONUS_TOP));
  joinMemory.push_back(CreateJoin(MIDDLE_CENTER, BONUS_TOP));

  joinMemory.push_back(CreateJoin(BOTTOM_CENTER, BONUS_BOTTOM));
  joinMemory.push_back(CreateJoin(MIDDLE_CENTER, BONUS_BOTTOM));
}

void Graph::updatePointsByMemory() {
  joinsForPoints.clear();
  joinsForPoints.resize(pointMemory.size());

  for (size_t index = 0; index < joinMemory.size(); index++) {
    const auto& join = joinMemory[index];

    joinsForPoints[join.p1Index].push_back(index);
    joinsForPoints[join.p2Index].push_back(index);
  }
}