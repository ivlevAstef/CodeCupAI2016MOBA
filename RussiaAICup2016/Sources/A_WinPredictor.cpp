#include "A_WinPredictor.h"
#include "C_Math.h"
#include "E_InfluenceMap.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Extensions.h"

using namespace AICup;

double Algorithm::changeOfWinning(const model::Wizard& self, double friendMult, double enemyMult, const double x, const double y, double strengthMult) {
  auto pos = EX::pos(self);
  if (x > 0 && y > 0) {
    pos = Position(x, y);
  }

  double wizardSummaryDanger = 0;
  double minDinstance = 300;
  for (const auto& wizard : World::instance().wizards()) {
    if (self.getId() == wizard.getId()) {
      wizardSummaryDanger += EX::danger(wizard);
    } else if (self.getFaction() == wizard.getFaction()) {
      /// ���� �������, �� �� ������ ���������� � ������������ ������ �����
      if ((EX::pos(wizard) - pos).length() < (EX::pos(self) - pos).length() + 50) {
        wizardSummaryDanger += friendMult * EX::danger(wizard);
      }
    } else {
      /// ���� ����, �� �� ������ ���������� ����� � ������ �������� ������
      double distance = (EX::pos(wizard) - pos).length();
      if (distance < 500) {
        wizardSummaryDanger -= enemyMult * EX::danger(wizard);
      }
      minDinstance = MIN(distance, minDinstance);
    }
  }

  /// ���������� ������� ���, ��� ��� ���� ���� �� ���� ������ ��, �� �� ����� ������� ���� ������ ������, � ������ �����
  auto center = EX::pos(self) + (pos - EX::pos(self)) * 0.5;
  float strength = InfluenceMap::instance().getStrength(center, minDinstance) / 6;

  double priority = wizardSummaryDanger + strengthMult * double(strength);

  /// �� ����� ����� ��������� ��� - ���� ��� �� ������� ���� ������� ������ ������, �� ���� �� ������ (����� �� ������) ����� �������.
  return INTERVAL(-1, priority / 30, 1);
}

double Algorithm::symmetrialChangeOfWinning(const model::Wizard& self, const model::Wizard& enemy, double friendMult, double enemyMult) {
  double friendChangeOfWinning = changeOfWinning(self, friendMult, enemyMult, enemy.getX(), enemy.getY(), 1);
  /// friendMult � enemyMult �� ��������������
  double enemyChangeOfWinning = changeOfWinning(enemy, friendMult, enemyMult, self.getX(), self.getY(), -1);

  return (friendChangeOfWinning - enemyChangeOfWinning) * 0.5;
}