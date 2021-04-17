
#ifndef RED_GLOBALS_H
#define RED_GLOBALS_H
#include <dConsole.h>
#include <PubSubClient.h>
#include <Ticker.h>

#include <dConsole.h>

/*
 * ********************************************************************************
 *            START CONFIGURATION SECTION
 * ********************************************************************************
*/

#define _TEMP_SENSOR_PERIOD 10000         // in ms the frequency of temperature sensor reading
#define _SEND_ROOM_TEMP_INTERVAL_MS 60000 // in ms how often the temperature is sent back to the server
#define _DISPLAY_INTERVAL 5000            // in ms how long before the display is dimmed then turned off
#define TEMPERATURE_PRECISION 9           // Lower resolution

#define VERSION "V1.3-PIO"
#define MQTT_TOPIC_PREFIX "led" // prefix for all MQTT topics

// in WiFiConfigurations.ino
extern char myHostName[];
extern char deviceLocation[];
extern char mqttServer[];
extern char mqttPort[];
extern char mqttUser[];
extern char mqttPwd[];
extern char numberOfLED[]; // nunber of leds in the strings
void configureESP();       // load configuration from FLASH & configure WIFI
void checkConnection(); // check WIFI connection
void writeConfigToDisk();
void configureOTA(char *hostName);

// in MQTT
extern PubSubClient mqtt_client;
extern char mqtt_topic[];
extern char mqtt_temperature_topic[];
extern char mqtt_outdoortemperature_topic[];
extern char mqtt_doorbell_topic[];
extern char mqtt_garagedoor_topic[];
extern char mqtt_debug_topic[];
extern char mqtt_debug_set_topic[];
void configureMQTT();
bool checkMQTTConnection();
void mqttDisconnect();
void mqttCallback(char *topic, byte *payload, unsigned int length);

// in console.ino
extern dConsole console;
void setupConsole();
void handleConsole();

// in Sensors.ino
void configSensors(long interval, void (*sensorCallback)(float insideTemp, float outsideTemp));
void serviceSensors();

// in lighting.ino
void initializeLED(); // turn all LEDs off.
void configLED();     // update with actual # of LED
void setLEDPower(char *mode);   // set LED power
void setLEDMode(int mode);      // & mode
void executeLED();
void fillList(uint32_t list[], int count);
void fillRainbow();

// in RedGarage
void updateTemperature(float temp, float outdoorTemp);


#endif
