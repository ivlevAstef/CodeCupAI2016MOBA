//
//File: E_WorldObject.h
//Author: Ivlev Alexander. Stef
//Created: 18/11/2016
//


#pragma once

#include "model\Tree.h"
#include "model\Building.h"
#include "model\Wizard.h"
#include "model\Minion.h"

namespace AICup
{
  class Tree: public model::Tree {
  public:
    //копия дерева но слегка с увеличенным радиусом, для того чтобы наверняка
    Tree(const model::Tree& tree, double radius);

    //свое дерево, дабы чтобы было в неизвестных зонах
    Tree(double x, double y, double radius);

    ~Tree() override;
  };

  class Building: public model::Building {
  public:
    //копия строения но слегка с увеличенным радиусом, для того чтобы наверняка
    Building(const model::Building& building, double radius);
  };

  class Looking {
  public:
    Looking(const model::Wizard& unit);
    Looking(const model::Minion& unit);
    Looking(const model::Building& unit);

    double getVisionRange() const {
      return visionRange;
    }

    double getX() const {
      return x;
    }

    double getY() const {
      return y;
    }

  private:
    double x;
    double y;

    double visionRange;
  };
};
