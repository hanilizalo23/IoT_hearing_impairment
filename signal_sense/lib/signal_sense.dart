import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

class SignalSense extends StatelessWidget {
  final MqttServerClient client;

  const SignalSense({super.key, required this.client});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Signal Sense')),
      body: StreamBuilder<List<MqttReceivedMessage<MqttMessage>>>(
        stream: client.updates, // Receive MQTT updates
        builder: (context, snapshot) {
          if (!snapshot.hasData) {
            return const CircularProgressIndicator();
          }
          final updates = snapshot.data!;
          final latestUpdate = updates.last.payload.toString();
          return Center(child: Text('AAAAAAAAAAa: $latestUpdate'));
        },
      ),
    );
  }
}
