#include "include/window_background/window_background_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>

namespace {

class WindowBackgroundPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  WindowBackgroundPlugin();

  virtual ~WindowBackgroundPlugin();

 private:
  LRESULT DrawWindowBackground(HWND hwnd, HDC hdc);
  void SetWindowBackground(COLORREF color);

  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  HWND _hwnd = nullptr;
  HBRUSH _bg = nullptr;
};

// static
void WindowBackgroundPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "window_background",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<WindowBackgroundPlugin>();

  registrar->RegisterTopLevelWindowProcDelegate(
    [p = plugin.get()](HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
      if (message == WM_ERASEBKGND) {
        auto hr = p->DrawWindowBackground(hwnd, reinterpret_cast<HDC>(wparam));
        return std::make_optional<LRESULT>(hr);
      }
      return std::optional<LRESULT>(std::nullopt);
    });

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

WindowBackgroundPlugin::WindowBackgroundPlugin() {}

WindowBackgroundPlugin::~WindowBackgroundPlugin() {}

LRESULT WindowBackgroundPlugin::DrawWindowBackground(HWND hwnd, HDC hdc) {
  _hwnd = hwnd;
  if (!_bg) {
    return 0L;
  }
  RECT rect;
  GetClientRect(hwnd, &rect);
  FillRect(hdc, &rect, _bg);
  return 1L;
}

void WindowBackgroundPlugin::SetWindowBackground(COLORREF color) {
  _bg = CreateSolidBrush(color);
  if (_hwnd) {
    RedrawWindow(_hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
  }
}

static inline uint8_t c(int64_t argb, int b) { return ((argb >> b) & 0xff); }
static inline uint8_t r(int64_t argb) { return c(argb, 16); }
static inline uint8_t g(int64_t argb) { return c(argb, 8); }
static inline uint8_t b(int64_t argb) { return c(argb, 0); }
static inline uint8_t a(int64_t argb) { return c(argb, 24); }
static inline uint32_t abgr(int64_t argb) { return RGB(r(argb), g(argb), b(argb)); }

void WindowBackgroundPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("setColor") == 0) {
    const flutter::EncodableList args = std::get<flutter::EncodableList>(*method_call.arguments());
    int64_t argb = std::get<int64_t>(args.front());
    COLORREF color = abgr(argb);
    SetWindowBackground(color);
    result->Success();
  } else {
    result->NotImplemented();
  }
}

}  // namespace

void WindowBackgroundPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  WindowBackgroundPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
