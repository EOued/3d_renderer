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

  while (1)
  {
    SDL_PollEvent(&event);
    if (event.type == SDL_EVENT_QUIT) break;

    // Faces surfaces
    std::vector<linalg::Vector<int, 3>> front = {
        {  0,   0, 0},
        {  0, 100, 0},
        {100, 100, 0},
        {100,   0, 0},
    };

    std::vector<linalg::Vector<int, 3>> back = {
        {  0,   0, 100},
        {  0, 100, 100},
        {100, 100, 100},
        {100,   0, 100},
    };

    std::vector<linalg::Vector<int, 3>> left = {
        {0,   0,   0},
        {0,   0, 100},
        {0, 100, 100},
        {0, 100,   0},
    };

    std::vector<linalg::Vector<int, 3>> right = {
        {100,   0,   0},
        {100,   0, 100},
        {100, 100, 100},
        {100, 100,   0},
    };

    // 3D to 2D

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);

    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

    // float cos = std::cos(M_PI);
    // float sin = std::sin(M_PI);

    auto faces3d = {front, back, left, right};
    for (std::vector<linalg::Vector<int, 3>> face : faces3d)
    {
      for (long unsigned int i = 0; i < face.size(); i++)
      {
        linalg::Vector<int, 3> l1 = face[i];
        linalg::Vector<int, 3> l2 = face[(i + 1) % face.size()];

        linalg::Vector<int, 3> _l1 =
            l1 * linalg::Matrix<float, 3, 3>::IsometricProjection();

        linalg::Vector<int, 3> _l2 =
            l2 * linalg::Matrix<float, 3, 3>::IsometricProjection();

        SDL_RenderLine(renderer, _l1.getValue(0), _l1.getValue(1),
                       _l2.getValue(0), _l2.getValue(1));
      }
    }
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
