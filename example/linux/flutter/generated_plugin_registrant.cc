//
//  Generated file. Do not edit.
//

#include "generated_plugin_registrant.h"

#include <window_background/window_background_plugin.h>

void fl_register_plugins(FlPluginRegistry* registry) {
  g_autoptr(FlPluginRegistrar) window_background_registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry, "WindowBackgroundPlugin");
  window_background_plugin_register_with_registrar(window_background_registrar);
}
