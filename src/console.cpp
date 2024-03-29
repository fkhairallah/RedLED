/**********************************************************************************
 * 
 *    Implements functionality for telnet and serial console
 * 
 *********************************************************************************/
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <RedGlobals.h>


dConsole console;

/*
 * ********************************************************************************

  ********************  CUSTOMIZABLE SECTION  ***************************

 * ********************************************************************************
*/
#define CUSTOM_COMMANDS "Custom Commands: on, off, mode #, led #, test, noaa"

void executeCustomCommands(char* commandString,char* parameterString)
{
  if (strcmp(commandString, "on") == 0)     setLEDPower((char *)"1");
  if (strcmp(commandString, "off") == 0)    setLEDPower((char *)"0");
  if (strcmp(commandString, "mode") == 0)
  {
    console.print("Color mode: ");
    console.println(parameterString);
    setLEDMode(atoi(parameterString));
  }

  if (strcmp(commandString, "led") == 0)
  {
    strcpy(numberOfLED, parameterString);
    configLED();
    writeConfigToDisk();
    console.printf("Number of LEDs changed to %s\r\n", numberOfLED);
  }

  if (strcmp(commandString, "test") == 0)
  {
    console.println(CUSTOM_COMMANDS);
    testLED();
  }

  if (strcmp(commandString, "noaa") == 0) {
    strcpy(NoaaStation, parameterString);
    writeConfigToDisk();
    console.printf("NOAA station changed to %s\r\n", NoaaStation);
  }
}

/*
 * ********************************************************************************

    ********************  END OF CUSTOMIZABLE SECTION  ***************************

 * ********************************************************************************
*/

void setupConsole()
{
  console.enableSerial(&Serial, true);
  console.enableTelnet(23);
}


void handleConsole()
{
  // console
  if (console.check())
  {
    executeCustomCommands(console.commandString, console.parameterString);

    if (strcmp(console.commandString, "?") == 0)
    {
      console.print("[RED]");
      console.print(MQTT_TOPIC_PREFIX);
      console.print(" ");
      console.println(VERSION);
      console.printf("Host: %s - %s @", myHostName, deviceLocation);
      console.println(WiFi.localIP().toString());
      console.printf("MQTT Server %s, port: %s, LED: %s\r\n", mqttServer, mqttPort, numberOfLED);
      console.println("Commands: ?, debug, location room, mqtt server, reset (Factory), reboot, quit");
      console.println(CUSTOM_COMMANDS);
    }
    if (strcmp(console.commandString, "debug") == 0)
    {
      debugMode = !debugMode;
      console.print("Debug mode is now ");
      console.println(debugMode);
    }
    if (strcmp(console.commandString, "reset") == 0)
    {
      console.print("Reseting configuration...");
      //reset settings - for testing
      WiFiManager wifiManager;
      wifiManager.resetSettings();
      console.println(" Done.");
    }
    if (strcmp(console.commandString, "reboot") == 0)
    {
      console.print("Rebooting...");
      delay(200);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
    if (strcmp(console.commandString, "mqtt") == 0)
    {
      strcpy(mqttServer, console.parameterString);
      writeConfigToDisk();
      console.print("MQTT server changed to ");
      console.println(mqttServer);
      mqttDisconnect();
    }
    if (strcmp(console.commandString, "location") == 0)
    {
      strcpy(deviceLocation, console.parameterString);
      writeConfigToDisk();
      console.printf("location changed to %s\r\n", deviceLocation);
      console.println("Change will take effect after next reboot");
    }
    if (strcmp(console.commandString, "quit") == 0)
    {
      console.print("quiting...");
      console.closeTelnetConnection();
      delay(500);
      console.println("");
    }

    console.print("[RED]> ");
  }
}
