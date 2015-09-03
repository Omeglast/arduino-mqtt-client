#ifndef IOTAPI_CLIENT_H
#define IOTAPI_CLIENT_h

// We're going to set our broker IP and union it to something we can use
union ArrayToIp {
  byte array[4];
  uint32_t ip;
};

#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER);


#define WLAN_SECURITY   WLAN_SEC_WPA2
Adafruit_CC3000_Client client;


#endif
