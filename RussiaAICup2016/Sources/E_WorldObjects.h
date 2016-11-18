//
//File: E_WorldObject.h
//Author: Ivlev Alexander. Stef
//Created: 18/11/2016
//


#pragma once

#include "model\Tree.h"
#include "model\Building.h"
#include "model\LivingUnit.h"

namespace AICup
{
  class Tree: public model::Tree {
  public:
    //копия дерева но слегка с увеличенным радиусом, для того чтобы наверняка
    Tree(const Tree& tree, double radius);

    //свое дерево, дабы чтобы было в неизвестных зонах
    Tree(double x, double y, double radius);
  };

  class Building: public model::Building {
  public:
    //копия строения но слегка с увеличенным радиусом, для того чтобы наверняка
    Building(const model::Building& building, double radius);
  };

  class Looking {
  public:
    //копия строения но слегка с увеличенным радиусом, для того чтобы наверняка
    Looking(const model::Unit& unit, double visibleRange);

    double getVisibleRange() {
      return visibleRange;
    }

  private:
    double x;
    double y;

    double visibleRange;
  };
};
