/*
 * ********************************************************************************
 * 
 * This program runs undercabinet LED lights AND temperature sensor from a single
 * WEMOS D1 esp8266
 * 
 * V1.0 -- initial realease
 * V1.1 -- include new console library stream class
 *      -- Responds to "ON" "OFF" commands in addition to 1 & 0
 * V1.2 -- Move to PlateformIO
 * 
 * 
 * Hardware configuration:
 * 
 *   - Blue LED connected to pin 2
 *   - 1Wire thermocouple connected to pin 3
 *   - WS2812B individually addressable LED pin 4


  It interfaces with MQTT server through 4 Topics:
  1. led/location/default - LED status
  2. led/location/set - on/off commands 
  3. led/location/mode - mode/color of LED
  4. led/location/temperature -- report current temp in F

  In addition there are two more topics the allows you to debug the hp:
  1. led/location/debug -- forwards all packets exchanged with hp
  2. led/location/debug/set -- received command (on/off) to control debug mode


   Hardware Notes:

    - GPIO-0 must be tied to ground for programming
    - GPIO-0 floats to run program
    - GPIO-0 runs Red LED on Huzzah
    - GPIO-2 is tied to Blue Led (*NOT* a PWM pin)
    - GPIO-13 is RESERVED

 * ********************************************************************************
 */
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager



// MQTT Settings
// debug mode, when true, will send all packets received from the heatpump to topic mqtt_debug_topic
// this can also be set by sending "on" to mqtt_debug_set_topic
bool _debugMode = false;
bool retain = true; //change to false to disable mqtt retain

// sketch settings

// prefix for all MQTT topics
const char* mqttTopicPrefix = "led";

/*
 * ********************************************************************************
 *            END CONFIGURATION SECTION
 * ********************************************************************************
*/


WiFiClient espClient;
PubSubClient mqtt_client(espClient);


// mqtt client settings
char mqtt_main_topic[64]        = "led/LOCATION";  //contains current settings
char mqtt_led_command[64]       = "led/LOCATION/set"; // contains LED control commands
char mqtt_led_mode[64]          = "led/LOCATION/mode"; // contains LED mode
char mqtt_temperature_topic[64] = "led/LOCATION/temperature"; //temperature

char mqtt_debug_topic[64]        = "led/LOCATION/debug"; //debug messages
char mqtt_debug_set_topic[64]    = "led/LOCATION/debug/set"; //enable/disable debug messages

