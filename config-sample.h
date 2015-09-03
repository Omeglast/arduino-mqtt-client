/**
 * This is a sample configuration file for the Arduino's MQTT client
 *
 * Please rename this file into "config.h" and update it's values
 * according to your needs.
 */

#ifndef arduinomqttclient_config_h
#define arduinomqttclient_config_h

#define WLAN_SSID       "my_wlan_ssid"
#define WLAN_PASS       "my_wlan_password"

/**
 * Ip of the MQTT server, in a reverse way.
 * Means that loopback 127.0.0.1 will be defined as {1, 0, 0, 127}
 */
ArrayToIp server = {1, 0, 0, 127};

#endif
