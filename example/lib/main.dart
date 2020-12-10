import 'package:flutter/material.dart';
import 'package:window_background/window_background.dart';

const backgroundColor = Colors.blue;

void main() {
  WindowBackground.setColor(backgroundColor);
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Transform.scale(
        scale: 0.5,
        child: MaterialApp(
          home: Scaffold(
            appBar: AppBar(
              title: const Text('Window Background Example'),
            ),
            body: Center(
              child: Text('#${backgroundColor.value.toRadixString(16)}'),
            ),
          ),
        ),
      ),
    );
  }
}
