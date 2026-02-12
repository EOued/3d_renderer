#include "basic.hpp"
#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <cmath>
#include <iostream>
#include <tuple>
#include <vector>

inline double trackball_z(const double x, const double y, const double radius)
{
  double x_2 = std::pow(x, 2);
  double y_2 = std::pow(y, 2);
  double r_2 = std::pow(radius, 2);
  // Smoothing when point exceed radius location
  return x_2 + y_2 <= r_2 / 2 ? std::sqrt(r_2 - x_2 - y_2)
                              : (r_2 / 2) / (std::sqrt(x_2 + y_2));
}

inline void to_canonical_space(linalg::Vector<double, 2>& vector, int s_width,
                               int s_height)
{
  linalg::Vector<double, 2> center = {static_cast<double>(s_width) - 1,
                                      static_cast<double>(s_height) - 1};
  center *= 1.0f / 2.0f;
  int scale = std::min(s_width, s_height) - 1;
  vector[0] = -(2.0f / scale) * (vector[0] - center[0]);
  vector[1] = -(2.0f / scale) * (vector[1] - center[1]);
}

int main(int, char**)
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* surface;
  SDL_Texture* texture;
  SDL_Event event;

  const int WIDTH = 320, HEIGHT = 240;

  SDL_CreateWindowAndRenderer("Renderer", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE,
                              &window, &renderer);
  surface = SDL_CreateSurface(WIDTH, HEIGHT, SDL_PIXELFORMAT_UNKNOWN);
  texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_DestroySurface(surface);

  double L    = 2.0f;
  double half = L / 2.0f;

  std::vector<linalg::Vector<double, 3>> front = {
      { half,  half, half},
      {-half,  half, half},
      {-half, -half, half},
      { half, -half, half},
  };

  std::vector<linalg::Vector<double, 3>> back = {
      { half, -half, -half}, // bottom-right
      {-half, -half, -half}, // bottom-left
      {-half,  half, -half}, // top-left
      { half,  half, -half}, // top-right
  };

  std::vector<linalg::Vector<double, 3>> left = {
      {-half,  half,  half}, // top-front
      {-half,  half, -half}, // top-back
      {-half, -half, -half}, // bottom-back
      {-half, -half,  half}, // bottom-front
  };

  std::vector<linalg::Vector<double, 3>> right = {
      {half,  half, -half}, // top-back
      {half,  half,  half}, // top-front
      {half, -half,  half}, // bottom-front
      {half, -half, -half}, // bottom-back
  };

  std::vector<linalg::Vector<double, 3>> top = {
      { half, half, -half}, // top-right back
      {-half, half, -half}, // top-left back
      {-half, half,  half}, // top-left front
      { half, half,  half}, // top-right front
  };

  std::vector<linalg::Vector<double, 3>> bottom = {
      { half, -half,  half}, // bottom-right front
      {-half, -half,  half}, // bottom-left front
      {-half, -half, -half}, // bottom-left back
      { half, -half, -half}, // bottom-right back
  };

  // Faces surfaces
  std::vector<linalg::Vector<double, 3>> x_axis = {
      {0, 0, 0},
      {2, 0, 0}
  };

  std::vector<linalg::Vector<double, 3>> y_axis = {
      {0, 0, 0},
      {0, 2, 0}
  };

  std::vector<linalg::Vector<double, 3>> z_axis = {
      {0, 0, 0},
      {0, 0, 2}
  };

  double origin2D_x = 100;
  double origin2D_y = 100;

  // Simulating cursor position
  linalg::Vector<double, 2> MouseFixedPoint    = {0.0f, 0.0f};
  linalg::Vector<double, 2> MouseRotatingPoint = {0.0f, 0.0f};
  linalg::Vector<double, 3> p, q, n;
  linalg::Quaternion<double> rot;
  linalg::Quaternion<double> rot_inv;
  int width, height, radius = 1;
  double theta;
  linalg::Vector<double, 2> center;
  while (1)
  {
    SDL_PollEvent(&event);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);

    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

    auto faces3d = {front, back, left, right, top, bottom};
    auto axes    = {x_axis, y_axis, z_axis};

    // Convertion to canonical space
    SDL_GetWindowSize(window, &width, &height);
    to_canonical_space(MouseFixedPoint, width, height);
    to_canonical_space(MouseRotatingPoint, width, height);

    // Trackball initialisation
    p = {MouseFixedPoint[0], MouseFixedPoint[1],
         trackball_z(MouseFixedPoint[0], MouseFixedPoint[1], radius)};
    q = {MouseRotatingPoint[0], MouseRotatingPoint[1],
         trackball_z(MouseRotatingPoint[0], MouseRotatingPoint[1], radius)};

    // Works for radius = 1

    theta = std::acos((p.dot_product(q)) / (p.norm() * q.norm()));
    n     = p.cross_product(q);
    n.normalise();

    rot     = linalg::Quaternion(std::cos(theta / 2), std::sin(theta / 2) * n);
    rot_inv = rot.inverse();

    // Rendering
    for (std::vector<linalg::Vector<double, 3>> face : faces3d)
    {
      for (long unsigned int i = 0; i < face.size(); i++)
      {
        linalg::Vector<double, 3> l1 = face[i];
        linalg::Vector<double, 3> l2 = face[(i + 1) % face.size()];

        l1 = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l1;
        l2 = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l2;

        linalg::Quaternion<double> _l1 = linalg::Quaternion(l1);
        linalg::Quaternion<double> _l2 = linalg::Quaternion(l2);

        _l1 = rot * _l1 * rot_inv;
        _l2 = rot * _l2 * rot_inv;

        l1 = _l1._v();
        l2 = _l2._v();

        // // To 2D :
        SDL_RenderLine(renderer, l1[0] + origin2D_x, l1[1] + origin2D_y,
                       l2[0] + origin2D_x, l2[1] + origin2D_y);
      }
    }

    for (std::vector<linalg::Vector<double, 3>> axe : axes)
    {
      linalg::Vector<double, 3> l1 = axe[0];
      linalg::Vector<double, 3> l2 = axe[1];

      l1 = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l1;
      l2 = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l2;

      linalg::Quaternion<double> _l1 = linalg::Quaternion(l1);
      linalg::Quaternion<double> _l2 = linalg::Quaternion(l2);

      _l1 = rot * _l1 * rot_inv;
      _l2 = rot * _l2 * rot_inv;

      l1 = _l1._v();
      l2 = _l2._v();

      // // To 2D :
      SDL_RenderLine(renderer, l1[0] + 10, l1[1] + 10, l2[0] + 10, l2[1] + 10);
    }
    SDL_RenderPresent(renderer);

    int running = 1;

    while (running == 1)
    {
      // Wait indefinitely for the next event
      if (SDL_WaitEvent(&event))
      { // blocks until an event arrives
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
          switch (event.key.key)
          {
          case SDLK_RIGHT: break;
          default: break;
          }
          running = 0; // exit loop after key press
          break;
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
          if (event.button.button == SDL_BUTTON_LEFT)
          {
            MouseFixedPoint    = {event.button.x, event.button.y};
            MouseRotatingPoint = {event.button.x, event.button.y};
          }
          running = 0;
          break;
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
          MouseFixedPoint    = {event.button.x, event.button.y};
          MouseRotatingPoint = {event.button.x, event.button.y};
          running            = 0;
          break;
        }
        if (event.type == SDL_EVENT_MOUSE_MOTION)
        {
          if (event.button.button == SDL_BUTTON_LEFT)
            MouseRotatingPoint = {event.button.x, event.button.y};
          running = 0;
          break;
        }
        if (event.type == SDL_EVENT_QUIT)
        {
          running = 2; // exit if window is closed
          break;
        }
      }
    }

    if (running == 2) break;
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
