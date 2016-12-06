#include "CM_CommandAvoidWizard.h"
#include "CM_CommandMoveToPoint.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"
#include "C_Math.h"
#include "A_WinPredictor.h"

using namespace AICup;

CommandAvoidWizard::CommandAvoidWizard(const model::Wizard& wizard): wizard(wizard) {
  distance = 0;
}

bool CommandAvoidWizard::check(const Wizard& self) {
  const auto mc = Game::model();

  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = selfPos - wizardPos;

  //// ��� ���������� ��������
  if (delta.length() > 650) {
    return false;
  }

  double finalDistance = 0;
  double finalCheckDistance = 0;

  const double distance2 = Algorithm::timeToTurnForAttack(self, wizard) * self.maxBackwardSpeed();
  const double distance3 = EX::frozenTime(wizard) * self.maxBackwardSpeed();

  const double castRange = wizard.getCastRange() + self.getRadius();
  if (EX::availableSkill(wizard, model::ACTION_MAGIC_MISSILE)) { ///magic missile
    const double mRadius = EX::radiusForGuaranteedDodge(self, 0);

    const double distance1 = EX::cooldownMaxSkill(wizard, model::ACTION_MAGIC_MISSILE) * self.maxBackwardSpeed();
    const double maxDistance = MAX(distance1, MAX(distance2, distance3));
    const double mDistance = mRadius - maxDistance;
    const double mCheckDistance = castRange - maxDistance + Game::model().getMagicMissileRadius();

    finalDistance = MAX(finalDistance, mDistance);
    finalCheckDistance = MAX(finalCheckDistance, mCheckDistance);
  }

  if (EX::availableSkill(wizard, model::ACTION_FIREBALL)) {
    const double fRadius = EX::radiusForGuaranteedDodgeFireBall(self, 0);

    const double distance1 = EX::cooldownMaxSkill(wizard, model::ACTION_FIREBALL) * self.maxBackwardSpeed();
    const double maxDistance = MAX(distance1, MAX(distance2, distance3));
    const double fDistance = fRadius - maxDistance;
    const double fCheckDistance = castRange - maxDistance + Game::model().getFireballRadius();

    finalDistance = MAX(finalDistance, fDistance);
    finalCheckDistance = MAX(finalCheckDistance, fCheckDistance);
  }

  if (EX::availableSkill(wizard, model::ACTION_FROST_BOLT)) {
    const double fRadius = 50/*������ ������ ��� ��������� �� ��� ��� ����� �����*/ + EX::radiusForGuaranteedDodgeFrostBolt(self, 0);

    const double distance1 = EX::cooldownMaxSkill(wizard, model::ACTION_FROST_BOLT) * self.maxBackwardSpeed();
    const double maxDistance = MAX(distance1, MAX(distance2, distance3));
    const double fDistance = fRadius - maxDistance;
    const double fCheckDistance = castRange - maxDistance + Game::model().getFrostBoltRadius();

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

  distance = MIN(finalDistance, finalCheckDistance);

  const double changeOfWin = Algorithm::changeOfWinning(self);
  if (changeOfWin > 0.5) {
    distance *= (1.0 - changeOfWin * 0.2);
  }

  /// ���� ������, �� ������� ��� �� �����
  if (delta.length() > distance + self.maxSpeed()) {
    return false;
  }

  /// ���� �� ���� ������, �� ���� ������� �� �����
  if (Algorithm::checkIntersectedTree(selfPos, wizardPos, Game::model().getMagicMissileRadius())) {
    return false;
  }

  return true;
}

void CommandAvoidWizard::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = selfPos - wizardPos;

  const auto pos = wizardPos + delta.normal() * distance;
  if (delta.length() < distance) {
    result.turnStyle = TurnStyle::BACK_TURN;
  } else {
    result.turnStyle = TurnStyle::TURN;
  }

  result.moveDirection = pos - selfPos;
  result.speedLimit = -1;
  result.turnPriority = TurnPriority::avoidWizard + int(delta.length()/2);
  result.deactivateOtherTurn = false;

  /// ���� �� ����� �� ���������� � ������� �� ������
  if (self.getLife() > self.getMaxLife()/2) {
    return;
  }

  // ���� � ������ ������ �� ������������� �� �����
  if (delta.length() > distance) {
    return;
  }

  auto direction = delta.perpendicular().normal();
  /// ���� ���� ������ 90 �������� �� ������ �� ����������� �� ���� ��������� ���������������
  if (direction.dot(Vector(1, 0).rotate(self.getAngle())) < 0) {
    direction *= -1;
  }

  const auto timeToTurn = Algorithm::timeToTurn(self, direction.angle());

  /// ���� ���� �� �������, �� ����� ����������� �����
  if (timeToTurn * EX::maxSpeed(wizard) + distance+15/*��������� �����*/ > delta.length()) {
    result.turnStyle = TurnStyle::SIDE_TURN;
    //result.deactivateOtherTurn = true;
  }
}

double CommandAvoidWizard::priority(const Wizard& self) {
  return MovePriorities::avoidWizard(self, wizard) * self.getRole().getAudacityWizard();
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidWizard::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
}
#endif // ENABLE_VISUALIZATOR