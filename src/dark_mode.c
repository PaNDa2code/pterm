#include <windows.h>
#include <dwmapi.h>

void EnableDarkMode(HWND hwnd)
{
  BOOL darkMode = TRUE;
  DwmSetWindowAttribute(hwnd,
                        DWMWA_USE_IMMERSIVE_DARK_MODE,
                        &darkMode,
                        sizeof(darkMode));

  // Set dark colors for window borders
  COLORREF darkColor = RGB(32, 32, 32);
  DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &darkColor, sizeof(darkColor));
  DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &darkColor, sizeof(darkColor));
}
