#include "include/window_background/window_background_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>

#define WINDOW_BACKGROUND_PLUGIN(obj)                                     \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), window_background_plugin_get_type(), \
                              WindowBackgroundPlugin))

struct _WindowBackgroundPlugin {
  GObject parent_instance;
  FlView* view = nullptr;
  bool has_color = false;
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  uint8_t alpha = 0;
};

G_DEFINE_TYPE(WindowBackgroundPlugin, window_background_plugin,
              g_object_get_type())

static gboolean window_background_plugin_draw(GtkWidget* widget, cairo_t* cr,
                                              gpointer data) {
  WindowBackgroundPlugin* plugin = WINDOW_BACKGROUND_PLUGIN(data);
  if (!plugin->has_color) {
    return FALSE;
  }

  float red = plugin->red / 255.0;
  float green = plugin->green / 255.0;
  float blue = plugin->blue / 255.0;
  float alpha = plugin->alpha / 255.0;
  cairo_set_source_rgba(cr, red, green, blue, alpha);

  GtkAllocation allocation;
  gtk_widget_get_allocation(widget, &allocation);
  cairo_rectangle(cr, 0, 0, allocation.width, allocation.height);
  cairo_fill(cr);

  return FALSE;
}

static void window_background_plugin_redraw(WindowBackgroundPlugin* self) {
  g_return_if_fail(self->view != nullptr);
  if (gtk_widget_get_realized(GTK_WIDGET(self->view))) {
    gtk_widget_queue_draw(GTK_WIDGET(self->view));
  }
}

static void window_background_plugin_set_color(WindowBackgroundPlugin* self,
                                               uint8_t red, uint8_t green,
                                               uint8_t blue, uint8_t alpha) {
  self->has_color = true;
  self->red = red;
  self->green = green;
  self->blue = blue;
  self->alpha = alpha;
  window_background_plugin_redraw(self);
}

static int fl_value_get_list_int_value(FlValue* list, int index) {
  return fl_value_get_int(fl_value_get_list_value(list, index));
}

static void window_background_plugin_handle_method_call(
    WindowBackgroundPlugin* self, FlMethodCall* method_call) {
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar* method = fl_method_call_get_name(method_call);

  if (strcmp(method, "setColor") == 0) {
    FlValue* args = fl_method_call_get_args(method_call);
    uint8_t red = fl_value_get_list_int_value(args, 0);
    uint8_t green = fl_value_get_list_int_value(args, 1);
    uint8_t blue = fl_value_get_list_int_value(args, 2);
    uint8_t alpha = fl_value_get_list_int_value(args, 3);
    window_background_plugin_set_color(self, red, green, blue, alpha);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(nullptr));
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
}

static void window_background_plugin_cleanup(WindowBackgroundPlugin* self) {
  self->view = nullptr;
}

static void weak_notify_cb(gpointer user_data, GObject* where_the_object_was) {
  WindowBackgroundPlugin* plugin = WINDOW_BACKGROUND_PLUGIN(user_data);
  window_background_plugin_cleanup(plugin);
}

static void window_background_plugin_dispose(GObject* object) {
  WindowBackgroundPlugin* self = WINDOW_BACKGROUND_PLUGIN(object);
  if (self->view != nullptr) {
    g_object_weak_unref(G_OBJECT(self->view), weak_notify_cb, self);
    self->view = nullptr;
  }

  G_OBJECT_CLASS(window_background_plugin_parent_class)->dispose(object);
}

static void window_background_plugin_class_init(
    WindowBackgroundPluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = window_background_plugin_dispose;
}

static void window_background_plugin_init(WindowBackgroundPlugin* self) {}

static void method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  WindowBackgroundPlugin* plugin = WINDOW_BACKGROUND_PLUGIN(user_data);
  window_background_plugin_handle_method_call(plugin, method_call);
}

void window_background_plugin_register_with_registrar(
    FlPluginRegistrar* registrar) {
  WindowBackgroundPlugin* plugin = WINDOW_BACKGROUND_PLUGIN(
      g_object_new(window_background_plugin_get_type(), nullptr));

  plugin->view = fl_plugin_registrar_get_view(registrar);
  g_object_weak_ref(G_OBJECT(plugin->view), weak_notify_cb, plugin);
  g_signal_connect(plugin->view, "draw",
                   G_CALLBACK(window_background_plugin_draw), plugin);

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "window_background", FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(
      channel, method_call_cb, g_object_ref(plugin), g_object_unref);

  g_object_unref(plugin);
}
