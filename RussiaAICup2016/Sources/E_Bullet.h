//
//File: E_Bullet.h
//Author: Ivlev Alexander. Stef
//Created: 07/12/2016
//


#pragma once

#include "C_Vector2D.h"
#include "model\ProjectileType.h"
#include "model\Faction.h"

namespace AICup
{

  struct Bullet {
    /// а короче не знаю как без лишних проблем, сделать их const... писать геттеры сеттеры лень
    long long id;
    Vector speed;
    double radius;
    Position startPoint;
    Position pos;
    double range;
    model::ProjectileType type;
    model::Faction faction;

    Bullet(long long id, Vector speed, double radius, Position startPoint, Position pos, double range, model::ProjectileType type, model::Faction faction) :
      id(id), speed(speed), radius(radius), startPoint(startPoint), pos(pos), range(range), type(type), faction(faction) {
    }
  };

};