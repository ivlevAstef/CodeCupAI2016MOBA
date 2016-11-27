//
//File: Extensions.h
//Description:
//Author: Ivlev Alexander. Stef
//Created: 21:21 16/11/2016
//Copyright (c) SIA 2016. All Right Reserved.
//
#pragma once

#include "C_Vector2D.h"
#include "model/CircularUnit.h"

#include "model/Minion.h"
#include "model/Tree.h"
#include "model/Building.h"
#include "model/Wizard.h"
#include "model/ActionType.h"

namespace AICup
{
  namespace EX
  {
    inline Position pos(const model::Unit& unit) {
      return Position(unit.getX(), unit.getY());
    }

    bool isTree(const model::Unit& unit);
    bool isNeutral(const model::Unit& unit);
    bool isMinion(const model::Unit& unit);
    bool isBuilding(const model::Unit& unit);
    bool isWizard(const model::Unit& unit);
    bool isProjectile(const model::Unit& unit);

    const model::Tree& asTree(const model::Unit& unit);
    const model::Minion& asMinion(const model::Unit& unit);
    const model::Building& asBuilding(const model::Unit& unit);
    const model::Wizard& asWizard(const model::Unit& unit);

    double maxSpeed(const model::CircularUnit& obj);

    double timeToTurnForAttack(const model::Unit& attacked, const model::Wizard& attacking);

    ///Wizard
    double speedFactor(const model::Wizard& obj);
    double maxSpeed(const model::Wizard& obj);
    double maxStrafeSpeed(const model::Wizard& obj);
    double maxBackwardSpeed(const model::Wizard& obj);
    double turnSpeed(const model::Wizard& obj);

    double attackRadius(const model::Wizard& obj);
    double magicMissleAttack(const model::Wizard& obj);
    double staffAttack(const model::Wizard& obj);
    double armor(const model::Wizard& obj);

    std::vector<bool> availableSkills(const model::Wizard& obj);
    bool availableSkill(const model::Wizard& obj, model::ActionType action);
    int cooldownSkill(const model::Wizard& obj, model::ActionType action);

    //Support
    double radiusForGuaranteedHit(const model::Wizard& obj, const model::CircularUnit& enemy);
    double radiusForGuaranteedHitFrostBolt(const model::Wizard& obj, const model::CircularUnit& enemy);
    double radiusForGuaranteedHitFireBall(const model::Wizard& obj, const model::CircularUnit& enemy);

    int minTimeForMagic(const model::Wizard& obj);

  }
}