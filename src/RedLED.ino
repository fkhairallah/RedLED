/************************************************************************************

   LED ESP

   Hardware Configuration parameters are found in .h file

  This code provides WIFI & MQTT configuration through the use of WIFI manager. Initial setup allow
  configuration of WIFI network and MQTT server

  It provides OTA functionality.

 *************************************************************************************/
#include <Ticker.h>
#include <pins.h>
#include <RedGlobals.h>

#include <MQTT.h>



float tempAccumulator;   // keeps sum of  temperature as reported by remote thermostat
int tempNumberOfReading;  // keeps # of readings
unsigned long lastTempSend;
float averageTemp;        // Average temp for the last interval -- what is displayed

int secondsWithoutMQTT;

Ticker ticker;

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

  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  setupConsole();



  console.print("[RED]LED ");
  console.println(version);


  // Configure WIFI
  configureESP(); // load configuration from FLASH & configure WIFI

  configLED();  // update with actual # of LED

  digitalWrite(blueLED, LOW);
  console.print("Connected! IP address: ");
  console.println(WiFi.localIP().toString());

  configureMQTT();


  // configure thermostat sensor
  tempAccumulator = 0;
  tempNumberOfReading = 0;
  averageTemp = -9999;
  lastTempSend = millis();
  configSensors(_TEMP_SENSOR_PERIOD, &updateTemperature);


  console.println("Ready!");
}


/*
 * ********************************************************************************

   main loop services all modules: Wifi, MQTT, HP, console and webserver

 * ********************************************************************************
*/
void loop() {

  checkConnection();  // check WIFI connection

  // service temperature and other sensors
  serviceSensors();

  // handle MQTT by reconnecting if necessary then executing a mqtt.loop() to service requests
  if (!mqtt_client.connected())
  {
    ticker.attach(1, tick); // start blink
    mqttConnect();  // retry connection
    if ( (secondsWithoutMQTT % 50) == 1 ) console.println("No MQTT Connection");
    if (secondsWithoutMQTT++ > 600) // after a few minutes -- reset
    {
      console.println("Failed to connect to MQTT! What to do???? Resetting...");
      delay(200);
      ESP.reset(); //reset and try again
      delay(5000);
    }
  }
  else
  {
    // connected -- stop blinking and reset counter
    ticker.detach();
    digitalWrite(blueLED, HIGH); // turn system LED off
    secondsWithoutMQTT = 0;
    mqtt_client.loop();
  }


  // handle any commands from console
  handleConsole();

}


/*
 * ********************************************************************************

   This is callback routine that is called when the temperature sensor receives
   a reading. It collects the data into a long term average then,
   every _TEMP_SENSOR_PERIOD report the value back to the MQTT server

 * ********************************************************************************
*/
void updateTemperature(float temp)
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

/*
 * ********************************************************************************

   Configure the MQTT server by:
    - create all the topic using prefix/location/subtopic
    - configure MQTT server and port and setup callback routine
    - attempt a connection and log to debug topic if success

 * ********************************************************************************
*/

void configureMQTT()
{
  // configure the topics using location
  // heatpump/location/...
  sprintf(mqtt_main_topic, "%s/%s", mqttTopicPrefix, deviceLocation);
  sprintf(mqtt_led_command, "%s/%s/set", mqttTopicPrefix, deviceLocation);
  sprintf(mqtt_led_mode, "%s/%s/mode", mqttTopicPrefix, deviceLocation);
  sprintf(mqtt_temperature_topic, "%s/%s/temperature", mqttTopicPrefix, deviceLocation);
  sprintf(mqtt_debug_topic, "%s/%s/debug", mqttTopicPrefix, deviceLocation);
  sprintf(mqtt_debug_set_topic, "%s/%s/debug/set", mqttTopicPrefix, deviceLocation);

  // configure mqtt connection
  mqtt_client.setServer(mqttServer, atoi(mqttPort));
  mqtt_client.setCallback(mqttCallback);

  console.print("MQTT Server :'");
  console.print(mqttServer);
  console.print("' Port: ");
  console.print(String(atoi(mqttPort)));
  console.print(" Topic set to: '");
  console.print(mqtt_main_topic);
  console.println("'");


}

/*
 * ********************************************************************************

   attemps a connection to the MQTT server. if it fails increment secondsWithoutMQTT
   and return.
   This code relies on an existing Wifi connection which checked and dealt with
   elsewhere in the code

   Future code might turn the webserver on/off depending on MQTT connection

 * ********************************************************************************
*/

bool mqttConnect() {

  if (!mqtt_client.connected()) {
    // Attempt to connect
    if (mqtt_client.connect(myHostName, mqttUser, mqttPwd))
    {
      mqtt_client.subscribe(mqtt_led_command);
      mqtt_client.subscribe(mqtt_led_mode);
      mqtt_client.subscribe(mqtt_debug_set_topic);
      console.println(mqtt_main_topic);
      console.println(mqtt_led_command);
      console.println("Connected to MQTT");
      char str[128];
      sprintf(str, "CabinetsLED %s booted at %i.%i.%i.%i", deviceLocation, WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
      mqtt_client.publish(mqtt_debug_topic, str, true);
      secondsWithoutMQTT = 0;
      return true;
    }
    else
    {
      delay(500);
      secondsWithoutMQTT++;
      return false;
    }
  }
  return true;
}


/*
 * ********************************************************************************

   This routine handles all MQTT callbacks and processes the commands sent to hp
   1. it changes the configuration sent to /set topic
   2. it updates the remote temp sent to /set
   3. it sends custom packets sent to /set (NOT USED)
   4. turns debug on/off

 * ********************************************************************************
*/

void mqttCallback(char* topic, byte * payload, unsigned int length) {

  // Copy payload into message buffer
  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  console.print("Received Topic=");
  console.print(topic);
  console.print(", message=");
  console.println(message);

  if (strcmp(topic, mqtt_led_command) == 0) {
    setLEDPower(message);

    // publish state back to main topic
    mqtt_client.publish(mqtt_main_topic, message);
  }
  else if (strcmp(topic, mqtt_led_mode) == 0) {
    setLEDMode(atoi(message));

    // publish state back to main topic
    mqtt_client.publish(mqtt_main_topic, message);
  }
  else
  {
    if (strcmp(topic, mqtt_debug_set_topic) == 0) {
      if (strcmp(message, "on") == 0) {
        _debugMode = true;
        mqtt_client.publish(mqtt_debug_topic, "debug mode enabled");
      } else if (strcmp(message, "off") == 0) {
        _debugMode = false;
        mqtt_client.publish(mqtt_debug_topic, "debug mode disabled");
      }
    } else {
      mqtt_client.publish(mqtt_debug_topic, strcat((char*)"wrong mqtt topic: ", topic));
    }
  }
}
