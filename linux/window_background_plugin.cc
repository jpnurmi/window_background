#include "include/window_background/window_background_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <gmodule.h>

#define WINDOW_BACKGROUND_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), window_background_plugin_get_type(), \
                              WindowBackgroundPlugin))

struct _WindowBackgroundPlugin {
  GObject parent_instance;
};

G_DEFINE_TYPE(WindowBackgroundPlugin, window_background_plugin, g_object_get_type())

static FlView* fl_view = nullptr;

static void view_weak_notify_cb(gpointer user_data,
                                GObject* where_the_object_was) {
  fl_view = nullptr;
}

static void window_background_plugin_dispose(GObject* object) {
  G_OBJECT_CLASS(window_background_plugin_parent_class)->dispose(object);

  if (fl_view != nullptr) {
    g_object_weak_unref(G_OBJECT(fl_view), view_weak_notify_cb, nullptr);
    fl_view = nullptr;
  }
}

static void window_background_plugin_class_init(WindowBackgroundPluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = window_background_plugin_dispose;
}

static void window_background_plugin_init(WindowBackgroundPlugin* self) {}

static void window_background_redraw() {
  g_return_if_fail(fl_view != nullptr);
  if (gtk_widget_get_realized(GTK_WIDGET(fl_view))) {
    gtk_widget_queue_draw(GTK_WIDGET(fl_view));
  }
}

static unsigned int bg = 0;

extern "C" {
  G_MODULE_EXPORT void window_background_set_color(unsigned int argb) {
    if (bg != argb) {
      bg = argb;
      window_background_redraw();
    }
  }
}

static inline float c(int argb, int b) { return ((argb >> b) & 0xff) / 255.0; }
static inline float r(unsigned int argb) { return c(argb, 16); }
static inline float g(unsigned int argb) { return c(argb, 8); }
static inline float b(unsigned int argb) { return c(argb, 0); }
static inline float a(unsigned int argb) { return c(argb, 24); }

static gboolean window_background_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  if (bg != 0) {
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);
    cairo_set_source_rgba(cr, r(bg), g(bg), b(bg), a(bg));
    cairo_rectangle(cr, 0, 0, allocation.width, allocation.height);
    cairo_fill(cr);
  }
  return FALSE;
}

static gchar* g_self_exe() {
  g_autoptr(GError) error = nullptr;
  g_autofree gchar* self_exe = g_file_read_link("/proc/self/exe", &error);
  if (error) {
    g_critical("g_file_read_link: %s", error->message);
  }
  return g_path_get_dirname(self_exe);
}

void window_background_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
  WindowBackgroundPlugin* plugin = WINDOW_BACKGROUND_PLUGIN(
      g_object_new(window_background_plugin_get_type(), nullptr));

  fl_view = fl_plugin_registrar_get_view(registrar);
  g_object_weak_ref(G_OBJECT(fl_view), view_weak_notify_cb, nullptr);
  g_signal_connect(fl_view, "draw", G_CALLBACK(window_background_draw), nullptr);

  g_autofree gchar* self_exe = g_self_exe();
  g_autofree gchar* plugin_path =
      g_build_filename(self_exe, "lib", "libwindow_background_plugin.so", nullptr);
  setenv("WINDOW_BACKGROUND_PLUGIN_PATH", plugin_path, 0);
}
