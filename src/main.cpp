#include "matrix.hpp"
#include "vectors.hpp"
#include "quaternions.hpp"
#include "arcball.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <cmath>
#include <iostream>
#include <tuple>
#include <vector>

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

  auto faces3d = {front, back, left, right, top, bottom};
  std::vector<std::vector<linalg::Vector<double, 3>>> axes = {x_axis, y_axis,
                                                              z_axis};
  std::vector<std::vector<int>> colors                     = {
      {0xFF, 0x00, 0x00},
      {0x00, 0xFF, 0x00},
      {0x00, 0x00, 0xFF}
  };

  bool running = true, redraw = true;
  int width, height;
  SDL_GetWindowSize(window, &width, &height);
  Arcball arcball = Arcball(width, height);

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
          arcball.initRotation(event.button.x, event.button.y);
          redraw = true;
          break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
          arcball.endRotation();
          redraw = true;
          break;
        case SDL_EVENT_MOUSE_MOTION:
          if (event.motion.state & SDL_BUTTON_LMASK)
          {
            arcball.computeRotation(event.motion.x, event.motion.y);
            redraw = true;
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

      for (std::vector<linalg::Vector<double, 3>> face : faces3d)
      {
        for (long unsigned int i = 0; i < face.size(); i++)
        {
          linalg::Vector<double, 3> l1 = face[i];
          linalg::Vector<double, 3> l2 = face[(i + 1) % face.size()];

          l1 = arcball.rotate(l1);
          l2 = arcball.rotate(l2);

          l1 = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l1;
          l2 = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l2;

          SDL_RenderLine(renderer, l1[0] + origin2D_x, l1[1] + origin2D_y,
                         l2[0] + origin2D_x, l2[1] + origin2D_y);
        }
      }
      for (int i = 0; i < 3; i++)
      {
        std::vector<linalg::Vector<double, 3>> axe = axes[i];
        std::vector<int> color                     = colors[i];
        linalg::Vector<double, 3> l1               = axe[0];
        linalg::Vector<double, 3> l2               = axe[1];

        l1 = arcball.rotate(l1);
        l2 = arcball.rotate(l2);

        l1 = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l1;
        l2 = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l2;

        SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 0xFF);
        SDL_RenderLine(renderer, l1[0] + 10, l1[1] + 10, l2[0] + 10,
                       l2[1] + 10);
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
