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

  //// для сокращения расчетов
  if (delta.length() > 650) {
    return false;
  }

  double finalDistance = 0;
  double finalCheckDistance = 0;

  const double castRange = wizard.getCastRange() + self.getRadius();
  if (EX::availableSkill(wizard, model::ACTION_MAGIC_MISSILE)) { ///magic missile
    const double mRadius = EX::radiusForGuaranteedDodge(self);

    const double mDistance1 = EX::cooldownMaxSkill(wizard, model::ACTION_MAGIC_MISSILE) * self.maxBackwardSpeed();
    const double mDistance2 = Algorithm::timeToTurnForAttack(self, wizard) * self.maxBackwardSpeed();
    const double mDistance = mRadius - MAX(mDistance1, mDistance2);
    const double mCheckDistance = castRange - MAX(mDistance1, mDistance2);

    finalDistance = MAX(finalDistance, mDistance);
    finalCheckDistance = MAX(finalCheckDistance, mCheckDistance);
  }

  if (EX::availableSkill(wizard, model::ACTION_FIREBALL)) {
    const double fRadius = EX::radiusForGuaranteedDodgeFrostBolt(self);

    const double fDistance1 = EX::cooldownMaxSkill(wizard, model::ACTION_FIREBALL) * self.maxBackwardSpeed();
    const double fDistance2 = Algorithm::timeToTurnForAttack(self, wizard) * self.maxBackwardSpeed();
    const double fDistance = fRadius - MAX(fDistance1, fDistance2);
    const double fCheckDistance = castRange - MAX(fDistance1, fDistance2);

    finalDistance = MAX(finalDistance, fDistance);
    finalCheckDistance = MAX(finalCheckDistance, fCheckDistance);
  }

  if (EX::availableSkill(wizard, model::ACTION_FROST_BOLT)) {
    const double fRadius = EX::radiusForGuaranteedDodgeFireBall(self);

    const double fDistance1 = EX::cooldownMaxSkill(wizard, model::ACTION_FROST_BOLT) * self.maxBackwardSpeed();
    const double fDistance2 = Algorithm::timeToTurnForAttack(self, wizard) * self.maxBackwardSpeed();
    const double fDistance = fRadius - MAX(fDistance1, fDistance2);
    const double fCheckDistance = castRange - MAX(fDistance1, fDistance2);

    finalDistance = MAX(finalDistance, fDistance);
    finalCheckDistance = MAX(finalCheckDistance, fCheckDistance);
  }

  if (EX::availableSkill(wizard, model::ACTION_STAFF)) {
    const double sRadius = Game::model().getStaffRange() + self.getRadius();
    const double sDistance1 = EX::cooldownMaxSkill(wizard, model::ACTION_STAFF) * self.maxBackwardSpeed();
    const double sDistance2 = Algorithm::timeToTurnForAttack(self, wizard) * self.maxBackwardSpeed();
    const double sDistance = sRadius - MAX(sDistance1, sDistance2);
    const double sCheckDistance = castRange - MAX(sDistance1, sDistance2);

    finalDistance = MAX(finalDistance, sDistance);
    finalCheckDistance = MAX(finalCheckDistance, sCheckDistance);
  }

  /// если далеко, то бояться его не стоит
  if (delta.length() > MAX(finalDistance, finalCheckDistance) + self.maxSpeed()) {
    return false;
  }


  /// точка где безопасно
  const auto pos = wizardPos + delta.normal() * finalDistance;
  moveToPointCommand = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y, TurnStyle::SIDE_TURN);

  return moveToPointCommand->check(self);
}

void CommandAvoidWizard::execute(const Wizard& self, Result& result) {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->execute(self, result);
  result.turnPriority = TurnPriority::avoidWizard;


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
  const auto minTime = MAX(minTimeForMagic, minTimeForRotate);

  const auto ticksToRotate = Algorithm::timeToTurn(self, direction.angle());
  if (minTime < ticksToRotate + 1) {
    result.deactivateOtherTurn = true;
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