/// TODO
library window_background;

import 'dart:ui';

import 'src/dylib.dart';

/// TODO
abstract class WindowBackground {
  /// TODO
  static void setColor(Color color) {
    window_background_set_color(color.value);
  }
}
