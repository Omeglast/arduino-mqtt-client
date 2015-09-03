
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <cc3000_PubSubClient.h>

#include "iotapi_client.h"
#include "config.h"

cc3000_PubSubClient mqttclient(server.ip, MQTT_PORT, callback, client, cc3000);

void callback (char* topic, byte* payload, unsigned int length) {
}

void setup(void)
{
  #ifdef DEBUG
    Serial.begin(115200);
    Serial.println(F("Hello, CC3000!\n"));
  #endif

  #ifdef DEBUG
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
}

void loop(void) {
  
  unsigned long time = millis();

  // cheap tricks I tell you
  char stringTime[10];
  ltoa(time, stringTime, 10);

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
  }
  else {
    // yep, publish that test
    mqttclient.publish("time", stringTime);
  }
  #ifdef DEBUG
    Serial.println(stringTime);
  #endif
  delay(5000);
}


