#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <iostream>

struct Camera
{
  float distance          = 20.0f;
  float targetDistance    = 20.0f;
  const float minDistance = 2.0f;
  const float maxDistance = 100.0f;
  const float zoomSpeed   = 3.0f;
  const float smoothing   = 8.0f;

  void zoomIn()
  {
    targetDistance = std::max(minDistance, targetDistance - zoomSpeed);
  }

  void zoomOut()
  {
    targetDistance = std::min(maxDistance, targetDistance + zoomSpeed);
  }

  void update(float deltaTime)
  {
    distance += (targetDistance - distance) * smoothing * deltaTime;
  }
};

#endif
