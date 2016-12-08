//
//File: CM_AttackPriorities.h
//Author: Ivlev Alexander. Stef
//Created: 25/11/2016
//

#pragma once

#include "E_Wizard.h"
#include "model\Tree.h"
#include "model\Building.h"
#include "model\Minion.h"
#include "model\LaneType.h"
#include "E_Bullet.h"
#include "C_Vector2D.h"


namespace AICup
{

  // ќбъедин€ю в один файл, чтобы легче было настраивать приоритеты
  namespace AttackPriorities {
    double attackWizard(const Wizard& self, const model::Wizard& wizard, const Bullet* bullet);
    double attackMinion(const Wizard& self, const model::Minion& minion);
    double attackBuild(const Wizard& self, const model::Building& build);
    double attackTree(const Wizard& self, const model::Tree& tree);

    double attackFrostbolt(const Wizard& self);
    double attackFireball(const Wizard& self);

    double pool(const Wizard& self, const model::Minion& neutral);

  };
}

