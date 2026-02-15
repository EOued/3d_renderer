#include "quaternions.hpp"
#include "arcball.hpp"
#include "vectors.hpp"

void Arcball::to_canonical_space(linalg::Vector<double, 2>& vector) const
{
  int scale = std::min(this->width, this->height) - 1;
  vector[0] = (2.0f / scale) * (vector[0] - center[0]);
  vector[1] = -(2.0f / scale) * (vector[1] - center[1]);
}

void Arcball::updateDims(const int width, const int height)
{
  this->width  = width;
  this->height = height;
}

double Arcball::trackball_z(const double x, const double y) const
{
  double x_2 = std::pow(x, 2);
  double y_2 = std::pow(y, 2);
  double r_2 = std::pow(this->radius, 2);
  // Smoothing when point exceed radius location
  return x_2 + y_2 <= r_2 / 2 ? std::sqrt(r_2 - x_2 - y_2)
                              : (r_2 / 2) / (std::sqrt(x_2 + y_2));
}

void Arcball::initRotation(const double x, const double y)
{
  mouseToScreen(this->startPos, x, y);
}

void Arcball::endRotation(void)
{
  current_rotation = drag_rotation * current_rotation;
  current_rotation.normalize();
  drag_rotation = linalg::Quaternion<double>::Identity();
}

void Arcball::computeRotation(const double x, const double y)
{

  mouseToScreen(this->endPos, x, y);

  linalg::Vector<double, 3> p = {
      this->startPos[0], this->startPos[1],
      this->trackball_z(this->startPos[0], this->startPos[1])};
  linalg::Vector<double, 3> q = {this->endPos[0], this->endPos[1],
                                 trackball_z(this->endPos[0], this->endPos[1])};
  double theta = std::acos((p.dot_product(q)) / (p.norm() * q.norm()));
  linalg::Vector<double, 3> n = p.cross_product(q);
  n.normalise();

  this->drag_rotation =
      linalg::Quaternion(std::cos(theta / 2), std::sin(theta / 2) * n);
  this->drag_rotation.normalize();
}

linalg::Vector<double, 3>
Arcball::rotate(const linalg::Vector<double, 3> vector) const
{
  linalg::Quaternion<double> total_rotation =
      this->drag_rotation * this->current_rotation;
  total_rotation.normalize();

  linalg::Quaternion<double> qVector = linalg::Quaternion(vector);
  return (total_rotation * qVector * total_rotation.inverse())._v();
}
