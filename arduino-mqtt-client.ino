
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "cc3000_PubSubClient.h"

#include "iotapi_client.h"
#include "config.h"

void sendMessage(char* topic, char* message);

void callback (char* topic, byte* payload, unsigned int length) {
}

cc3000_PubSubClient mqttclient(server.ip, MQTT_PORT, callback, client, cc3000);
DHT_Unified dht(DHTPIN, DHTTYPE);

void setup(void)
{
  #ifdef DEBUG
    Serial.begin(115200);
    Serial.println(F("Hello, CC3000!\n"));
    Serial.println(F("\nInitialising the CC3000 ..."));
  #endif

  if (!cc3000.begin()) {
    #ifdef DEBUG
      Serial.println(F("Unable to initialise the CC3000! Check your wiring?"));
    #endif
    for(;;);
  }

  /* Attempt to connect to an access point */
  #ifdef DEBUG
    char *ssid = WLAN_SSID;             /* Max 32 chars */
    Serial.print(F("\nAttempting to connect to "));
    Serial.println(ssid);
  #endif

  /* NOTE: Secure connections are not available in 'Tiny' mode! */
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    #ifdef DEBUG
      Serial.println(F("Failed!"));
    #endif
    while(1);
  }

  #ifdef DEBUG
    Serial.println(F("Connected to WiFi!"));
  #endif
  // Display the IP address DNS, Gateway, etc.
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  #ifdef DEBUG
    Serial.println(F("Connected to server ?"));
  #endif
  // connect to the broker
  if (!client.connected()) {
    #ifdef DEBUG
      Serial.println("Connecting to TCP server...");
    #endif
    client = cc3000.connectTCP(server.ip, 1883);
    #ifdef DEBUG
      Serial.println("Server connected");
    #endif
  } else {
    #ifdef DEBUG
      Serial.println("Server already connected");
    #endif
  }

  // did that last thing work? sweet, let's do something
  if(client.connected()) {
    if (mqttclient.connect("home")) {
      mqttclient.publish("home","home");
      #ifdef DEBUG
        Serial.println("Home connected");
      #endif
    }
  }

  // Start DHT Sensor
  dht.begin();
  #ifdef DEBUG
    debugSensorData();
  #endif
}

/**
 * The main loop
 */
void loop(void) {
  char message[10];
  
  // are we still connected?
  if (!client.connected()) {
    client = cc3000.connectTCP(server.ip, 1883);
    #ifdef DEBUG
      Serial.println("Server connected");
    #endif
    if(client.connected()) {
      if (mqttclient.connect("client")) {
        mqttclient.publish("home","home");
        #ifdef DEBUG
          Serial.println("Home connected");
        #endif
      }
    }
  } else {
    // Send sensor data
    sendMessage(MQTT_TEMPERATURE_TOPIC, getTemperature(dht));
    sendMessage(MQTT_HUMIDITY_TOPIC, getHumidity(dht));
  }
  delay(DELAY);
}

/**
 * Debug data from DHT sensor
 */
void debugSensorData() {
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("DHTxx Unified Sensor Example");
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
}

/**
 * Read humidity value from DHT sensor
 * 
 * @param DHT_Unified sensor
 * @return char*
 */
char* getHumidity(DHT_Unified dhtSensor) {
  sensors_event_t event;  
  char message[10];
  dhtSensor.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    #ifdef DEBUG
      Serial.println("Error reading temperature!");
    #endif
  }
  else {
    #ifdef DEBUG
      Serial.print("Humidity: ");
      Serial.print(event.relative_humidity);
      Serial.println("%");
    #endif
    dtostrf(event.relative_humidity, 2, 3, message);
  }
  return message;
}

/**
 * Read temperature value from DHT sensor
 * 
 * @param DHT_Unified sensor
 * @return char*
 */
char* getTemperature(DHT_Unified dhtSensor) {
  sensors_event_t event;  
  char message[10];
  dhtSensor.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    #ifdef DEBUG
      Serial.println("Error reading temperature!");
    #endif
  } else {
    #ifdef DEBUG
      Serial.print("Temperature: ");
      Serial.print(event.temperature);
      Serial.println(" *C");
    #endif
    dtostrf(event.temperature, 2, 3, message);
  }
  return message;
}

/**
 * Send Message to the MQTT server
 * @param char* topic
 * @param char* message
 */
void sendMessage(char* topic, char* message)
{
  mqttclient.publish(topic, message);
}
