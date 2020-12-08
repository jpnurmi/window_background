import 'package:flutter/material.dart';
import 'package:window_background/window_background.dart';

void main() {
  WindowBackground.setColor(Colors.blueGrey);
  //runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Hello'),
        ),
        body: Center(
          child: const Text('World'),
        ),
      ),
    );
  }
}
