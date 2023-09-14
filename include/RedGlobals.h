
#ifndef RED_GLOBALS_H
#define RED_GLOBALS_H
#include <dConsole.h>
#include <PubSubClient.h>
#include <Ticker.h>

#ifndef _PINS_H
#include <pins.h>
#endif

#include <dConsole.h>

/*
 * ********************************************************************************
 *            START CONFIGURATION SECTION
 * ********************************************************************************
*/

#define TEMP_SENSOR_PRESENT                 // indicates a temperature sensor is present

#ifdef TEMP_SENSOR_PRESENT
#define _TEMP_SENSOR_PERIOD 10000         // in ms the frequency of temperature sensor reading
#define _SEND_ROOM_TEMP_INTERVAL_MS 60000 // in ms how often the temperature is sent back to the server
#define TEMPERATURE_PRECISION 9           // Lower resolution
#endif

#define VERSION "V2.0"      // N.B: document changes in README.md
#define MQTT_TOPIC_PREFIX "tide" // prefix for all MQTT topics

// in WiFiConfig
extern char myHostName[];
extern char deviceLocation[];
extern char mqttServer[];
extern char mqttPort[];
extern char mqttUser[];
extern char mqttPwd[];
extern char numberOfLED[]; // nunber of leds in the strings
extern char NoaaStation[];
void configureESP();       // load configuration from FLASH & configure WIFI
void checkConnection(); // check WIFI connection
void writeConfigToDisk();
void configureOTA(char *hostName);

// in MQTTConfig
extern bool debugMode;
void configureMQTT();
bool checkMQTTConnection();
void mqttDisconnect();
void mqttCallback(char *topic, byte *payload, unsigned int length);

// in console.ino
extern dConsole console;
void setupConsole();
void handleConsole();

#ifdef TEMP_SENSOR_PRESENT
// in Sensors.ino
void configSensors(long interval, void (*sensorCallback)(float insideTemp, float outsideTemp));
void serviceSensors();
#endif


// in lighting.ino
void initializeLED(); // turn all LEDs off.
void configLED();     // update with actual # of LED
void testLED();       // test all LEDs
void setLEDPower(char *mode);   // set LED power
void setLEDMode(int mode);      // & mode
void executeLED();
void stripFill(uint32_t color);
void fillList(uint32_t list[], int count);
void fillRainbow();

// in RedLED
void ledON();
void ledOFF();
void tick();
#ifdef TEMP_SENSOR_PRESENT
void updateTemperature(float temp, float outdoorTemp);
#endif

#endif
