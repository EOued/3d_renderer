#ifndef ARCBALL_HPP
#define ARCBALL_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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
  double trackball_z(const glm::dvec2 vec) const;

  void initRotation(const double x, const double y);
  void endRotation(void);

  void computeRotation(const double x, const double y,
                       const bool is_rotation_only);
  glm::mat4 rotate(void) const;

protected:
  glm::dvec2 center;
  int width;
  int height;
  int radius = 1;

  glm::dvec2 startPos;
  glm::dvec2 endPos;

  glm::dquat current_rotation =
      glm::normalize(glm::dquat{0.948576, -0.3154, -0.0269403, 0});

  glm::dquat drag_rotation = glm::identity<glm::dquat>();

  void to_canonical_space(glm::dvec2& vector) const;
  inline void computeCenter(void)
  {
    this->center = {static_cast<double>(this->width) - 1,
                    static_cast<double>(this->height) - 1};
    this->center *= 0.5f;
  }
  inline void mouseToScreen(glm::dvec2& vector, const double x, const double y)
  {
    vector = {x, y};
    to_canonical_space(vector);
  }
};

#endif
