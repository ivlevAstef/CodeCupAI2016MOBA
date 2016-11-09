//
//File: E_Graph.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "E_Graph.h"
#include "E_World.h"
#include "C_Vector2D.h"
#include "C_Extensions.h"
#include <unordered_set>

using namespace AICup;

#define CreateJoin(I1, I2) Join{I1, I2, (pointMemory[I1] - pointMemory[I2]).length()}

Graph::Graph() {
  initDefaultGraph();
}

void Graph::update() {
}

std::vector<Position> Graph::path(const Position& from, const Position& to, double& length) {
  assert(joinsForPoints.size() == pointMemory.size());
  /// save graph
  const auto pointMemorySave = pointMemory;
  const auto joinMemorySave = joinMemory;
  const auto joinsForPointsSave = joinsForPoints;

  // update graph
  const auto& pointFrom = addPoint(from);
  const auto& pointTo = addPoint(to);
  adaptPoints(pointFrom, pointTo);

  // calc path
  const auto& reversedPrivatePath = dijkstraPath(pointFrom, pointTo, length);

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
  const auto& pointIndex = pointMemory.size();
  pointMemory.push_back({pos.x, pos.y});
  joinsForPoints.push_back({ });


  /// calculate all length
  std::vector<double> lengths;
  lengths.resize(joinMemory.size());
  for (size_t joinIndex = 0; joinIndex < joinMemory.size(); joinIndex++) {
    const auto& join = joinMemory[joinIndex];

    const auto& pos1 = pointMemory[join.p1Index];
    const auto& pos2 = pointMemory[join.p2Index];


    lengths[joinIndex] = Extensions::distance(pos, pos1, pos2);
  }

  /// find nearest join

  size_t nearJoinIndex = UINT16_MAX;
  double minLength = DBL_MAX;
  for (size_t lenIndex = 0; lenIndex < lengths.size(); lenIndex++) {
    double length = lengths[lenIndex];
    if (minLength > length) {
      minLength = length;
      nearJoinIndex = lenIndex;
    }
  }

  assert(UINT16_MAX != nearJoinIndex);

  auto insertIndex = [this, &pointIndex] (size_t index) {
    joinMemory.push_back(CreateJoin(index, pointIndex));
    joinsForPoints[pointIndex].push_back(joinMemory.size() - 1);
    joinsForPoints[index].push_back(joinMemory.size() - 1);
  };

  /// added point for join
  insertIndex(joinMemory[nearJoinIndex].p1Index);
  insertIndex(joinMemory[nearJoinIndex].p2Index);

  return pointIndex;
}

void Graph::adaptPoints(const size_t p1Index, const size_t p2Index) {
  std::unordered_set<size_t> pIndexes;

  for (const auto& joinIndex : joinsForPoints[p1Index]) {
    const auto& join = joinMemory[joinIndex];
    pIndexes.insert((p1Index == join.p1Index) ? join.p2Index : join.p1Index);
  }

  size_t intersectCount = 0;
  for (const auto& joinIndex : joinsForPoints[p2Index]) {
    const auto& join = joinMemory[joinIndex];
    const auto& result = pIndexes.insert((p2Index == join.p1Index) ? join.p2Index : join.p1Index);
    intersectCount += result.second ? 1 : 0;
  }

  if (intersectCount > 2) {
    joinMemory.push_back(CreateJoin(p1Index, p2Index));
    joinsForPoints[p1Index].push_back(joinMemory.size() - 1);
    joinsForPoints[p2Index].push_back(joinMemory.size() - 1);
  }
}

struct DijkstraData {
  double weight;
  bool passed;
  size_t from;
};

