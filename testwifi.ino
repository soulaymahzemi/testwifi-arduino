#include <strings_en.h>

#include "DHT.h"
#include "PubSubClient.h" 

// Code for the ESP32
#include "WiFi.h" 
#define DHTPIN 4   


#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE); 

// WiFi
const char* ssid = "Soulayma";                 // Your personal network SSID
const char* wifi_password = "11111111"; // Your personal network password
const char* mqtt_server = "192.168.137.1";  // IP of the MQTT broker
const char* humidity_topic = "esp32/dht/humidity";
const char* temperature_topic = "esp32/dht/temperature";
const char* mqtt_username = ""; // MQTT username
const char* mqtt_password = ""; // MQTT password
const char* clientID = " "; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 


// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }


  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

 
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  connect_MQTT();
  Serial.setTimeout(2000);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" °C");

  // MQTT can only transmit strings
  String hs="Hum: "+String((float)h)+" % ";
  String ts="Temp: "+String((float)t)+" °C ";

  
  if   (client.publish(temperature_topic, String(ts).c_str())) {
    Serial.println("Temperature sent");
  }

  else {
    Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); 
    client.publish(temperature_topic, String(ts).c_str());
  }


  client.disconnect();  // disconnect from the MQTT broker
  delay(1000*60);       // print new values every 1 Minute
}
