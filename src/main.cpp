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

  // Faces surfaces
  std::vector<linalg::Vector<int, 3>> front = {
      { 1,  1, -1}, // Bottom - Right
      { 1, -1, -1}, // Top - Right
      {-1, -1, -1}, // Top - Left
      {-1,  1, -1}  // Bottom - Left
  };

  std::vector<linalg::Vector<int, 3>> back = {
      { 1,  1, 1}, // Bottom - Right
      { 1, -1, 1}, // Top - Right
      {-1, -1, 1}, // Top - Left
      {-1,  1, 1}  // Bottom - Left
  };

  std::vector<linalg::Vector<int, 3>> left = {
      {-1,  1,  1}, // Bottom - Right
      {-1, -1,  1}, // Top - Right
      {-1, -1, -1}, // Top - Left
      {-1,  1, -1}  // Bottom - Left
  };

  std::vector<linalg::Vector<int, 3>> right = {
      {1,  1,  1}, // Bottom - Right
      {1, -1,  1}, // Top - Right
      {1, -1, -1}, // Top - Left
      {1,  1, -1}  // Bottom - Left
  };

  std::vector<linalg::Vector<int, 3>> top = {
      {-1, -1, -1},
      { 1, -1, -1},
      { 1, -1,  1},
      {-1, -1,  1}
  };

  std::vector<linalg::Vector<int, 3>> bottom = {
      {-1, 1, -1},
      { 1, 1, -1},
      { 1, 1,  1},
      {-1, 1,  1}
  };

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

  while (1)
  {
    SDL_PollEvent(&event);
    if (event.type == SDL_EVENT_QUIT) break;

    // 3D to 2D
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);

    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

    auto faces3d = {front, back, left, right};
    auto axes    = {x_axis, y_axis, z_axis};

    for (std::vector<linalg::Vector<int, 3>> face : faces3d)
    {
      for (long unsigned int i = 0; i < face.size(); i++)
      {
        linalg::Vector<int, 3> l1 = face[i];
        linalg::Vector<int, 3> l2 = face[(i + 1) % face.size()];

        l1 *= linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10});
        l2 *= linalg::Matrix<int, 3, 3>::ScalingMatrix({10, 10, 10});

        linalg::Matrix<int, 3, 1> _l1 = l1;
        linalg::Matrix<int, 3, 1> _l2 = l2;

        // To 2D :
        SDL_RenderLine(renderer, _l1[0] + origin2D_x, _l1[1] + origin2D_y,
                       _l2[0] + origin2D_x, _l2[1] + origin2D_y);
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
