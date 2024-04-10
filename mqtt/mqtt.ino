#include "SPIFFS.h" //Permite subir archivos a la memoria del ESP32
#include <WiFiClientSecure.h> //conectarse y poder crear un cliente
#include <PubSubClient.h> //MQTT


const char* ssid = "IoT";
const char* password = "1t3s0IoT23";

const char* mqtt_server = "a3fqgfoikj274h-ats.iot.us-east-2.amazonaws.com";
const int mqtt_port = 8883;



// Variables para archivos
String Read_rootca;
String Read_cert;
String Read_privatekey;




//Algo aqui
//********************************
#define BUFFER_LEN  256
long lastMsg = 0;
char msg[BUFFER_LEN];
int value = 0;
byte mac[6];
char mac_Id[18];
int count = 1;
//********************************





// config del cliente mqtt
WiFiClientSecure espClient;
PubSubClient client(espClient);




// funcion conectarse a wifi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando.. ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP()); //IP local del ESP32
}





// comunicacion del MQTT
//Callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}





// conectarnos al servidor mqtt constantemente
//Conectar a broker MQTT
void reconnect() {
  
  // Loop para reconección
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Creando un ID como ramdon
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    
    // Intentando conectarse
    if (client.connect(clientId.c_str())) {
      Serial.println("conectada");
      
    // Conectado, publicando un payload...
      client.publish("ei_out", "hello world");
    
    // ... y suscribiendo
      client.subscribe("ei_in");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" Esperando 5 segundos");
      
      // Tiempo muerto de 5 segundos
      delay(5000);
    }
  }
}



void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  setup_wifi();   //conectarse a internet
  delay(1000);

  // ------------------------------LECTURA DE ARCHIVOS---------------------------------
    //****************
    if (!SPIFFS.begin(true)) {
        Serial.println("Se ha producido un error al montar SPIFFS");
        return;
    }
    //**********************
    //Root CA leer archivo.
    File file2 = SPIFFS.open("/AmazonRootCA1.pem", "r");
    if (!file2) {
        Serial.println("No se pudo abrir el archivo para leerlo");
        return;
    }
    Serial.println("Root CA File Content:");
    while (file2.available()) {
        Read_rootca = file2.readString();
        Serial.println(Read_rootca);
    }
    //*****************************
    // Cert leer archivo
    File file4 = SPIFFS.open("/bee5fbf52-certificate.pem.crt", "r");
    if (!file4) {
        Serial.println("No se pudo abrir el archivo para leerlo");
        return;
    }
    Serial.println("Cert File Content:");
    while (file4.available()) {
        Read_cert = file4.readString();
        Serial.println(Read_cert);
    }
    //***************************************
    //Privatekey leer archivo
    File file6 = SPIFFS.open("/bee5fbf52-private.pem.key", "r");
    if (!file6) {
        Serial.println("No se pudo abrir el archivo para leerlo");
        return;
    }
    Serial.println("privateKey contenido:");
    while (file6.available()) {
        Read_privatekey = file6.readString();
        Serial.println(Read_privatekey);
    }
    //=====================================================

    // Procesamiento de datos
    char* pRead_rootca;
    pRead_rootca = (char *)malloc(sizeof(char) * (Read_rootca.length() + 1));
    strcpy(pRead_rootca, Read_rootca.c_str());

    char* pRead_cert;
    pRead_cert = (char *)malloc(sizeof(char) * (Read_cert.length() + 1));
    strcpy(pRead_cert, Read_cert.c_str());

    char* pRead_privatekey;
    pRead_privatekey = (char *)malloc(sizeof(char) * (Read_privatekey.length() + 1));
    strcpy(pRead_privatekey, Read_privatekey.c_str());

    Serial.println("================================================================================================");
    Serial.println("Certificados que pasan adjuntan al espClient");
    Serial.println();
    Serial.println("Root CA:");
    Serial.write(pRead_rootca);
    Serial.println("================================================================================================");
    Serial.println();
    Serial.println("Cert:");
    Serial.write(pRead_cert);
    Serial.println("================================================================================================");
    Serial.println();
    Serial.println("privateKey:");
    Serial.write(pRead_privatekey);
    Serial.println("================================================================================================");



    // Vinculacion con el ESP32 con el cliente wifi creado
    espClient.setCACert(pRead_rootca);
    espClient.setCertificate(pRead_cert);
    espClient.setPrivateKey(pRead_privatekey);

    // cliente declara a que server conectarse de Aws
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);


    // NO LO EXPLICA EL COMPA
    //******************************************
    WiFi.macAddress(mac);
    snprintf(mac_Id, sizeof(mac_Id), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.println(mac_Id);
    //****************************************
    delay(2000);




  
}

void loop() {
  // Continuar con conexcion mqtt
    if (!client.connected()) {
        reconnect();
    }
    client.loop();





    // si han pasado mas de 5 segundos, hcaer json 
    long now = millis();
    if (now - lastMsg > 5000) {
        lastMsg = now;
        //=============================================================================================
//        String macIdStr = mac_Id;
//        String Temprature = String(t);
//        String Humidity = String(h);                                                                // convirtiendo a str los valores para el json
        snprintf (msg, BUFFER_LEN, "{\"mac_Id\" : \"hola\", \"Temperatura\" : 50, \"Humedad\" : 10}");
        Serial.print("Publicando mensaje: ");
        Serial.print(count);
        Serial.println(msg);
        // publicacion en el topic sensor, msg es el json
        client.publish("sensor", msg);
        count = count + 1;
        //================================================================================================
    }
    // parpadeo en el led que viene soldado en la placa
    digitalWrite(2, HIGH);   
    delay(1000);                      
    digitalWrite(2, LOW);   
    delay(1000);   

}
