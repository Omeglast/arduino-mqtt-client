#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <cc3000_PubSubClient.h>

//#define aref_voltage 3.3
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER);
#define WLAN_SSID       "/tmp/lab"
#define WLAN_PASS       "bitte2cucung"
//#define WLAN_SSID       "Fraggl"
//#define WLAN_PASS       "futurama"

#define WLAN_SECURITY   WLAN_SEC_WPA2
Adafruit_CC3000_Client client;

//TMP36
//int tempPin = 1;        
//int tempReading;

// We're going to set our broker IP and union it to something we can use
union ArrayToIp {
  byte array[4];
  uint32_t ip;
};

/*ArrayToIp server = {
  192,168,2,101 }*/
//ArrayToIp server = {10, 255, 0, 155};
ArrayToIp server = {155, 0, 255, 10};
cc3000_PubSubClient mqttclient(server.ip, 1883, callback, client, cc3000);

void callback (char* topic, byte* payload, unsigned int length) {
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n"));

//  analogReference(EXTERNAL);
  Serial.println(F("\nInitialising the CC3000 ..."));
  if (!cc3000.begin()) {
    Serial.println(F("Unable to initialise the CC3000! Check your wiring?"));
    for(;;);
  }

/*  Serial.println(F("\nDeleting old connection profiles"));
  if (!cc3000.deleteProfiles()) {
    Serial.println(F("Failed!"));
    while(1);
  }*/

  /* Attempt to connect to an access point */
  char *ssid = WLAN_SSID;             /* Max 32 chars */
  Serial.print(F("\nAttempting to connect to "));
  Serial.println(ssid);

  /* NOTE: Secure connections are not available in 'Tiny' mode! */
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }

  Serial.println(F("Connected to WiFi!"));
  // Display the IP address DNS, Gateway, etc.
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  Serial.println(F("Connected to server ?"));
  // connect to the broker
  if (!client.connected()) {
    Serial.println("Connecting to TCP server...");
    client = cc3000.connectTCP(server.ip, 1883);
    Serial.println("Server connected");
  } else {
    Serial.println("Server already connected");
  }


  // did that last thing work? sweet, let's do something
  if(client.connected()) {
    if (mqttclient.connect("home")) {
      mqttclient.publish("home","home");
      Serial.println("Home connected");
    }
  }
}

void loop(void) {
//  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  
  unsigned long time = millis();

  // cheap tricks I tell you
  char stringTime[10];
//  dtostrf(time, 0, 3, stringTime);
  ltoa(time, stringTime, 10);

  // are we still connected?
  if (!client.connected()) {
    client = cc3000.connectTCP(server.ip, 1883);
    Serial.println("Server connected");
    if(client.connected()) {
      if (mqttclient.connect("client")) {
        mqttclient.publish("home","home");
        Serial.println("Home connected");
      }
    }
  }
  else {
    // yep, publish that test
    mqttclient.publish("time", stringTime);
  }
  Serial.println(stringTime);
  delay(5000);
}


// Tries to read the IP address and other connection details
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
