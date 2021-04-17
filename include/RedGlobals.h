
#ifndef RED_GLOBALS_H
#define RED_GLOBALS_H
#include <dConsole.h>

/*
 * ********************************************************************************
 *            START CONFIGURATION SECTION
 * ********************************************************************************
*/

#define _TEMP_SENSOR_PERIOD 10000         // in ms the frequency of temperature sensor reading
#define _SEND_ROOM_TEMP_INTERVAL_MS 60000 // in ms how often the temperature is sent back to the server
#define _DISPLAY_INTERVAL 5000            // in ms how long before the display is dimmed then turned off


#define version "V1.3-PIO"

extern dConsole console;

// configuration parameters
// Hostname, AP name & MQTT clientID
extern char myHostName[];

//define your default values here, if there are different values in config.json, they are overwritten.
extern char deviceLocation[];
extern char mqttServer[];
extern char mqttPort[];
extern char mqttUser[];
extern char mqttPwd[];
extern char numberOfLED[];  // nunber of leds in the strings

// 
void configureMQTT();
bool mqttConnect();
void updateTemperature(float temp);
void mqttCallback(char* topic, byte * payload, unsigned int length);

// in console.ino
void setupConsole();
void handleConsole();

// in WiFiConfigurations.ino
void configureESP(); // load configuration from FLASH & configure WIFI
void checkConnection();   // check WIFI connection
void writeConfigToDisk();
void mqttDisconnect();
void configureOTA(char *hostName);


// in Sensors.ino
void configSensors(long interval, void (*sensorCallback)(float temp));
void serviceSensors();

// in lighting.ino
void initializeLED(); // turn all LEDs off.
void configLED();     // update with actual # of LED
void setLEDPower(char *mode);   // set LED power
void setLEDMode(int mode);      // & mode
void executeLED();
void fillList(uint32_t list[], int count);
void fillRainbow();

#endif
