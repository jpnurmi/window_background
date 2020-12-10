/// TODO
library window_background;

import 'dart:ui';

import 'package:flutter/services.dart';
import 'package:flutter/widgets.dart';

/// TODO
class WindowBackground {
  static const MethodChannel _channel =
      const MethodChannel('window_background');

  /// TODO
  static void setColor(Color color) {
    WidgetsFlutterBinding.ensureInitialized();
    var args = [color.red, color.green, color.blue, color.alpha];
    _channel.invokeMethod('setColor', args);
  }
}
