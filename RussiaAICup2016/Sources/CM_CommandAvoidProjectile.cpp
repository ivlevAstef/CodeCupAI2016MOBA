#include "CM_CommandAvoidProjectile.h"
#include "CM_CommandMoveToPoint.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"

using namespace AICup;

CommandAvoidProjectile::CommandAvoidProjectile(const model::Projectile& projectile): projectile(projectile) {
  turnStyle = TurnStyle::NO_TURN;
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


  auto perpedicular = speed.perpendicular().normal();
  /// ���� ���� ������ 90 �������� �� ������ �� ����������� �� ���� ��������� ���������������
  if (perpedicular.dot(Vector(1, 0).rotate(self.getAngle())) < 0) {
    perpedicular *= -1;
  }

  /// ���� ���� ���������� ��� ������ � � ��� �� ������ �� ������
  if (Algorithm::canSideForwardEscape(projectilePos, 600, self, speed, projectile.getRadius())) {
    position = selfPos + perpedicular.normal() * (distanceMoved + self.maxSpeed());
    turnStyle = TurnStyle::TURN;
    return true;
  }

  /// ���� ���� ���������� ��� ����� � � ��� �� ������ �� ������
  if (Algorithm::canSideBackwardEscape(projectilePos, 600, self, speed, projectile.getRadius())) {
    position = selfPos - perpedicular.normal() * (distanceMoved + self.maxBackwardSpeed());
    turnStyle = TurnStyle::BACK_TURN;
    return true;
  }

  auto direction = speed.normal();
  /// ���� ���� ������ 90 �������� �� ������ �� ����������� �� ���� ��������� ���������������
  if (direction.dot(Vector(1, 0).rotate(self.getAngle())) < 0) {
    direction *= -1;
  }

  /// ���� ���� ���������� ��� ����� �������
  if (Algorithm::canBackForwardEscape(projectilePos, 600, self, speed, projectile.getRadius())) {
    position = selfPos + direction.normal() * (distanceMoved + self.maxSpeed());
    turnStyle = TurnStyle::TURN;
    return true;
  }

  /// ���� ���� ���������� ��� ����� �����
  if (Algorithm::canBackBackwardEscape(projectilePos, 600, self, speed, projectile.getRadius())) {
    position = selfPos - direction.normal() * (distanceMoved + self.maxBackwardSpeed());
    turnStyle = TurnStyle::BACK_TURN;
    return true;
  }

  /// ����� �����...
  return false;
}

void CommandAvoidProjectile::execute(const Wizard& self, Result& result) {
  result.set(position, self);
  result.turnStyle = turnStyle;
  result.turnPriority = TurnPriority::avoidProjectile;
  result.deactivateOtherTurn = true;
}

double CommandAvoidProjectile::priority(const Wizard& self) {
  return MovePriorities::avoidProjectile(self, projectile);
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidProjectile::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, position.x, position.y, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR