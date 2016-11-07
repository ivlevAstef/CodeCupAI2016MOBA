//
//File: Extensions.h
//Description:
//Author: Ivlev Alexander. Stef
//Created: 10:16 28/12/2015
//Copyright (c) SIA 2015. All Right Reserved.
//

#pragma once


namespace AICup
{
  namespace Extensions
  {
    double constPI();

    template <typename T> int sign(T val) {
      return (T(0) < val) - (val < T(0));
    }

    #define MAX(a,b) (((a) > (b)) ? (a) : (b))
    #define MIN(a,b) (((a) < (b)) ? (a) : (b))

    #define SIA_PI constPI()

  };

};