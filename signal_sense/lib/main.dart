import 'dart:io';

import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

import 'signal_sense.dart';

void main() {
  final client = MqttServerClient(
      'http://a3fqgfoikj274h-ats.iot.us-east-2.amazonaws.com',
      ''); // Replace with your broker's address
  try {
    client.connect();
  } on NoConnectionException catch (e) {
    // Raised by the client when connection fails.
    print('EXAMPLE::client exception - $e');
    client.disconnect();
  } on SocketException catch (e) {
    // Raised by the socket layer
    print('EXAMPLE::socket exception - $e');
    client.disconnect();
  } // Replace with a unique client ID
  runApp(MyApp(client: client));
}

class MyApp extends StatelessWidget {
  final MqttServerClient client;

  const MyApp({super.key, required this.client});
  @override
  Widget build(BuildContext context) {
    client.subscribe(
        'sensor', MqttQos.atMostOnce); // Subscribe to the weather updates topic
    return MaterialApp(
      title: 'Signal Sense',
      home: SignalSense(client: client),
    );
  }
}
