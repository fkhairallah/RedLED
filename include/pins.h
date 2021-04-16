/*
 * ********************************************************************************
 * 
 * This program runs undercabinet LED lights AND temperature sensor from a single
 * WEMOS D1 esp8266
 * 
 * 
 * 
 * Hardware configuration:
 * 
 *   - Blue LED connected to pin 2
 *   - 1Wire thermocouple connected to pin 3
 *   - WS2812B individually addressable LED pin 4


    Hardware Notes:

    - GPIO-0 must be tied to ground for programming
    - GPIO-0 floats to run program
    - GPIO-0 runs Red LED on Huzzah
    - GPIO-2 is tied to Blue Led (*NOT* a PWM pin)
    - GPIO-13 is RESERVED

 * ********************************************************************************
 */
#ifndef _PINS_H
#define _PINS_H

// hardware pin definitions
#define pgm_pin 0
#define blueLED 2      // blue LED light (D1 Mini D4)
#define LED_DATA_PIN 4 // LED data pin (D1 Mini D2)

//#define ONE_WIRE_BUS 3 // 1Wire Data in
#define ONE_WIRE_BUS 14

#define NUM_LEDS 30        // Max # of LEDS
#define LED_BRIGHTNESS 250 // NeoPixel brightness, 0 (min) to 255 (max)

#endif