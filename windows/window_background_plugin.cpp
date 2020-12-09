#include "include/window_background/window_background_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace {

static inline HWND _hwnd = nullptr;
static inline HBRUSH _bg = nullptr;

LRESULT DrawWindowBackground(HWND hwnd, HDC hdc) {
  _hwnd = hwnd;
  if (!_bg) {
    return 0L;
  }
  RECT rect;
  GetClientRect(hwnd, &rect);
  FillRect(hdc, &rect, _bg);
  return 1L;
}

static void SetWindowBackground(COLORREF color) {
  _bg = CreateSolidBrush(color);
  if (_hwnd) {
    RedrawWindow(_hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
  }
}

class WindowBackgroundPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  WindowBackgroundPlugin();

  virtual ~WindowBackgroundPlugin();
};

// static
void WindowBackgroundPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto plugin = std::make_unique<WindowBackgroundPlugin>();

  registrar->RegisterTopLevelWindowProcDelegate(
    [](HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
      if (message == WM_ERASEBKGND) {
        auto hr = DrawWindowBackground(hwnd, reinterpret_cast<HDC>(wparam));
        return std::make_optional<LRESULT>(hr);
      }
      return std::optional<LRESULT>(std::nullopt);
    });

  registrar->AddPlugin(std::move(plugin));
}

WindowBackgroundPlugin::WindowBackgroundPlugin() {}

WindowBackgroundPlugin::~WindowBackgroundPlugin() {}

}  // namespace

void WindowBackgroundPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  WindowBackgroundPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

typedef unsigned int uint;
static inline uint c(uint argb, int b) { return ((argb >> b) & 0xff); }
static inline uint r(uint argb) { return c(argb, 16); }
static inline uint g(uint argb) { return c(argb, 8); }
static inline uint b(uint argb) { return c(argb, 0); }
static inline uint a(uint argb) { return c(argb, 24); }
static inline uint abgr(uint argb) { return RGB(r(argb), g(argb), b(argb)); }

extern "C" {
  __declspec(dllexport)
  void window_background_set_color(uint argb) {
    COLORREF color = abgr(argb);
    SetWindowBackground(color);
  }
}
