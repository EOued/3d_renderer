#include "basic.hpp"
#include <iostream>

#include <SDL3/SDL.h>
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
  vector[0] = (2.0f / scale) * (vector[0] - center[0]);
  vector[1] = -(2.0f / scale) * (vector[1] - center[1]);
}

inline linalg::Vector<double, 4>
quaternion_mult(const linalg::Vector<double, 4>& p,
                const linalg::Vector<double, 4>& q)
{
  linalg::Vector<double, 4> res;
  linalg::Vector<double, 3> v1 = {p[1], p[2], p[3]};
  linalg::Vector<double, 3> v2 = {q[1], q[2], q[3]};
  res[0] = p[0] * q[0] - v1.transpose().dot_product(v2.transpose());
  linalg::Vector<double, 3> v3 =
      v2 * p[0] + v1 * q[0] +
      v1.transpose().cross_product(v2.transpose()).transpose();
  res[1] = v3[0];
  res[2] = v3[1];
  res[3] = v3[2];
  return res;
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
  std::vector<linalg::Vector<int, 3>> x_axis = {
      {0, 0, 0},
      {2, 0, 0}
  };

  std::vector<linalg::Vector<int, 3>> y_axis = {
      {0, 0, 0},
      {0, 2, 0}
  };

  std::vector<linalg::Vector<int, 3>> z_axis = {
      {0, 0, 0},
      {0, 0, 2}
  };

  double origin2D_x = 100;
  double origin2D_y = 100;

  // Simulating cursor position
  linalg::Vector<double, 2> P = {100.0f, 100.0f};
  linalg::Vector<double, 2> Q = {100.0f, 100.0f};
  linalg::Vector<double, 3> p, q;
  linalg::Matrix<double, 3, 1> n;
  linalg::Vector<double, 4> quaternion_rot;
  linalg::Vector<double, 4> quaternion_rot_inverse;
  int width, height, radius = 1;
  double theta;
  linalg::Vector<double, 2> center;
  while (1)
  {
    std::cout << "------\n";
    SDL_PollEvent(&event);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);

    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

    auto faces3d = {front, back, left, right, top, bottom};
    // auto axes    = {x_axis, y_axis, z_axis};

    // Computing trackball rotation
    P = {100.0f, 500.0f};
    Q = {100.0f, 300.0f};

    // Convertion to canonical space
    SDL_GetWindowSize(window, &width, &height);
    to_canonical_space(P, width, height);
    to_canonical_space(Q, width, height);

    // Trackball initialisation
    p = {P[0], P[1], trackball_z(P[0], P[1], radius)};
    q = {Q[0], Q[1], trackball_z(Q[0], Q[1], radius)};

    // Works for radius = 1

    double p_norm =
        std::sqrt(std::pow(p[0], 2) + std::pow(p[1], 2) + std::pow(p[2], 2));
    double q_norm =
        std::sqrt(std::pow(q[0], 2) + std::pow(q[1], 2) + std::pow(q[2], 2));

    theta = std::acos((p.transpose().dot_product(q.transpose())) /
                      (p_norm * q_norm));
    n     = p.transpose().cross_product(q.transpose());
    n *= 1 /
         std::sqrt(std::pow(n[0], 2) + std::pow(n[1], 2) + std::pow(n[2], 2));

    std::cout << theta << "\n";

    quaternion_rot         = {std::cos(theta / 2), std::sin(theta / 2) * n[0],
                              std::sin(theta / 2) * n[1], std::sin(theta / 2) * n[2]};
    quaternion_rot_inverse = {quaternion_rot[0], -quaternion_rot[1],
                              -quaternion_rot[2], -quaternion_rot[3]};
    std::cout << "Rotation quaternion " << quaternion_rot << "\n";
    std::cout << "Rotation inverse quaternion " << quaternion_rot_inverse
              << "\n";

    // Rendering
    for (std::vector<linalg::Vector<double, 3>> face : faces3d)
    {
      for (long unsigned int i = 0; i < face.size(); i++)
      {
        linalg::Vector<double, 3> l1 = face[i];
        linalg::Vector<double, 3> l2 = face[(i + 1) % face.size()];

        l1 *= linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10});
        l2 *= linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10});
        linalg::Vector<double, 4> _l1 = {0, l1[0], l1[1], l1[2]};
        linalg::Vector<double, 4> _l2 = {0, l2[0], l2[1], l2[2]};

        std::cout << "l1 " << _l1 << "\n";
        std::cout << "l2 " << _l2 << "\n";

        _l1 = quaternion_mult(quaternion_mult(quaternion_rot, _l1),
                              quaternion_rot_inverse);

        _l2 = quaternion_mult(quaternion_mult(quaternion_rot, _l2),
                              quaternion_rot_inverse);
        std::cout << "Rotated l1 " << _l1 << "\n";
        std::cout << "Rotated l2 " << _l2 << "\n";

        // To 2D :
        SDL_RenderLine(renderer, _l1[1] + origin2D_x, _l1[2] + origin2D_y,
                       _l2[1] + origin2D_x, _l2[2] + origin2D_y);
      }
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
