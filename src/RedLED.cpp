/************************************************************************************

   LED ESP

   Hardware Configuration parameters are found in .h file

  This code provides WIFI & MQTT configuration through the use of WIFI manager. Initial setup allow
  configuration of WIFI network and MQTT server

  It provides OTA functionality.

 *************************************************************************************/
#include <Arduino.h>
#include <pins.h>
#include <RedGlobals.h>



float tempAccumulator;   // keeps sum of  temperature as reported by remote thermostat
int tempNumberOfReading;  // keeps # of readings
unsigned long lastTempSend;
float averageTemp;        // Average temp for the last interval -- what is displayed




void tick()
{
  //toggle state
  int state = digitalRead(blueLED); // get the current state of GPIO1 pin
  digitalWrite(blueLED, !state);    // set pin to the opposite state
}


/*
 * ********************************************************************************

   Perform the initial hardware setup and then sequence the starting of the
   various modules

 * ********************************************************************************
*/

void setup() {
  pinMode(blueLED, OUTPUT);
  pinMode(LED_DATA_PIN, OUTPUT);

  initializeLED();    // turn all LEDs off.

  setupConsole();

  console.print("[RED]LED ");
  console.println(VERSION);


  // Configure WIFI
  configureESP(); // load configuration from FLASH & configure WIFI

  configLED();  // update with actual # of LED

  console.print("Connected! IP address: ");
  console.println(WiFi.localIP().toString());

  configureMQTT();

#ifdef TEMP_SENSOR_PRESENT
  // configure thermostat sensor
  tempAccumulator = 0;
  tempNumberOfReading = 0;
  averageTemp = -9999;
  lastTempSend = millis();
  configSensors(_TEMP_SENSOR_PERIOD, &updateTemperature);
#endif

  console.println("Ready!");
  digitalWrite(blueLED, HIGH);
}


/*
 * ********************************************************************************

   main loop services all modules: Wifi, MQTT, HP, console and webserver

 * ********************************************************************************
*/
void loop() {

  checkConnection();  // check WIFI connection

#ifdef TEMP_SENSOR_PRESENT
  // service temperature and other sensors
  serviceSensors();
#endif

  checkMQTTConnection(); // check MQTT

  // handle any commands from console
  handleConsole();

}

#ifdef TEMP_SENSOR_PRESENT
/*
 * ********************************************************************************

   This is callback routine that is called when the temperature sensor receives
   a reading. It collects the data into a long term average then,
   every _TEMP_SENSOR_PERIOD report the value back to the MQTT server

 * ********************************************************************************
*/
void updateTemperature(float temp, float temp2)
{
  char str[128];
  console.println("Reporting temp reading of " + String(temp));
  tempAccumulator += temp;
  tempNumberOfReading++;

  if ((unsigned long)(millis() - lastTempSend) >= _SEND_ROOM_TEMP_INTERVAL_MS)
  {

    averageTemp = tempAccumulator / tempNumberOfReading;
    tempAccumulator = 0;
    tempNumberOfReading = 0;
    sprintf(str, "%.1f", averageTemp);
    mqtt_client.publish(mqtt_temperature_topic, str);


    tick();
  }

}
#endif
