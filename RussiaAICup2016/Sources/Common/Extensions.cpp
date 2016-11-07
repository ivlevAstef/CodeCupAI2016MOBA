//
//File: Extensions.cpp
//Description:
//Author: Ivlev Alexander. Stef
//Created: 10:16 28/12/2015
//Copyright (c) SIA 2015. All Right Reserved.
//


#include "Extensions.h"
#include <cmath>

namespace Extensions
{
  double constPI() {
    static const double PI = std::atan(1) * 4;
    return PI;
  }
}