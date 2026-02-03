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

    std::vector<std::tuple<int, int, int>> front = {
        {  0,   0, 0},
        {  0, 100, 0},
        {100, 100, 0},
        {100,   0, 0},
    };

    std::vector<std::tuple<int, int, int>> back = {
        {  0,   0, 100},
        {  0, 100, 100},
        {100, 100, 100},
        {100,   0, 100},
    };

    std::vector<std::tuple<int, int, int>> left = {
        {0,   0,   0},
        {0,   0, 100},
        {0, 100, 100},
        {0, 100,   0},
    };

    std::vector<std::tuple<int, int, int>> right = {
        {100,   0,   0},
        {100,   0, 100},
        {100, 100, 100},
        {100, 100,   0},
    };

    // 3D to 2D

    for (float angle = 0;; angle++)
    {
      SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
      SDL_RenderClear(renderer);
      SDL_RenderTexture(renderer, texture, NULL, NULL);

      SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

      float cos = std::cos(angle);
      float sin = std::sin(angle);

      std::cout << cos << std::endl;
      std::cout << sin << std::endl;

      auto faces3d = {front, back, left, right};
      for (std::vector<std::tuple<int, int, int>> face : faces3d)
      {
        for (long unsigned int i = 0; i < face.size(); i++)
        {
          std::tuple<int, int, int> l1 = face[i];
          std::tuple<int, int, int> l2 = face[(i + 1) % face.size()];

          // Apply translation
          l1 = {
              std::get<0>(l1) * cos + std::get<1>(l1) * (-sin),
              std::get<0>(l1) * (sin) + std::get<1>(l1) * cos,
              std::get<2>(l1),
          };

          l2 = {
              std::get<0>(l2) * cos + std::get<1>(l2) * (-sin),
              std::get<0>(l2) * (sin) + std::get<1>(l2) * cos,
              std::get<2>(l2),
          };

          l1 = {
              std::get<0>(l1) + 100,
              std::get<1>(l1) + 100,
              std::get<2>(l1) + 100,
          };

          l2 = {
              std::get<0>(l2) + 100,
              std::get<1>(l2) + 100,
              std::get<2>(l2) + 100,
          };

          // Isometric 3d
          l1 = {
              1 / std::sqrt(6) *
                  (std::sqrt(3) * std::get<0>(l1) - std::get<1>(l1) +
                   std::sqrt(2) * std::get<2>(l1)),
              1 / std::sqrt(6) *
                  (2 * std::get<1>(l1) + std::sqrt(2) * std::get<2>(l1)),
              1 / std::sqrt(6) *
                  (std::sqrt(3) * std::get<0>(l1) - std::get<1>(l1) +
                   std::sqrt(2) * std::get<2>(l1)),
          };

          l2 = {
              1 / std::sqrt(6) *
                  (std::sqrt(3) * std::get<0>(l2) - std::get<1>(l2) +
                   std::sqrt(2) * std::get<2>(l2)),
              1 / std::sqrt(6) *
                  (2 * std::get<1>(l2) + std::sqrt(2) * std::get<2>(l2)),
              1 / std::sqrt(6) *
                  (std::sqrt(3) * std::get<0>(l2) - std::get<1>(l2) +
                   std::sqrt(2) * std::get<2>(l2)),
          };

          SDL_RenderLine(renderer, std::get<0>(l1), std::get<1>(l1),
                         std::get<0>(l2), std::get<1>(l2));
        }
      }
      SDL_RenderPresent(renderer);
      angle += 0.00000001;
      angle = std::fmod(angle, 2 * M_PI);
    }
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
