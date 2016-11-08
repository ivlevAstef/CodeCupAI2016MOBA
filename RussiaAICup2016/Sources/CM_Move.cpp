//
//File: CM_Move.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "CM_Move.h"
#include "E_Graph.h"

using namespace AICup;

std::vector<Position> AICup::path(const Position from, const Position to) {
  return Graph::instance().path(from, to);
}

MoveAction AICup::move(const MovableUnit unit, const Position& to) {
  return MoveAction();
}

MoveAction AICup::move(const MovableUnit unit, const std::vector<Position>& path) {
  return MoveAction();
}