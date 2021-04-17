#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <RedGlobals.h>

dConsole console;
/*
 * ********************************************************************************

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
    //char str[128];
    if (strcmp(console.commandString, "?") == 0)
    {
      console.print("\n\n\nCabinetsLED ");
      console.println(VERSION);
      console.print("IP address: ");
      console.println(WiFi.localIP().toString());
      console.println("Available commands are: on, off, mode x, led #, location room, mqtt server, status, reset (Factory), reboot");
    }
    if (strcmp(console.commandString, "on") == 0) setLEDPower((char*)"1");
    if (strcmp(console.commandString, "off") == 0 )setLEDPower((char*)"0");
    if (strcmp(console.commandString, "mode") == 0) {
      console.print("Color mode: ");
      console.println(console.parameterString);
      setLEDMode(atoi(console.parameterString));
    }

    if (strcmp(console.commandString, "led") == 0)
    {
      strcpy(numberOfLED, console.parameterString);
      writeConfigToDisk();
      console.printf("Number of LEDs changed to %s\r\n", numberOfLED);
    }


    if ( strcmp(console.commandString, "reset") == 0)
    {
      console.print("Reseting configuration...");
      //reset settings - for testing
      WiFiManager wifiManager;
      wifiManager.resetSettings();
      console.println(" Done.");

    }
    if ( strcmp(console.commandString, "reboot") == 0)
    {
      console.print("Rebooting...");
      delay(200);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
    if ( strcmp(console.commandString, "status") == 0)
    {
      console.printf("Location: %s, hostname: %s\r\n", deviceLocation, myHostName);
      console.printf("IP address: x.x.%i.%i\r\n", WiFi.localIP()[2], WiFi.localIP()[3]);
      console.printf("MQTT Server %s, port: %s\r\n", mqttServer, mqttPort);
      console.printf("Number of LEDs: %s\r\n", numberOfLED);

    }


    if (strcmp(console.commandString, "mqtt") == 0)
    {
      strcpy(mqttServer, console.parameterString);
      console.println(mqttServer);
      mqttDisconnect();
      writeConfigToDisk();
      console.print("MQTT server changed to ");
      console.println(mqttServer);

    }
    if (strcmp(console.commandString, "location") == 0)
    {
      strcpy(deviceLocation, console.parameterString);
      writeConfigToDisk();
      console.printf("location changed to %s\r\n", deviceLocation);
      console.println("Change will take effect after next reboot");
    }

    console.print("[RED]> ");
  }

}
