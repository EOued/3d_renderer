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

  const std::vector<linalg::Vector<double, 3>> cube_vertex = {
      {-1.0f, -1.0f, -1.0f}, // triangle 1 : begin
      {-1.0f, -1.0f,  1.0f},
      {-1.0f,  1.0f,  1.0f}, // triangle 1 : end
      { 1.0f,  1.0f, -1.0f}, // triangle 2 : begin
      {-1.0f, -1.0f, -1.0f},
      {-1.0f,  1.0f, -1.0f}, // triangle 2 : end
      { 1.0f, -1.0f,  1.0f},
      {-1.0f, -1.0f, -1.0f},
      { 1.0f, -1.0f, -1.0f},
      { 1.0f,  1.0f, -1.0f},
      { 1.0f, -1.0f, -1.0f},
      {-1.0f, -1.0f, -1.0f},
      {-1.0f, -1.0f, -1.0f},
      {-1.0f,  1.0f,  1.0f},
      {-1.0f,  1.0f, -1.0f},
      { 1.0f, -1.0f,  1.0f},
      {-1.0f, -1.0f,  1.0f},
      {-1.0f, -1.0f, -1.0f},
      {-1.0f,  1.0f,  1.0f},
      {-1.0f, -1.0f,  1.0f},
      { 1.0f, -1.0f,  1.0f},
      { 1.0f,  1.0f,  1.0f},
      { 1.0f, -1.0f, -1.0f},
      { 1.0f,  1.0f, -1.0f},
      { 1.0f, -1.0f, -1.0f},
      { 1.0f,  1.0f,  1.0f},
      { 1.0f, -1.0f,  1.0f},
      { 1.0f,  1.0f,  1.0f},
      { 1.0f,  1.0f, -1.0f},
      {-1.0f,  1.0f, -1.0f},
      { 1.0f,  1.0f,  1.0f},
      {-1.0f,  1.0f, -1.0f},
      {-1.0f,  1.0f,  1.0f},
      { 1.0f,  1.0f,  1.0f},
      {-1.0f,  1.0f,  1.0f},
      { 1.0f, -1.0f,  1.0f}
  };

  const std::vector<linalg::Vector<double, 3>> cube_colors = {
      {0.583f, 0.771f, 0.014f},
      {0.609f, 0.115f, 0.436f},
      {0.327f, 0.483f, 0.844f},
      {0.822f, 0.569f, 0.201f},
      {0.435f, 0.602f, 0.223f},
      {0.310f, 0.747f, 0.185f},
      {0.597f, 0.770f, 0.761f},
      {0.559f, 0.436f, 0.730f},
      {0.359f, 0.583f, 0.152f},
      {0.483f, 0.596f, 0.789f},
      {0.559f, 0.861f, 0.639f},
      {0.195f, 0.548f, 0.859f},
      {0.014f, 0.184f, 0.576f},
      {0.771f, 0.328f, 0.970f},
      {0.406f, 0.615f, 0.116f},
      {0.676f, 0.977f, 0.133f},
      {0.971f, 0.572f, 0.833f},
      {0.140f, 0.616f, 0.489f},
      {0.997f, 0.513f, 0.064f},
      {0.945f, 0.719f, 0.592f},
      {0.543f, 0.021f, 0.978f},
      {0.279f, 0.317f, 0.505f},
      {0.167f, 0.620f, 0.077f},
      {0.347f, 0.857f, 0.137f},
      {0.055f, 0.953f, 0.042f},
      {0.714f, 0.505f, 0.345f},
      {0.783f, 0.290f, 0.734f},
      {0.722f, 0.645f, 0.174f},
      {0.302f, 0.455f, 0.848f},
      {0.225f, 0.587f, 0.040f},
      {0.517f, 0.713f, 0.338f},
      {0.053f, 0.959f, 0.120f},
      {0.393f, 0.621f, 0.362f},
      {0.673f, 0.211f, 0.457f},
      {0.820f, 0.883f, 0.371f},
      {0.982f, 0.099f, 0.879f}
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
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
      for (size_t i = 0; i < cube_vertex.size(); i += 3)
      {
        SDL_Vertex vertex[3];
        for (int j = 0; j < 3; j++)
        {
          linalg::Vector<double, 3> l = cube_vertex[i + j];
          l = linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10}) * l;
          l = arcball.rotate(l);
          l += linalg::Vector<double, 3>{origin2D_x, origin2D_y, 100};
          vertex[j].position.x = l[0];
          vertex[j].position.y = l[1];

          vertex[j].color.r = cube_colors[i + j][0];
          vertex[j].color.g = cube_colors[i + j][1];
          vertex[j].color.b = cube_colors[i + j][2];
          vertex[j].color.a = 1.0;
        }

        SDL_RenderGeometry(renderer, NULL, vertex, 3, NULL, 0);
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
