#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <windows.h>
#include <stdbool.h>

// Enable dark mode for window
void EnableDarkMode(HWND hwnd);

#define TERMINAL_WIDTH 60
#define TERMINAL_HEIGHT 30
#define FONT_HEIGHT 24
#define FONT_WEDTH 20
#define FONT_SIZE FONT_HEIGHT
#define DARK_MODE true
#define TRANSPARANT false

typedef struct
{
  unsigned char character;
  SDL_Color fg_color;
  SDL_Color bg_color;
} TerminalCell;

TTF_Font *font = NULL;
SDL_Texture *glyph_atlas[128];
TerminalCell terminal[TERMINAL_HEIGHT][TERMINAL_WIDTH];
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
int glyph_width = 0;

void InitializeTerminal()
{
  for (int y = 0; y < TERMINAL_HEIGHT; y++)
  {
    for (int x = 0; x < TERMINAL_WIDTH; x++)
    {
      terminal[y][x].character = ' ';
      terminal[y][x].fg_color = (SDL_Color){255, 255, 255, 255};
      terminal[y][x].bg_color = (SDL_Color){0, 0, 0, 255};
    }
  }
}

bool CreateGlyphAtlas()
{
  if (!TTF_FontIsFixedWidth(font))
    return false;

  SDL_Color white = {255, 255, 255, 255};
  for (unsigned char i = 0; i < 128; i++)
  {
    SDL_Surface *surface = TTF_RenderText_Solid(font, (const char *)&i, 1, white);
    if (surface == NULL)
    {
      SDL_Log("Faild to render gliph to index: %d", i);
      return false;
    }
    glyph_atlas[i] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (glyph_atlas[i] == NULL)
    {
      SDL_Log("Faild to create atlas to index: %d", i);
      return false;
    }
  }
  return true;
}

void RenderTerminal()
{
  SDL_RenderClear(renderer);

  for (int y = 0; y < TERMINAL_HEIGHT; y++)
  {
    for (int x = 0; x < TERMINAL_WIDTH; x++)
    {
      TerminalCell cell = terminal[y][x];

      SDL_FRect bgRect = {x * FONT_SIZE, y * FONT_SIZE, FONT_SIZE, FONT_SIZE};
      SDL_SetRenderDrawColor(renderer, cell.bg_color.r, cell.bg_color.g, cell.bg_color.b, cell.bg_color.a);
      SDL_RenderFillRect(renderer, &bgRect);

      if (cell.character >= 0 && cell.character < 128 && glyph_atlas[(int)cell.character])
      {
        SDL_FRect dstRect = {x * FONT_SIZE, y * FONT_SIZE, FONT_WEDTH, FONT_HEIGHT};
        SDL_RenderTexture(renderer, glyph_atlas[(int)cell.character], NULL, &dstRect);
      }
    }
  }

  SDL_RenderPresent(renderer);
}

int main(int argc, char **argv)
{
  HWND hwnd;
  SDL_PropertiesID windowPropId = 0;
  bool quit = false;

  if (!SDL_Init(SDL_INIT_VIDEO))
    return -1;

  if (!TTF_Init())
    return -1;

  if (!SDL_CreateWindowAndRenderer(TEXT("Pterminal"),
                                   TERMINAL_WIDTH * FONT_WEDTH,
                                   TERMINAL_HEIGHT * FONT_HEIGHT,
                                   SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE,
                                   &window,
                                   &renderer))
  {
    SDL_Log("Error SDL_CreateWindowAndRenderer");
    return -1;
  };

  windowPropId = SDL_GetWindowProperties(window);
  if (windowPropId == 0)
  {
    SDL_Log("Error SDL_GetWindowProperties");
    return -1;
  }
  hwnd = SDL_GetPointerProperty(windowPropId, SDL_PROP_WINDOW_WIN32_HWND_POINTER, INVALID_HANDLE_VALUE);

  if (hwnd == INVALID_HANDLE_VALUE)
  {
    SDL_Log("Error SDL_GetPointerProperty");
    return -1;
  }

  if (DARK_MODE)
    EnableDarkMode(hwnd);
  if (TRANSPARANT)
  {
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 200, LWA_ALPHA);
    UpdateWindow(hwnd);
  }

  SDL_ShowWindow(window);

  font = TTF_OpenFont("C:/Windows/Fonts/CascadiaCode.ttf", FONT_SIZE);
  InitializeTerminal();

  if (!CreateGlyphAtlas())
  {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Event event;

  while (!quit)
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
        quit = true;
    }
    terminal[0][0].character = 'H';
    terminal[0][1].character = 'i';
    terminal[0][2].character = '!';
    SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);

    SDL_RenderClear(renderer);
    RenderTerminal();

    SDL_RenderPresent(renderer);
  }
  for (int i = 0; i < 128; i++)
  {
    if (glyph_atlas[i])
    {
      SDL_DestroyTexture(glyph_atlas[i]);
    }
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
