#include "arcball.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

void Arcball::to_canonical_space(glm::dvec2& vec) const
{
  int scale = std::min(this->width, this->height) - 1;
  vec.x     = (2.0f / scale) * (vec.x - this->center.x);
  vec.y     = -(2.0f / scale) * (vec.y - this->center.y);
}

void Arcball::updateDims(const int width, const int height)
{
  this->width  = width;
  this->height = height;
  computeCenter();
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

double Arcball::trackball_z(const glm::dvec2 vec) const
{
  return this->trackball_z(vec.x, vec.y);
}

void Arcball::initRotation(const double x, const double y)
{
  mouseToScreen(this->startPos, x, y);
}

void Arcball::endRotation(void)
{
  this->current_rotation = this->drag_rotation * this->current_rotation;
  this->current_rotation = glm::normalize(this->current_rotation);
  this->drag_rotation    = glm::identity<glm::dquat>();
}

void Arcball::computeRotation(const double x, const double y,
                              const bool is_rotation_only)
{

  mouseToScreen(this->endPos, x, y);

  glm::dvec3 p = {this->startPos, this->trackball_z(this->startPos)};
  glm::dvec3 q = {this->endPos, trackball_z(this->endPos)};

  double theta =
      std::acos((glm::dot(p, q) / (glm::length(p) * glm::length(q))));
  glm::dvec3 n = glm::normalize(glm::cross(p, q));
  n.z          = 0;
  if (is_rotation_only) n.x = 0;

  this->drag_rotation =
      glm::normalize(glm::dquat(std::cos(theta / 2), std::sin(theta / 2) * n));
}

glm::mat4 Arcball::rotate(void) const
{

  glm::dquat total_rotation = this->drag_rotation * this->current_rotation;
  total_rotation            = glm::normalize(total_rotation);
  return glm::mat4_cast(total_rotation);
}
