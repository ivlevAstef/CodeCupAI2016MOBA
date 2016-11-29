#include "CM_CommandAvoidProjectile.h"
#include "CM_CommandMoveToPoint.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"

using namespace AICup;

CommandAvoidProjectile::CommandAvoidProjectile(Algorithm::PathFinder& finder, const model::Projectile& projectile) :
  MoveCommand(finder), projectile(projectile) {

}

bool CommandAvoidProjectile::check(const Wizard& self) {
  const auto mc = Game::model();

  const auto selfPos = EX::pos(self);
  const auto projectilePos = EX::pos(projectile);
  const auto speed = Vector(projectile.getSpeedX(), projectile.getSpeedY());

  const auto distanceToLine = Math::distanceToLine(selfPos, projectilePos, projectilePos + speed);
  const auto distanceMoved = self.getRadius() + projectile.getRadius() - distanceToLine;

  /// ���� ������ �� � ���, � � �� ���� � ���� ����� �� ��� ������������
  if (distanceMoved < -self.maxSpeed()) {
    return false;
  }


  auto direction = speed.perpendicular().normal();
  /// ���� ���� ������ 90 �������� �� ������ �� ����������� �� ���� ��������� ���������������
  if (direction.dot(Vector(1, 0).rotate(self.getAngle())) < 0) {
    direction *= -1;
  }

  /// ���� ���� ���������� ��� ������ � � ��� �� ������ �� ������
  if (Algorithm::canForwardEscape(projectilePos, 600, self, speed, projectile.getRadius())) {
    /// ����� ��� ���������
    const auto pos = selfPos + direction.normal() * (distanceMoved + 2 * self.maxSpeed());

    moveToPointCommand = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y, TurnStyle::TURN);

    return moveToPointCommand->check(self);
  }

  /// ���� ���� ���������� ��� ����� � � ��� �� ������ �� ������
  if (Algorithm::canBackwardEscape(projectilePos, 600, self, speed, projectile.getRadius())) {
    /// ����� ��� ���������
    const auto pos = selfPos - direction.normal() * (distanceMoved + 2 * self.maxBackwardSpeed());

    moveToPointCommand = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y, TurnStyle::BACK_TURN);

    return moveToPointCommand->check(self);
  }

  /// ����� �����...
  return false;
}

void CommandAvoidProjectile::execute(const Wizard& self, Result& result) {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->execute(self, result);
  result.turnPriority = TurnPriority::avoidProjectile;
  result.deactivateOtherTurn = true;
}

double CommandAvoidProjectile::priority(const Wizard& self) {
  return MovePriorities::avoidProjectile(self, projectile);
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidProjectile::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->visualization(self, visualizator);
}
#endif // ENABLE_VISUALIZATOR