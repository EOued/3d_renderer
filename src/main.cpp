#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>
#include <iostream>

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
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
