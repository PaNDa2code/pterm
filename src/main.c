#include <windows.h>
#include <stdio.h>
#include <dwmapi.h>

// Global variables
HBITMAP hGlyphAtlas = NULL;
int glyphWidth = 32;
int glyphHeight = 32;
int atlasColumns = 16;
int atlasRows = 8;

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnPaint(HWND hwnd);
void CreateGlyphAtlas(HDC hdc, HFONT hFont);
void RenderText(HDC hdc, const wchar_t *text, int x, int y);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  const char CLASS_NAME[] = "Glyph Atlas Example";

  WNDCLASS wc = {};
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  HWND hwnd = CreateWindowEx(0,
                             CLASS_NAME,
                             "Glyph Atlas with GDI",
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             800,
                             600,
                             NULL,
                             NULL,
                             hInstance,
                             NULL);

  if (hwnd == NULL)
  {
    return 0;
  }

  BOOL darkmode = TRUE;
  DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkmode, sizeof(darkmode));

  ShowWindow(hwnd, nShowCmd);

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_PAINT:
    OnPaint(hwnd);
    return 0;
  case WM_DESTROY:
    if (hGlyphAtlas)
      DeleteObject(hGlyphAtlas);
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CreateGlyphAtlas(HDC hdc, HFONT hFont)
{
  HDC hdcMem = CreateCompatibleDC(hdc);
  hGlyphAtlas = CreateCompatibleBitmap(hdc, glyphWidth * atlasColumns, glyphHeight * atlasRows);
  SelectObject(hdcMem, hGlyphAtlas);

  HBRUSH hBrush = CreateSolidBrush(RGB(32, 32, 32));
  RECT rect = {0, 0, glyphWidth * atlasColumns, glyphHeight * atlasRows};
  FillRect(hdcMem, &rect, hBrush);
  DeleteObject(hBrush);

  SelectObject(hdcMem, hFont);
  SetBkMode(hdcMem, TRANSPARENT);
  SetTextColor(hdcMem, RGB(255, 255, 255));

  for (int row = 0; row < atlasRows; row++)
  {
    for (int col = 0; col < atlasColumns; col++)
    {
      int glyphIndex = row * atlasColumns + col;
      if (glyphIndex >= 256)
        break;
      int x = col * glyphWidth;
      int y = row * glyphHeight;
      wchar_t glyph = (wchar_t)glyphIndex;
      RECT glyphRect = {x, y, x + glyphWidth, y + glyphHeight};
      DrawTextW(hdcMem, &glyph, 1, &glyphRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
  }
  DeleteDC(hdcMem);
}

void RenderText(HDC hdc, const wchar_t *text, int x, int y)
{
  if (!hGlyphAtlas)
    return;

  HDC hdcMem = CreateCompatibleDC(hdc);
  SelectObject(hdcMem, hGlyphAtlas);

  for (int i = 0; text[i] != L'\0'; i++)
  {
    wchar_t glyph = text[i];
    int glyphIndex = (int)glyph;
    if (glyphIndex < 0 || glyphIndex >= 256)
      glyphIndex = '?';

    int atlasX = (glyphIndex % atlasColumns) * glyphWidth;
    int atlasY = (glyphIndex / atlasColumns) * glyphHeight;
    BitBlt(hdc, x + i * glyphWidth, y, glyphWidth, glyphHeight, hdcMem, atlasX, atlasY, SRCCOPY);
  }

  DeleteDC(hdcMem);
}

void OnPaint(HWND hwnd)
{
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hwnd, &ps);

  HBRUSH hBrush = CreateSolidBrush(RGB(32, 32, 32));
  FillRect(hdc, &ps.rcPaint, hBrush);
  DeleteObject(hBrush);

  HFONT hFont = CreateFontW(0,
                            0,
                            0,
                            0,
                            FW_NORMAL,
                            FALSE,
                            FALSE,
                            FALSE,
                            DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY,
                            DEFAULT_PITCH | FF_DONTCARE,
                            L"Arial");

  CreateGlyphAtlas(hdc, hFont);
  RenderText(hdc, L"Hello, Glyph Atlas!", 10, 10);

  DeleteObject(hFont);
  EndPaint(hwnd, &ps);
}
