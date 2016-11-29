#include "CM_CommandAvoidWizard.h"
#include "CM_CommandMoveToPoint.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"
#include "C_Math.h"

using namespace AICup;

CommandAvoidWizard::CommandAvoidWizard(Algorithm::PathFinder& finder, const model::Wizard& wizard) :
  MoveCommand(finder), wizard(wizard) {

}

bool CommandAvoidWizard::check(const Wizard& self) {
  const auto mc = Game::model();

  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = selfPos - wizardPos;

  needAvoidProjectile = false;
  //// для сокращения расчетов
  if (delta.length() > 650) {
    return false;
  }

  double finalDistance = 0;
  double finalCheckDistance = 0;

  const double distance2 = Algorithm::timeToTurnForAttack(self, wizard) * self.maxBackwardSpeed();
  const double distance3 = EX::frozenTime(wizard) * self.maxBackwardSpeed();

  const double castRange = wizard.getCastRange() + self.getRadius();
  if (EX::availableSkill(wizard, model::ACTION_MAGIC_MISSILE)) { ///magic missile
    const double mRadius = EX::radiusForGuaranteedDodge(self);

    const double distance1 = EX::cooldownMaxSkill(wizard, model::ACTION_MAGIC_MISSILE) * self.maxBackwardSpeed();
    const double maxDistance = MAX(distance1, MAX(distance2, distance3));
    const double mDistance = mRadius - maxDistance;
    const double mCheckDistance = castRange - maxDistance;

    finalDistance = MAX(finalDistance, mDistance);
    finalCheckDistance = MAX(finalCheckDistance, mCheckDistance);
  }

  if (EX::availableSkill(wizard, model::ACTION_FIREBALL)) {
    const double fRadius = EX::radiusForGuaranteedDodgeFireBall(self);

    const double distance1 = EX::cooldownMaxSkill(wizard, model::ACTION_FIREBALL) * self.maxBackwardSpeed();
    const double maxDistance = MAX(distance1, MAX(distance2, distance3));
    const double fDistance = fRadius - maxDistance;
    const double fCheckDistance = castRange - maxDistance;

    finalDistance = MAX(finalDistance, fDistance);
    finalCheckDistance = MAX(finalCheckDistance, fCheckDistance);
  }

  if (EX::availableSkill(wizard, model::ACTION_FROST_BOLT)) {
    const double fRadius = EX::radiusForGuaranteedDodgeFrostBolt(self);

    const double distance1 = EX::cooldownMaxSkill(wizard, model::ACTION_FROST_BOLT) * self.maxBackwardSpeed();
    const double maxDistance = MAX(distance1, MAX(distance2, distance3));
    const double fDistance = fRadius - maxDistance;
    const double fCheckDistance = castRange - maxDistance;

    finalDistance = MAX(finalDistance, fDistance);
    finalCheckDistance = MAX(finalCheckDistance, fCheckDistance);
  }

  if (EX::availableSkill(wizard, model::ACTION_STAFF)) {
    const double sRadius = Game::model().getStaffRange() + self.getRadius();
    const double distance1 = EX::cooldownMaxSkill(wizard, model::ACTION_STAFF) * self.maxBackwardSpeed();
    const double maxDistance = MAX(distance1, MAX(distance2, distance3));
    const double sDistance = sRadius - maxDistance;
    const double sCheckDistance = castRange - maxDistance;

    finalDistance = MAX(finalDistance, sDistance);
    finalCheckDistance = MAX(finalCheckDistance, sCheckDistance);
  }

  /// если далеко, то бояться его не стоит
  if (delta.length() > MAX(finalDistance, finalCheckDistance) + self.maxSpeed()) {
    return false;
  }

  /// если на пути дерево, то тоже бояться не стоит
  if (Algorithm::checkIntersectedTree(selfPos, wizardPos, Game::model().getMagicMissileRadius())) {
    return false;
  }

  auto direction = delta.perpendicular().normal();
  /// если угол больше 90 градусов то выбран не оптимальный из двух возможных перпендикуляров
  if (direction.dot(Vector(1, 0).rotate(self.getAngle())) < 0) {
    direction *= -1;
  }

  if (delta.length() < finalDistance + Algorithm::timeToTurn(self, direction.angle()) * self.maxBackwardSpeed()) {
    needAvoidProjectile = true;
  }


  /// точка где безопасно
  const auto pos = wizardPos + delta.normal() * finalDistance;
  moveToPointCommand = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y, TurnStyle::SIDE_TURN);

  return moveToPointCommand->check(self);
}

void CommandAvoidWizard::execute(const Wizard& self, Result& result) {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->execute(self, result);
  /// Воспользуемся свойством системы, чтобы всегда уворочиваться используя одного из магов, а не разных...
  result.turnPriority = TurnPriority::avoidWizard + wizard.getId();

  if (needAvoidProjectile) {
    const auto selfPos = EX::pos(self);
    const auto wizardPos = EX::pos(wizard);
    const auto delta = selfPos - wizardPos;

    auto direction = delta.perpendicular().normal();
    /// если угол больше 90 градусов то выбран не оптимальный из двух возможных перпендикуляров
    if (direction.dot(Vector(1, 0).rotate(self.getAngle())) < 0) {
      direction *= -1;
    }

    const auto minTimeForMagic = EX::minTimeForMagic(wizard);
    const auto minTimeForRotate = Algorithm::timeToTurnForAttack(self, wizard);
    const auto miTimeForUnFrozen = EX::frozenTime(wizard);

    const auto minTime = MAX(minTimeForMagic, MAX(minTimeForRotate, miTimeForUnFrozen));

    const auto ticksToRotate = Algorithm::timeToTurn(self, direction.angle());
    if (minTime < ticksToRotate) {
      result.deactivateOtherTurn = true;
    }
  }
}

double CommandAvoidWizard::priority(const Wizard& self) {
  return MovePriorities::avoidWizard(self, wizard) * self.getRole().getWizardPriority();
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidWizard::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->visualization(self, visualizator);
}
#endif // ENABLE_VISUALIZATOR