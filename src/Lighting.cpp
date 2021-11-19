//#include <FastLED.h>
#include <Adafruit_NeoPixel.h>

#include <RedGlobals.h>



/*
 * ********************************************************************************

 * ********************************************************************************
*/

#define LED_UPDATE_INTERVAL 30000   // LED update interval

bool  ledStripON;     // Led is on or off
int   ledMode;        // mode of display 1 - 2700K, 2 - rainbow,
//CRGB leds[NUM_LEDS];
long lastLEDUpdate;   // hold last time update was sent to LED

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(NUM_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
uint32_t rgbList[] = { strip.Color(255, 0, 0), strip.Color(0, 255, 0), strip.Color(0, 0, 255)};

void initializeLED()
{
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  delay(500);
}

void configLED()
{

  strip.updateLength(atoi(numberOfLED));
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(LED_BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)

  ledStripON = false;
  ledMode = 1;
  executeLED();

  console.println(String(atoi(numberOfLED)) + " LEDS configured");
}

// show a green pattern on the LED strip
void testLED()
{
  strip.clear();

  for (int i = 0; i < atoi(numberOfLED); i++)
  {

    strip.setPixelColor(i, strip.Color(0, 150, 0));
    strip.show();
    console.printf("LED %d set to green\r\n", i);
    delay(50);
  }

}


/*
 * ********************************************************************************

   Service an LED command.

 * ********************************************************************************
*/

void setLEDPower(char* mode)
{
  //  console.print("setLEDPower=");
  if ( ((atof(mode) > 0) && (atof(mode) <= 1) ) || (strcmp(mode, "1") == 0)
      || (strcmp(mode, "ON") == 0) ) {
    ledStripON = true;
    console.println("LEDs turned on");
  }
  else
  {
    ledStripON = false;
    console.println("LEDs turned off");
  }

  executeLED();

}

void setLEDMode(int mode)
{
  ledMode = mode;
  executeLED();

}

void executeLED()
{

  if (ledStripON) // if ON determine what to display
  {
    switch (ledMode)
    {
      case 1: // 100W tungsten -- Pam's favorite
        stripFill(strip.gamma32(strip.Color(255, 214, 170)));
        //strip.show();
        console.println("ON");
        break;
      case 2: // dimmed 50W tungsten
        stripFill(strip.gamma32(strip.Color(255 / 2, 202 / 2, 148 / 2)));
        //strip.show();
        console.println("DIMMED");
        break;
      case 3: // navy blue
        stripFill(strip.gamma32(strip.Color(0, 0, 128)));
        //strip.show();
        console.println("BLUE");
        break;
      case 4: // Xmas
        fillList(rgbList, 2);
        //strip.show();
        console.println("XMAS");
        break;
      case 5: // Rainbow
        fillRainbow();
        //strip.show();
        console.println("RAINBOW");
        break;
      default: // full white
        stripFill(strip.gamma32(strip.Color(255, 255, 255)));
        //strip.show();
        console.println("Default");
    }
  }
  else
  {
    stripFill(strip.gamma32(strip.Color(0, 0, 0)));
    //strip.clear();
    strip.show();
    console.println("OFF");
  }

  console.println("LEDs updated");

}

// fill entire strip with a single color 
void stripFill(uint32_t color)
{

  for (int i = 0; i < atoi(numberOfLED); i++)
  {
    strip.setPixelColor(i, color);
    strip.show();
    delay(50);
  }
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {

  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    delay(wait);//  Pause for a moment
    strip.show();                          //  Update strip to match

  }

}

// fill the strip with a sequence of RGB color
void fillRainbow()
{

  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    if ( (i % 3) == 0) strip.setPixelColor(i, strip.Color(255, 0, 0)); //  Red
    if ( (i % 3) == 1) strip.setPixelColor(i, strip.Color(0, 255, 0)); //  Green
    if ( (i % 3) == 2) strip.setPixelColor(i, strip.Color(0, 0, 255)); //  Blue
    strip.show();                          //  Update strip to match
    delay(50);
  }

}

void fillList(uint32_t list[], int count)
{

  int listCount = 0;
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, list[listCount++]);
    if (listCount >= count) listCount = 0;
    strip.show();                          //  Update strip to match
    delay(50);
  }

}
