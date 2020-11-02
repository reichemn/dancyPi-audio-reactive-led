// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 150

#define DEBUG false
#define MIRROR_FIRST_HALF false

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip1(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, 7, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


// setup() function -- runs once at startup --------------------------------

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip1.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip1.show();            // Turn OFF all pixels ASAP
  strip1.setBrightness(255); // Set BRIGHTNESS to max level
  Serial.begin(230400);
}


// loop() function -- runs repeatedly as long as board is on ---------------
bool wait_for_bytes(byte *buf, int n) {
  int i = 0;
  while (i < n) {
    if (Serial.available()) {
      byte b = Serial.read();
      if (b == 0xff) {
        // sequence start found
        return true;
      }
      buf[i] = b;
      i++;
    }
  }
  return false;
}

void process_sequence() {
  byte numberHL[2];
  if (wait_for_bytes(numberHL, 2)) return;
  unsigned int numberOfPixels = numberHL[1] | ((unsigned int)(numberHL[0]) << 8);
  if (DEBUG) {
    Serial.print("Pixel num: ");
    Serial.println(numberOfPixels);
  }
  for (int i = 0; i < numberOfPixels && i < strip1.numPixels(); i++) {
    byte rgb[3];
    if (wait_for_bytes(rgb, 3)) return;
    strip1.setPixelColor(i, rgb[0], rgb[1], rgb[2]);
    if (MIRROR_FIRST_HALF) {
      strip1.setPixelColor(LED_COUNT - 1 - i, rgb[0], rgb[1], rgb[2]);
    }

    if (DEBUG) {
      Serial.print("Pixel ");
      Serial.print(i);
      Serial.print(" \t r:");
      Serial.print(rgb[0]);
      Serial.print(" \t g:");
      Serial.print(rgb[1]);
      Serial.print(" \t b:");
      Serial.println(rgb[2]);
    }
  }
  strip1.show();
}

void loop() {
  if (Serial.available()) {
    byte lastByte = Serial.read();
    // read unit we found the start of the sequence marked with 0xff byte
    while (lastByte != 255) {
      if (Serial.available()) {
        lastByte = Serial.read();
        if (DEBUG) Serial.print('.');
      }
    }
    process_sequence();
  }
}