std::vector<size_t> Graph::dijkstraPath(const size_t& fromPIndex, const size_t& toPIndex, double& length) {
  assert(pointMemory.size() == joinsForPoints.size());

  const auto& COUNT = pointMemory.size();

  /// Initialize
  std::vector<DijkstraData> data;
  data.resize(COUNT, {DBL_MAX, false, INT_MAX});

  size_t currentIndex = fromPIndex;
  data[currentIndex].weight = 0;

  /// Step

  do {
    const auto& currentWeight = data[currentIndex].weight;
    for (const auto& joinIndex : joinsForPoints[currentIndex]) {
      const auto& join = joinMemory[joinIndex];
      const auto index = (currentIndex == join.p1Index) ? join.p2Index : join.p1Index;

      const auto weight = currentWeight + join.weight;
      if (data[index].weight > weight) {
        data[index].weight = weight;
        data[index].from = currentIndex;
      }
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
  length = 0;
  do {
    result.push_back(currentIndex);
    length += data[currentIndex].weight;
    currentIndex = data[currentIndex].from;
  } while (currentIndex != INT_MAX);

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
  pointMemory[ACADEMY_BASE_TOP] = {left, bottom - height*0.15};
  pointMemory[ACADEMY_BASE_MIDDLE] = {left + width*0.12, bottom - height*0.12};
  pointMemory[ACADEMY_BASE_BOTTOM] = {left + width*0.15, bottom};

  pointMemory[DEFIANT_BASE] = {right, top};
  pointMemory[DEFIANT_BASE_TOP] = {right - width*0.15, top};
  pointMemory[DEFIANT_BASE_MIDDLE] = {right - width*0.12, top + height*0.12};
  pointMemory[DEFIANT_BASE_BOTTOM] = {right, top + height*0.15};

  pointMemory[ACADEMY_TOP_FIRST_TOWER] = {left, top + pHeight * 0.44};
  pointMemory[ACADEMY_TOP_SECOND_TOWER] = {left, bottom - pHeight * 0.3};
  pointMemory[DEFIANT_TOP_FIRST_TOWER] = {left + pWidth * 0.44, top};
  pointMemory[DEFIANT_TOP_SECOND_TOWER] = {right - pWidth * 0.3, top};

  pointMemory[ACADEMY_BOTTOM_FIRST_TOWER] = {right - pWidth * 0.44, bottom};
  pointMemory[ACADEMY_BOTTOM_SECOND_TOWER] = {left + pWidth * 0.3, bottom};
  pointMemory[DEFIANT_BOTTOM_FIRST_TOWER] = {right, bottom - pHeight * 0.44};
  pointMemory[DEFIANT_BOTTOM_SECOND_TOWER] = {right, top + pHeight * 0.3};

  pointMemory[ACADEMY_MIDDLE_FIRST_TOWER] = {left + pWidth * 0.42, bottom - pHeight * 0.42};
  pointMemory[ACADEMY_MIDDLE_SECOND_TOWER] = {left + pWidth * 0.23, bottom - pHeight * 0.23};
  pointMemory[DEFIANT_MIDDLE_FIRST_TOWER] = {right - pWidth * 0.42, top + pHeight * 0.42};
  pointMemory[DEFIANT_MIDDLE_SECOND_TOWER] = {right - pWidth * 0.23, top + pHeight * 0.23};


  pointMemory[BONUS_TOP] = {width * 0.3, height * 0.3};
  pointMemory[BONUS_BOTTOM] = {width * 0.7, height * 0.7};


  pointMemory[TOP_CENTER] = {left, top};
  pointMemory[BOTTOM_CENTER] = {right, bottom};
  pointMemory[MIDDLE_CENTER] = {width * 0.5, height * 0.5};

  pointMemory[CENTER_BONUS_TOP] = {width*0.5 - width*0.1, height*0.5 - height*0.1};
  pointMemory[CENTER_ACADEMY] = {width*0.5 - width*0.05, height*0.5 + height*0.05};
  pointMemory[CENTER_BONUS_BOTTOM] = {width*0.5 + width*0.1, height*0.5 + height*0.1};
  pointMemory[CENTER_DEFIANT] = {width*0.5 + width*0.05, height*0.5 - height*0.05};

  pointMemory[ACADEMY_TOP_CENTER] = {left, top + height * 0.12};
  pointMemory[BONUS_TOP_CENTER] = {left + width*0.13, top + height * 0.13};
  pointMemory[DEFIANT_TOP_CENTER] = {left + width*0.12, top};

  pointMemory[ACADEMY_BOTTOM_CENTER] = {right - width*0.12, bottom};
  pointMemory[BONUS_BOTTOM_CENTER] = {right - width*0.13, bottom - height*0.13};
  pointMemory[DEFIANT_BOTTOM_CENTER] = {right, bottom - height*0.12};
}

void Graph::initDefaultJoinMemory() {
  joinMemory.push_back(CreateJoin(ACADEMY_BASE, ACADEMY_BASE_TOP));
  joinMemory.push_back(CreateJoin(ACADEMY_BASE, ACADEMY_BASE_MIDDLE));
  joinMemory.push_back(CreateJoin(ACADEMY_BASE, ACADEMY_BASE_BOTTOM));

  joinMemory.push_back(CreateJoin(ACADEMY_BASE_TOP, ACADEMY_TOP_SECOND_TOWER));
  joinMemory.push_back(CreateJoin(ACADEMY_BASE_MIDDLE, ACADEMY_MIDDLE_SECOND_TOWER));
  joinMemory.push_back(CreateJoin(ACADEMY_BASE_BOTTOM, ACADEMY_BOTTOM_SECOND_TOWER));

  joinMemory.push_back(CreateJoin(ACADEMY_BASE_TOP, ACADEMY_BASE_MIDDLE));
  joinMemory.push_back(CreateJoin(ACADEMY_BASE_BOTTOM, ACADEMY_BASE_MIDDLE));

  joinMemory.push_back(CreateJoin(ACADEMY_TOP_SECOND_TOWER, ACADEMY_TOP_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(ACADEMY_TOP_FIRST_TOWER, ACADEMY_TOP_CENTER));
  joinMemory.push_back(CreateJoin(ACADEMY_TOP_CENTER, TOP_CENTER));
  joinMemory.push_back(CreateJoin(ACADEMY_TOP_CENTER, BONUS_TOP_CENTER));

  joinMemory.push_back(CreateJoin(ACADEMY_BOTTOM_SECOND_TOWER, ACADEMY_BOTTOM_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(ACADEMY_BOTTOM_FIRST_TOWER, ACADEMY_BOTTOM_CENTER));
  joinMemory.push_back(CreateJoin(ACADEMY_BOTTOM_CENTER, BOTTOM_CENTER));
  joinMemory.push_back(CreateJoin(ACADEMY_BOTTOM_CENTER, BONUS_BOTTOM_CENTER));

  joinMemory.push_back(CreateJoin(ACADEMY_MIDDLE_SECOND_TOWER, ACADEMY_MIDDLE_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(ACADEMY_MIDDLE_FIRST_TOWER, CENTER_ACADEMY));
  joinMemory.push_back(CreateJoin(CENTER_ACADEMY, CENTER_BONUS_TOP));
  joinMemory.push_back(CreateJoin(CENTER_ACADEMY, CENTER_BONUS_BOTTOM));
  joinMemory.push_back(CreateJoin(CENTER_ACADEMY, MIDDLE_CENTER));


  joinMemory.push_back(CreateJoin(DEFIANT_BASE, DEFIANT_BASE_TOP));
  joinMemory.push_back(CreateJoin(DEFIANT_BASE, DEFIANT_BASE_MIDDLE));
  joinMemory.push_back(CreateJoin(DEFIANT_BASE, DEFIANT_BASE_BOTTOM));

  joinMemory.push_back(CreateJoin(DEFIANT_BASE_TOP, DEFIANT_TOP_SECOND_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_BASE_MIDDLE, DEFIANT_MIDDLE_SECOND_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_BASE_BOTTOM, DEFIANT_BOTTOM_SECOND_TOWER));

  joinMemory.push_back(CreateJoin(DEFIANT_BASE_TOP, DEFIANT_BASE_MIDDLE));
  joinMemory.push_back(CreateJoin(DEFIANT_BASE_BOTTOM, DEFIANT_BASE_MIDDLE));

  joinMemory.push_back(CreateJoin(DEFIANT_TOP_SECOND_TOWER, DEFIANT_TOP_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_TOP_FIRST_TOWER, DEFIANT_TOP_CENTER));
  joinMemory.push_back(CreateJoin(DEFIANT_TOP_CENTER, TOP_CENTER));
  joinMemory.push_back(CreateJoin(DEFIANT_TOP_CENTER, BONUS_TOP_CENTER));

  joinMemory.push_back(CreateJoin(DEFIANT_BOTTOM_SECOND_TOWER, DEFIANT_BOTTOM_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_BOTTOM_FIRST_TOWER, DEFIANT_BOTTOM_CENTER));
  joinMemory.push_back(CreateJoin(DEFIANT_BOTTOM_CENTER, BOTTOM_CENTER));
  joinMemory.push_back(CreateJoin(DEFIANT_BOTTOM_CENTER, BONUS_BOTTOM_CENTER));

  joinMemory.push_back(CreateJoin(DEFIANT_MIDDLE_SECOND_TOWER, DEFIANT_MIDDLE_FIRST_TOWER));
  joinMemory.push_back(CreateJoin(DEFIANT_MIDDLE_FIRST_TOWER, CENTER_DEFIANT));
  joinMemory.push_back(CreateJoin(CENTER_DEFIANT, CENTER_BONUS_TOP));
  joinMemory.push_back(CreateJoin(CENTER_DEFIANT, CENTER_BONUS_BOTTOM));
  joinMemory.push_back(CreateJoin(CENTER_DEFIANT, MIDDLE_CENTER));


  joinMemory.push_back(CreateJoin(TOP_CENTER, CENTER_BONUS_TOP));
  joinMemory.push_back(CreateJoin(BONUS_TOP_CENTER, BONUS_TOP));
  joinMemory.push_back(CreateJoin(BONUS_TOP, CENTER_BONUS_TOP));
  joinMemory.push_back(CreateJoin(CENTER_BONUS_TOP, MIDDLE_CENTER));

  joinMemory.push_back(CreateJoin(BOTTOM_CENTER, CENTER_BONUS_BOTTOM));
  joinMemory.push_back(CreateJoin(BONUS_BOTTOM_CENTER, BONUS_BOTTOM));
  joinMemory.push_back(CreateJoin(BONUS_BOTTOM, CENTER_BONUS_BOTTOM));
  joinMemory.push_back(CreateJoin(CENTER_BONUS_BOTTOM, MIDDLE_CENTER));


  joinMemory.push_back(CreateJoin(ACADEMY_TOP_CENTER, DEFIANT_TOP_CENTER));
  joinMemory.push_back(CreateJoin(ACADEMY_BOTTOM_CENTER, DEFIANT_BOTTOM_CENTER));

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

#ifdef ENABLE_VISUALIZATOR
void Graph::visualization(const Visualizator& visualizator) const {
  for (const auto& join : joinMemory) {
    const auto& p1 = pointMemory[join.p1Index];
    const auto& p2 = pointMemory[join.p2Index];

    visualizator.line(p1.x, p1.y, p2.x, p2.y, 0x0000aa);
  }

  for (const auto& point : pointMemory) {
    visualizator.fillCircle(point.x, point.y, 10, 0x0000aa);
  }
}
#endif // ENABLE_VISUALIZATOR