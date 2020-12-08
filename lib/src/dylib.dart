import 'dart:ffi' as ffi;
import 'dart:io';

typedef _d_window_background_set_color = void Function(int color);
typedef _c_window_background_set_color = ffi.Void Function(ffi.Uint32 color);

ffi.DynamicLibrary _dylib;

// ignore: non_constant_identifier_names
_d_window_background_set_color _window_background_set_color;

// ignore: non_constant_identifier_names
void window_background_set_color(int rgba) {
  _dylib ??= _LibraryLoader.load();
  _window_background_set_color ??= _dylib.lookupFunction<
      _c_window_background_set_color,
      _d_window_background_set_color>('window_background_set_color');
  _window_background_set_color(rgba);
}

extension _StringWith on String {
  String prefixWith(String prefix) {
    if (isEmpty || startsWith(prefix)) return this;
    return prefix + this;
  }

  String suffixWith(String suffix) {
    if (isEmpty || endsWith(suffix)) return this;
    return this + suffix;
  }
}

class _LibraryLoader {
  static String get platformPrefix => Platform.isWindows ? '' : 'lib';

  static String get platformSuffix {
    return Platform.isWindows
        ? '.dll'
        : Platform.isMacOS || Platform.isIOS
            ? '.dylib'
            : '.so';
  }

  static String fixupName(String baseName) {
    return baseName.prefixWith(platformPrefix).suffixWith(platformSuffix);
  }

  static String fixupPath(String path) => path.suffixWith('/');

  static bool isFile(String path) {
    return path.isNotEmpty &&
        Directory(path).statSync().type == FileSystemEntityType.file;
  }

  static String resolvePath() {
    final path = String.fromEnvironment(
      'WINDOW_BACKGROUND_PLUGIN_PATH',
      defaultValue: Platform.environment['WINDOW_BACKGROUND_PLUGIN_PATH'] ?? '',
    );
    if (isFile(path)) return path;
    return fixupPath(path) + fixupName('window_background_plugin');
  }

  static ffi.DynamicLibrary load() => ffi.DynamicLibrary.open(resolvePath());
}
