/*
 * ********************************************************************************

   This code implements all sensor related function
 * ********************************************************************************
*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <pins.h>
#include <RedGlobals.h>

#define TEMPERATURE_PRECISION 9 // Lower resolution


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

void (*sensorCallBackFunction)(float temp); // holds the callback function pointer

unsigned long lastSensorQuery;       // holds time since last sensor query
unsigned long sensorSendInterval;    // interval to send temp update

/*
 * ********************************************************************************

   configure sensors, get # and set resolution

 * ********************************************************************************
*/

void configSensors(long interval, void (*sensorCallback)(float temp))
{

  sensorSendInterval = interval;            // remember the interval to send temp update
  sensorCallBackFunction = sensorCallback;  // remember the callback function

  // Start up the library
  sensors.begin();

  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  console.printf("Found %i 1Wire Devices\r\n",numberOfDevices);

  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
    }
  }

  lastSensorQuery = millis();   // remember the time....
}

/*
 * ********************************************************************************

   Service sensors routine requests temperature reading every period
   It then wait 1 second, reads the temperature and calls a callback
   routine to report it back to the main code
 * ********************************************************************************
*/

void serviceSensors()
{

  if ((unsigned long)(millis() - lastSensorQuery) >= sensorSendInterval)
  {
    // read temperature and convert to F
    float tempC = sensors.getTempC(tempDeviceAddress);
    if (tempC > -127)    // -127 is error
    {
      float tempF = DallasTemperature::toFahrenheit(tempC);
      //    console.print("Temp C: " + String(tempC));
      //    console.println(" Temp F: " + String(tempF)); // Converts tempC to Fahrenheit

      // invoke the callback function
      sensorCallBackFunction(tempF);
    }
    lastSensorQuery = millis();
  }
  else
  {
    // about 1 second before we need it --> issue a command to read sensors
    if ((unsigned long)(millis() - lastSensorQuery) >= (sensorSendInterval - 1000))
    {
      sensors.requestTemperatures(); // Send the command to get temperatures
    }
  }

}
