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
#define LED_COUNT 111

uint32_t current_palette[LED_COUNT];

/* Global variables */
typedef enum {
  off_t,
  red_t,
  blue_t,
  pink_t,
  rainbow_t,
  twinkle_t,
  sparkle_t,
  huebow_t,
  palette_loop_t
} loop_type;

loop_type current_loop = palette_loop_t;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


// setup() function -- runs once at startup --------------------------------
void create_hue_palette(long huemin, long huemax)
{
  /* Distributed over all pixels */
  long increment = (huemax - huemin) / (strip.numPixels()/2);

  for (int i = 0; i < strip.numPixels()/2; i++)
  {
    int pixelHue = huemin + (i * increment);
    current_palette[i] = strip.gamma32(strip.ColorHSV(pixelHue));
  }

  for (int i = strip.numPixels()/2; i < strip.numPixels(); i++)
  {
    int pixelHue = huemax - ((i-(strip.numPixels()/2)) * increment);
    current_palette[i] = strip.gamma32(strip.ColorHSV(pixelHue));
  }
}

void create_rgb_palette()
{
  long full = strip.numPixels();
  long half = strip.numPixels()/2;

  for (int i = 0; i < half; i++)
  {
    current_palette[i] = strip.Color(100+i, 0, 0);
  }

  for (int i = half; i < full; i++)
  {
    int zero_i = i - half;
    current_palette[i] = strip.Color(100 + half - zero_i, 0, 0);
  }
}

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pinMode(8, OUTPUT);
  // END of Trinket-specific code.
  Serial.begin(9600);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(10); // Set BRIGHTNESS to about 1/5 (max = 255)
  create_hue_palette(4500, 6000);
}

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < LED_COUNT; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
 
  for (int i=0; i<Count; i++) {
     setPixel(random(LED_COUNT),random(0,255),random(0,255),random(0,255));
     showStrip();
     delay(SpeedDelay);
     if(OnlyOne) {
       setAll(0,0,0);
     }
   }
 
  delay(SpeedDelay);
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(LED_COUNT);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void Huebow(int wait) {
  long huemin = 35000L;
  long huemax = 50000L;
  /* Distributed over all pixels */
  long increment = (huemax - huemin) / strip.numPixels();

  for (int i = 0; i < strip.numPixels(); i++)
  {
    long offset = i*increment;
    for (int j = 0; j < strip.numPixels(); j++)
    {
      int pixelHue = huemin + offset + j*increment;
      while (pixelHue > huemax){
        pixelHue = pixelHue - huemax + huemin;
      }
      strip.setPixelColor(j, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(10);  // Pause for a moment
  }
}

void run_palette_loop(int wait) {
  for (int i = 0; i < strip.numPixels(); i++)
  {
    for (int j = 0; j < strip.numPixels(); j++)
    {
      int offset = (i + j) % strip.numPixels();
      strip.setPixelColor(j, current_palette[offset]);
    }
    strip.show(); // Update strip with new contents

    /* Check if we have new serial data if so leave */
    if (Serial.available() > 0 )
      break;

    delay(wait);  // Pause for a moment
  }
}

void loop_wrap() {
  if (current_loop == red_t)
  {
    colorWipe(strip.Color(255,   50,   0), 50);
  }
  else if (current_loop == blue_t)
  {
    colorWipe(strip.Color(50,   50,   255), 50);
  }
  else if (current_loop == pink_t)
  {
    colorWipe(strip.Color(255,   50,   255), 50);
  }
  else if (current_loop == rainbow_t)
  {
    rainbow(10);
  }
  else if (current_loop == twinkle_t)
  {
    TwinkleRandom(20, 100, false);
  }
  else if (current_loop == sparkle_t)
  {
    Sparkle(0xff, 0xff, 0xff, 0);
  }
  else if (current_loop == huebow_t)
  {
    Huebow(10);
  }
  else if (current_loop == palette_loop_t)
  {
    run_palette_loop(100);
  }
}

// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {

  while(Serial.available() > 0 ){
    String str = Serial.readString();
    if(str.indexOf("blue") > -1){
      current_loop = blue_t;
    } 
    else if(str.indexOf("pink") > -1){
      current_loop = pink_t;
    } 
    else if(str.indexOf("red") > -1){
      current_loop = red_t;
    } 
    else if(str.indexOf("bright") > -1){
      strip.setBrightness(180);
      strip.show();
    } 
    else if(str.indexOf("dim") > -1){
      strip.setBrightness(10);
      strip.show();
    } 
    else if(str.indexOf("clear") > -1){
      strip.clear();
      strip.show();
      current_loop = off_t;
    } 
    else if(str.indexOf("power") > -1){
      digitalWrite(8, HIGH);
      delay(200);
      digitalWrite(8, LOW);
    } 
    else if(str.indexOf("off") > -1){
      digitalWrite(8, HIGH);
      delay(200);
      digitalWrite(8, LOW);
      strip.clear();
      strip.show();
      current_loop = off_t;
    } 
    else if(str.indexOf("on") > -1){
      digitalWrite(8, HIGH);
      delay(200);
      digitalWrite(8, LOW);
      current_loop = palette_loop_t;
    } 
    else if(str.indexOf("rainbow") > -1){
      current_loop = rainbow_t;
    } 
    else if(str.indexOf("twinkle") > -1){
      current_loop = twinkle_t;
    } 
    else if(str.indexOf("sparkle") > -1){
      current_loop = sparkle_t;
    } 
    else if(str.indexOf("huebow") > -1){
      current_loop = huebow_t;
    } 
    else if(str.indexOf("hue_palette") > -1){
      current_loop = palette_loop_t;
    } 
    else if(str.indexOf("hue_red") > -1){
      create_hue_palette(0, 5000);
    } 
    else if(str.indexOf("hue_blue") > -1){
      create_hue_palette(40000, 50000);
    } 
  }

  loop_wrap();

  //colorWipe(strip.Color(50,   50,   255), 50); // Blueis

  //rainbow(10);             // Flowing rainbow cycle along the whole strip


  /*
  // Fill along the length of the strip in various colors...
  colorWipe(strip.Color(255,   0,   0), 50); // Red
  colorWipe(strip.Color(  0, 255,   0), 50); // Green
  colorWipe(strip.Color(  0,   0, 255), 50); // Blue

  // Do a theater marquee effect in various colors...
  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
  theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness

  rainbow(10);             // Flowing rainbow cycle along the whole strip
  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
  */

}


