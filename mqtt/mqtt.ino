#include <WiFi.h>
#include <PubSubClient.h>
#include <Hash.h>
#include <Base64.h>

const char* ssid = "IoT";
const char* password = "1t3s0IoT23";
const char* mqttServer = "https://a3fqgfoikj274h-ats.iot.us-east-1.amazonaws.com/";
const int mqttPort = 8883;
const char* mqttUser = "ESP32-Nelida"; // MQTT client ID or Thing Name
String mqttPassword = ""; // Derived MQTT password

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Derive mqttPassword
  mqttPassword = generateMQTTPassword();

  // Set up MQTT client
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Connect to MQTT broker
  connectToMQTT();
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();
}

void connectToMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    
    if (client.connect("ESP32Client", mqttUser.c_str(), mqttPassword.c_str())) {
      Serial.println("Connected to MQTT");
      client.subscribe("test/topic"); // Subscribe to a test topic
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

String generateMQTTPassword() {
  // Load private key from file (for demonstration, replace with your actual key)
  String privateKey = "-----BEGIN PRIVATE KEY-----\n"
                      "Your private key content here\n"
                      "-----END PRIVATE KEY-----\n";

  // Remove header and footer from private key
  privateKey.replace("-----BEGIN PRIVATE KEY-----\n", "");
  privateKey.replace("-----END PRIVATE KEY-----\n", "");

  // Hash the private key using SHA-256
  SHA256 sha256;
  sha256.update((const uint8_t*)privateKey.c_str(), privateKey.length());
  uint8_t* hash = sha256.finalize();

  // Base64 encode the hashed key
  char base64Hash[43]; // Base64 hash length (32 bytes hashed to 43 Base64 characters)
  Base64.encode(hash, 32, (uint8_t*)base64Hash, sizeof(base64Hash));

  return String(base64Hash);
}