//
//File: A_WinPredictor.h
//Author: Ivlev Alexander. Stef
//Created: 06/12/2016
//


#pragma once

#include "C_Vector2D.h"
#include "E_Wizard.h"
#include "model\Move.h"
#include "E_Types.h"

namespace AICup
{
  namespace Algorithm
  {
    /// ������������� ��������� ������� ���� ������ � ����� ���� � ����� ���� �� ������ � �����
    /// ��������� 1 ���� ����� 100%, -1 ���� ��� ����� �����
    double changeOfWinning(const Wizard& self, const double x = -1, const double y = -1);
  };

};