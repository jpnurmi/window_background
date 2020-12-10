import Cocoa
import FlutterMacOS

public class WindowBackgroundPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "window_background", binaryMessenger: registrar.messenger)
    let instance = WindowBackgroundPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "setColor":
      let args = call.arguments as! [Int]
      let view = NSApplication.shared.mainWindow?.contentView
      view?.layer?.backgroundColor = rgba(args[0], args[1], args[2], args[3])
      result(true)
    default:
      result(FlutterMethodNotImplemented)
    }
  }
}

private func rgba(_ red: Int, _ green: Int, _ blue: Int, _ alpha: Int) -> CGColor {
  let r = CGFloat(red) / 255.0
  let g = CGFloat(green) / 255.0
  let b = CGFloat(blue) / 255.0
  let a = CGFloat(alpha) / 255.0
  let color = NSColor(red: r, green: g, blue: b, alpha: a)
  return color.cgColor
}
