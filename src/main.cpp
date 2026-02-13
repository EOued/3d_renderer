#include "matrix.hpp"
#include "vectors.hpp"
#include "quaternions.hpp"

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

inline void to_canonical_space(linalg::Vector<double, 3>& vector, int s_width,
                               int s_height)
{
  linalg::Vector<double, 2> center = {static_cast<double>(s_width) - 1,
                                      static_cast<double>(s_height) - 1};
  center *= 0.5f;
  int scale = std::min(s_width, s_height) - 1;
  vector[0] = (2.0f / scale) * (vector[0] - center[0]);
  vector[1] = -(2.0f / scale) * (vector[1] - center[1]);
}

linalg::Quaternion<double> compute_rotation(linalg::Vector<double, 3> startPos,
                                            linalg::Vector<double, 3> endPos)
{
  int radius = 1;

  linalg::Vector<double, 3> direction    = endPos - startPos;
  linalg::Vector<double, 3> rotationAxis = {-direction[1], direction[0], 0.0f};
  rotationAxis.normalise();

  // // Trackball initialisation
  linalg::Vector<double, 3> p = {startPos[0], startPos[1],
                                 trackball_z(startPos[0], startPos[1], radius)};
  linalg::Vector<double, 3> q = {endPos[0], endPos[1],
                                 trackball_z(endPos[0], endPos[1], radius)};

  // // Works for radius = 1

  double theta = std::acos((p.dot_product(q)) / (p.norm() * q.norm()));
  linalg::Vector<double, 3> n = p.cross_product(q);
  n.normalise();

  linalg::Quaternion<double> retval =
      linalg::Quaternion(std::cos(theta / 2), std::sin(theta / 2) * n);
  retval.normalize();
  return retval;
}

int main(int, char**)
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Event event;

  const int WIDTH = 320, HEIGHT = 240;

  SDL_CreateWindowAndRenderer("Renderer", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE,
                              &window, &renderer);

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

  std::vector<std::vector<linalg::Vector<double, 3>>> axes = {x_axis, y_axis,
                                                              z_axis};
  std::vector<std::vector<int>> colors                     = {
      {0xFF, 0x00, 0x00},
      {0x00, 0xFF, 0x00},
      {0x00, 0x00, 0xFF}
  };

  linalg::Quaternion<double> current_rotation =
      linalg::Quaternion<double>::Identity();
  linalg::Quaternion<double> drag_rotation =
      linalg::Quaternion<double>::Identity();

  linalg::Quaternion<double> total;
  linalg::Quaternion<double> total_inv;

  linalg::Vector<double, 3> startPos;
  linalg::Vector<double, 3> endPos;

  bool running = true, redraw = true;
  int width, height;

  while (running)
  {
    if (!redraw && SDL_WaitEvent(&event))
    {
      do
      {
        switch (event.type)
        {
        case SDL_EVENT_QUIT: running = false; break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
          SDL_GetWindowSize(window, &width, &height);
          startPos = {event.button.x, event.button.y};
          to_canonical_space(startPos, width, height);
          redraw = true;
          break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
          current_rotation = drag_rotation * current_rotation;
          current_rotation.normalize();
          drag_rotation = linalg::Quaternion<double>::Identity();
          redraw        = true;
          break;
        case SDL_EVENT_MOUSE_MOTION:
          if (event.motion.state & SDL_BUTTON_LMASK)
          {
            endPos = {event.motion.x, event.motion.y};
            to_canonical_space(endPos, width, height);
            drag_rotation = compute_rotation(window, startPos, endPos);
            redraw        = true;
          }
          break;
        }
      } while (SDL_PollEvent(&event));
    }
    if (redraw)
    {

      SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
      SDL_RenderClear(renderer);
      SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);

      // Convertion to canonical space

      total = drag_rotation * current_rotation;
      total.normalize();
      total_inv = total.inverse();

      for (int i = 0; i < 3; i++)
      {
        int width, height;
        SDL_GetWindowSize(window, &width, &height);

        std::vector<linalg::Vector<double, 3>> axe = axes[i];
        std::vector<int> color                     = colors[i];
        linalg::Vector<double, 3> l1               = axe[0];
        linalg::Vector<double, 3> l2               = axe[1];

        linalg::Quaternion<double> _l1 = linalg::Quaternion(l1);
        linalg::Quaternion<double> _l2 = linalg::Quaternion(l2);

        _l1 = total * _l1 * total_inv;
        _l2 = total * _l2 * total_inv;

        l1 = _l1._v();
        l2 = _l2._v();

        l1 = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l1;
        l2 = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l2;
        linalg::Vector<double, 2> L1 = {l1[0], l1[1]};
        linalg::Vector<double, 2> L2 = {l2[0], l2[1]};

        // // To 2D :

        double perspective = 100.0f / (100.0f + l1[2]);
        L1 *= perspective;
        L2 *= perspective;

        SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 0xFF);
        SDL_RenderLine(renderer, L1[0] + 10, L1[1] + 10, L2[0] + 10,
                       L2[1] + 10);
      }

      SDL_RenderPresent(renderer);
      redraw = false;
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
