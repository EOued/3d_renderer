#ifndef ARCBALL_HPP
#define ARCBALL_HPP

#include "quaternions.hpp"
#include "vectors.hpp"
#include <iostream>

class Arcball
{
public:
  Arcball() { computeCenter(); }
  Arcball(const int _width, const int _height) : width(_width), height(_height)
  {
    computeCenter();
  }
  Arcball(const int _radius) : radius(_radius) { computeCenter(); }
  Arcball(const int _width, const int _height, const int _radius)
      : width(_width), height(_height), radius(_radius)
  {
    computeCenter();
  }

  void updateDims(const int width, const int height);
  double trackball_z(const double x, const double y) const;

  void initRotation(const double x, const double y);
  void endRotation(void);
  void computeRotation(const double x, const double y);
  linalg::Vector<double, 3>
  rotate(const linalg::Vector<double, 3> vector) const;

protected:
  linalg::Vector<double, 2> center;
  int width;
  int height;
  int radius = 1;

  linalg::Vector<double, 2> startPos;
  linalg::Vector<double, 2> endPos;

  linalg::Quaternion<double> current_rotation =
      linalg::Quaternion<double>::Identity();
  linalg::Quaternion<double> drag_rotation =
      linalg::Quaternion<double>::Identity();

  void to_canonical_space(linalg::Vector<double, 2>& vector) const;
  inline void computeCenter(void)
  {
    this->center = {static_cast<double>(this->width) - 1,
                    static_cast<double>(this->height) - 1};
    this->center *= 0.5f;
  }
  inline void mouseToScreen(linalg::Vector<double, 2>& vector, const double x,
                            const double y)
  {
    vector = {x, y};
    to_canonical_space(vector);
  }
};

#endif
