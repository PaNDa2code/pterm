#include <stdbool.h>
#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_main.h>

// Enable dark mode for window
void EnableDarkMode(HWND hwnd);

#define WEDTH 800
#define HIGHT 600
int main(int argc, char **argv)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    return -1;

  SDL_Window *window = SDL_CreateWindow("Pterminal",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WEDTH,
                                        HIGHT,
                                        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(window, &wmInfo);
  HWND hwnd = wmInfo.info.win.window;

  EnableDarkMode(hwnd);

  SDL_ShowWindow(window);

  bool quit = false;

  while (!quit)
  {
    SDL_Event e;
    SDL_WaitEvent(&e);
    if (e.type == SDL_QUIT)
      quit = true;

    SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }

  return 0;
}
