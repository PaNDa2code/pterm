#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>

#include "create_process.h"
#include "ring_buffer.h"

// Enable dark mode for window
void EnableDarkMode(HWND hwnd);

#define TERMINAL_WIDTH 60
#define TERMINAL_HEIGHT 30
#define FONT_SIZE 24
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
RING_BUFFER stdoutRingBuffer = {};
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
int glyph_width = 0;

void InitializeTerminal()
{
  CreateRingBuffer(&stdoutRingBuffer, 64 KB);
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

  size_t ReadOffset = stdoutRingBuffer.ReadOffset;
  size_t WriteOffset = stdoutRingBuffer.WriteOffset;
  size_t BufferSize = stdoutRingBuffer.BufferSize;

  size_t Length = (WriteOffset >= ReadOffset) ? (WriteOffset - ReadOffset) : BufferSize;

  unsigned char *BufferStart = stdoutRingBuffer.BaseBuffer + ReadOffset;

  for (size_t i = 0; i < Length; i++)
  {
    size_t x = i % TERMINAL_WIDTH;
    size_t y = i / TERMINAL_WIDTH;

    SDL_Texture *glyph = glyph_atlas[BufferStart[i]];
    SDL_FRect disRect = {x * FONT_SIZE, y * FONT_SIZE, FONT_SIZE, FONT_SIZE};
    SDL_RenderTexture(renderer, glyph, NULL, &disRect);
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

  if (!SDL_CreateWindowAndRenderer(TEXT("pterminal"),
                                   TERMINAL_WIDTH * FONT_SIZE,
                                   TERMINAL_HEIGHT * FONT_SIZE,
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

  HANDLE hRead, hWrite, hProcess;
  HPCON hPC;
  SpawnChildProcess(L"C:\\windows\\system32\\cmd.exe", &hProcess, &hWrite, &hRead, &hPC);
  CreateRingBuffer(&stdoutRingBuffer, 64 KB);

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
    SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
    SDL_RenderClear(renderer);

    RingBufferHandleRead(&stdoutRingBuffer, hRead);
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
  FreeRingBuffer(&stdoutRingBuffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
