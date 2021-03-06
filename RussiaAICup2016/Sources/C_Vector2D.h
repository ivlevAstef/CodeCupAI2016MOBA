//
//File: Vector2D.h
//Description:
//Author: Ivlev Alexander. Stef
//Created: 21:21 16/7/2015
//Copyright (c) SIA 2016. All Right Reserved.
//

#pragma once

#include <cmath>

namespace AICup
{
  template<typename Type>
  struct Vector2D {
  public:
    Vector2D() : x(0), y(0) {

    }
    Vector2D(Type xx, Type yy) {
      set(xx, yy);
    }
    Vector2D(const Type* array) {
      set(array);
    }

    Vector2D(const Vector2D<Type>& copy) {
      set(copy);
    }

    void negative() {
      x = -x;
      y = -y;
    }

    void scale(Type scalar) {
      x *= scalar;
      y *= scalar;
    }
    void scale(const Vector2D<Type>& scale) {
      x *= scale.x;
      y *= scale.y;
    }

    void set(Type xx, Type yy) {
      x = xx;
      y = yy;
    }
    void set(const Type* array) {
      x = array[0];
      y = array[1];
    }
    void set(const Vector2D<Type>& v) {
      x = v.x;
      y = v.y;
    }

    inline const Vector2D<Type> operator+(const Vector2D<Type>& v) const {
      return Vector2D<Type>(x + v.x, y + v.y);
    }
    inline Vector2D<Type>& operator+=(const Vector2D<Type>& v) {
      x += v.x;
      y += v.y;
      return *this;
    }

    inline const Vector2D<Type> operator-(const Vector2D<Type>& v) const {
      return Vector2D<Type>(x - v.x, y - v.y);
    }
    inline Vector2D<Type>& operator-=(const Vector2D<Type>& v) {
      x -= v.x;
      y -= v.y;
      return *this;
    }

    inline const Vector2D<Type> operator-() const {
      return Vector2D<Type>(-x, -y);
    }

    inline const Vector2D<Type> operator*(Type s) const {
      return Vector2D<Type>(x * s, y * s);
    }
    inline Vector2D<Type>& operator*=(Type s) {
      x *= s;
      y *= s;
      return *this;
    }

    inline const Vector2D<Type> operator/(Type s) const {
      return Vector2D<Type>(x / s, y / s);
    }

    inline const Vector2D<Type> operator/=(Type s) {
      x /= s;
      y /= s;
      return *this;
    }

    inline bool operator<(const Vector2D<Type>& v) const {
      return x < v.x && y < v.y;
    }
    inline bool operator>(const Vector2D<Type>& v) const {
      return x > v.x && y > v.y;
    }
    inline bool operator==(const Vector2D<Type>& v) const {
      return x == v.x && y == v.y;
    }
    inline bool operator!=(const Vector2D<Type>& v) const {
      return !(*this == v);
    }

    inline Type dot(const Vector2D<Type>& v) const {
      return x * v.x + y * v.y;
    }

    inline Type cross(const Vector2D<Type>& v) const {
      return x * v.y - y * v.x;
    }

    inline double length() const {
      return sqrt(x * x + y * y);
    }

    inline Type length2() const {
      return x * x + y * y;
    }

    inline Vector2D<Type>& normalize() {
      auto len = length();
      x /= len;
      y /= len;
      return *this;
    }

    inline Vector2D<Type> normal() const {
      auto len = length();
      return Vector2D<Type>(x / len, y / len);
    }

    inline Vector2D<Type> perpendicular() const {
      return Vector2D<Type>(-y, x);
    }

    Vector2D<Type>& rotate(double angle) {
      Type xNew = x * cos(angle) - y * sin(angle);
      Type yNew = y * cos(angle) + x * sin(angle);
      x = xNew;
      y = yNew;
      return *this;
    }

    Vector2D<Type> rotated(double angle) const {
      Type xNew = x * cos(angle) - y * sin(angle);
      Type yNew = y * cos(angle) + x * sin(angle);
      return Vector2D<Type>(xNew, yNew);
    }

    double angle() const {
      if (-1.0e-3 < y && y < 1.0e-3) {
        return x > 0 ? 0 : 3.14159265358979323846;
      }

      return atan2(y, x);
    }


  public:
    Type x;
    Type y;
  };


  template<> inline bool Vector2D<double>::operator==(const Vector2D<double>& v) const {
    return (*this - v).length2() < 1.0e-9;
  }

  typedef Vector2D<double> Position;
  typedef Vector2D<double> Vector;
};
