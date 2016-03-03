#include "FastLED.h"

FASTLED_USING_NAMESPACE

// Modified from the Mark Kriegsman, December 2014 DemoReel100
// By Bob Eells, 2015 - for the BlinkyCube - a 3d printed, lighted, geared cube.  
// TODO: Bluetooth/cellphone app, accelerometer input, graphic eq

#if FASTLED_VERSION < 3001000
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    21
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          64
#define FRAMES_PER_SECOND   120


const int buttonPin = 2;    // the number of the pushbutton pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  delay(3000); // 3 second delay for recovery

  pinMode(buttonPin, INPUT);

  Serial.begin(9600);  
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, green_rand, blue_rand, earth_rand, water_rand, fire_rand, bw_rand, air_rand, inside_outside, ins_out_fadedown, center_rainbow, corners_hsvcrossfade, halves, rainbow, chase, randy, corners, corners_hsvfade, solids, alternate };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int led_center = 0;
int small_corners[] = {1,5,6,7};
int large_corners[] = {2,3,4,8};
int left_side[] = {1,2,3,7};
int right_side[] = {4,5,6,8};
int top[] = {1,3,4,6};
int bottom[] = {2,5,7,8};
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  //FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 60 ) { nextPattern(); } // change patterns periodically

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        nextPattern();
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;  
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  fill_solid(leds,NUM_LEDS,CRGB::Black);

  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void randy()
{
  CRGB r;
  leds[random(0,NUM_LEDS)] = r.setRGB(random(0,256),random(0,256),random(0,256));
  FastLED.show();
  FastLED.delay(random(0,255));
}

void chase()
{
  //use c1 and c2 to hold our background and foreground colors, respectively.
  CRGB c1, c2;
  //wait is how long each dot takes.
  int wait;
  
  //half the time, let's get some random colors in there.
  if(random(0,2))
  {
    c1.setRGB(random(0,256),random(0,256),random(0,256));
    //invert the random color for a good contrasting color
    c2 = -c1;
    wait = random(50,500);
  }else{
    //hardcoded values to run half the time, because they look good together.
    c1 = CRGB::Red;
    c2 = CRGB::Blue;
    wait = 100;
  }

  //set all to c1.
  fill_solid(leds,NUM_LEDS,c1);
  
  for(int i=0;i<NUM_LEDS;i++)
  {
    leds[i] = c2;
    FastLED.show();
    FastLED.delay(wait);
  }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void center_rainbow()
{
  fill_rainbow( leds, 1, gHue, 7 );
}

void inside_outside()
{
  //use c1 and c2 to hold our background and foreground colors, respectively.
  CRGB c1, c2;
  
  c1 = CRGB::Red;
  c2 = CRGB::Blue;
    
  leds[led_center] = c1;

  for(int i = 0;i<4;i++)
  {
    leds[small_corners[i]] = c2;
    leds[large_corners[i]] = c2;
  }
  FastLED.show();
}

void ins_out_fadedown()
{
  CRGB c1,c2;
  leds[led_center] = c1;

  for(int i = 0;i<4;i++)
  {
    leds[small_corners[i]] = c2;
    leds[large_corners[i]] = c2;
  }
  FastLED.show();  

  for(int j=0;j<255;j++)
  {
    c1 = CHSV(j,255,j);
    c2 = CHSV(256-j,255,255);

    leds[led_center] = c1;
  
    for(int i = 0;i<4;i++)
    {
      leds[small_corners[i]] = c2;
      leds[large_corners[i]] = c2;
    }
    FastLED.show();
    FastLED.delay(100);
  }
  
}

void corners()
{
  //use c1 and c2 to hold our background and foreground colors, respectively.
  CRGB c1, c2;
  // wait is how long each swap takes.
  int  wait;
  
  //half the time, let's get some random colors in there.
  if(random(0,2))
  {
    c1.setRGB(random(0,256),random(0,256),random(0,256));
    //invert the random color for a good contrasting color
    c2 = -c1;
    wait = random(500,1000);
  }else{
    //hardcoded values to run half the time, because they look good together.
    c1 = CRGB::Red;
    c2 = CRGB::Blue;
    wait = 100;
  }
    
  leds[led_center] = c1;

  for(int i = 0;i<4;i++)
  {
    leds[small_corners[i]] = c1;
    leds[large_corners[i]] = c2;
  }
  FastLED.show();
  FastLED.delay(wait); 

}

void corners_hsvfade()
{
  //use h1 and h2 to hold our hues  
  byte h1, h2;
  int delta, wait;
  h1 = 160;//blue
  h2 = 38; //orange
  
  wait = 10;
  delta = 1;

  int loops;
  loops = 5000;

  for(int j=0;j<loops;j++)
  {
    leds[led_center] = CHSV(h1,255,255);
  
    for(int i = 0;i<4;i++)
    {
      leds[small_corners[i]] = CHSV(h1,255,255);;
      leds[large_corners[i]] = CHSV(h2,255,255);;
    }

    h1 = h1+delta;
    h2 = h2+delta;

    FastLED.show();
    FastLED.delay(wait); 
  }
}


void corners_hsvcrossfade()
{
  //use h1 and h2 to hold our hues  
  byte h1, h2;
  int delta, wait;
  h1 = 160;//blue
  h2 = 38; //orange
  
  wait = 30;
  delta = 1;

  int loops;
  loops = 5000;

  for(int j=0;j<loops;j++)
  {
    leds[led_center] = CHSV(h1,255,255);
  
    for(int i = 0;i<4;i++)
    {
      leds[small_corners[i]] = CHSV(h1,255,255);
      leds[large_corners[i]] = CHSV(h2,255,255);
    }

    h1 = h1+delta;
    h2 = h2-delta;

    FastLED.show();
    FastLED.delay(wait); 
  }
}

void halves()
{
  //use c1 and c2 to hold our background and foreground colors, respectively.
  CRGB c1, c2;
  
  //half the time, let's get some random colors in there.
  if(random(0,2))
  {
    c1.setRGB(random(0,256),random(0,256),random(0,256));
    //invert the random color for a good contrasting color
    c2 = -c1;
  }else{
    //hardcoded values to run half the time, because they look good together.
    c1 = CRGB::Black;
    c2 = CRGB::Green;
  }
    
  leds[led_center] = c1;
  for(int i = 0;i<4;i++)
  {
    leds[left_side[i]] = c2;
    leds[right_side[i]] = c1;
  }
  FastLED.show();
  FastLED.delay(1000); 
  
  for(int i = 0;i<4;i++)
  {
    leds[top[i]] = c2;
    leds[bottom[i]] = c1;
  }
  FastLED.show();
  FastLED.delay(1000); 

  for(int i = 0;i<4;i++)
  {
    leds[left_side[i]] = c1;
    leds[right_side[i]] = c2;
  }
  FastLED.show();
  FastLED.delay(1000); 
  
  for(int i = 0;i<4;i++)
  {
    leds[top[i]] = c1;
    leds[bottom[i]] = c2;
  }
  FastLED.show();
  FastLED.delay(1000);   
    
}

void green_rand()
{
  CRGB colors[24];
  for(int i=0;i<23;i++)
  {
    colors[i].setRGB(0,i*8,0); 
  }
  palette_rand(colors, 24, 200);
}

void red_rand()
{
  CRGB colors[24];
  for(int i=0;i<23;i++)
  {
    colors[i].setRGB(i*8,0,0); 
  }
  palette_rand(colors, 24, 200);
}

void blue_rand()
{
  CRGB colors[24];
  for(int i=0;i<23;i++)
  {
    colors[i].setRGB(0,0,i*8); 
  }
  palette_rand(colors, 24, 200);
}

void fire_rand()
{
  CRGB colors[] = {CRGB::Tomato, CRGB::Red, CRGB::OrangeRed, CRGB::Brown, CRGB::FireBrick, CRGB::Maroon};
  palette_rand(colors, 6, 200);
}

void water_rand()
{
  CRGB colors[] = {CRGB::Blue, CRGB::DarkBlue, CRGB::DarkTurquoise, CRGB::Aqua, CRGB::SeaGreen, CRGB::Aquamarine};
  palette_rand(colors, 6, 200);
}

void earth_rand()
{
  CRGB colors[] = {CRGB::Chartreuse, CRGB::Green, CRGB::DarkOliveGreen, CRGB::DarkGreen, CRGB::LimeGreen, CRGB::ForestGreen};
  palette_rand(colors, 6, 200);
}

void air_rand()
{
  CRGB colors[] = {CRGB::Azure, CRGB::Cyan, CRGB::DeepSkyBlue, CRGB::LightSkyBlue, CRGB::DodgerBlue, CRGB::Turquoise};
  palette_rand(colors, 6, 200);
}

void bw_rand()
{
  CRGB colors[] = {CRGB::Black, CRGB::White};
  palette_rand(colors, 2, 200);
}

void palette_rand(CRGB colors[], int colorCount,int wait)
{
    leds[random(0,NUM_LEDS)] = colors[random(0,colorCount)];
    FastLED.show();
    FastLED.delay(wait); 

}

void alternate()
{
  //use c1 and c2 to hold our background and foreground colors, respectively.
  CRGB c1, c2;
  
  //half the time, let's get some random colors in there.
  if(random(0,2))
  {
    c1.setRGB(random(0,256),random(0,256),random(0,256));
    //invert the random color for a good contrasting color
    c2 = -c1;
  }else{
    //hardcoded values to run half the time, because they look good together.
    c1 = CRGB::Red;
    c2 = CRGB::Blue;
  }
  for(int j = 0;j<20;j++)
  {
    leds[led_center] = CRGB::Black;
    for(int i = 0;i<4;i++)
    {
      if(j%2)
      {
        leds[left_side[i]] = c1;
        leds[right_side[i]] = c2;
      }else{
        leds[left_side[i]] = c2;
        leds[right_side[i]] = c1;
      }
    }
    FastLED.show();
    FastLED.delay(1000); 
  }
}


void solids()
{
  fill_solid(leds,NUM_LEDS,CRGB::Red);
  FastLED.show();
  FastLED.delay(3000);  
  fill_solid(leds,NUM_LEDS,CRGB::Green);
  FastLED.show();
  FastLED.delay(3000);  
  fill_solid(leds,NUM_LEDS,CRGB::Blue);
  FastLED.show();
  FastLED.delay(3000);  
  fill_solid(leds,NUM_LEDS,CRGB::White);
  FastLED.show();
  FastLED.delay(10000);  
  fill_solid(leds,NUM_LEDS,CRGB::Black);
  FastLED.show();
  FastLED.delay(1000);  
  fill_solid(leds,NUM_LEDS,CRGB::White);
  FastLED.show();
  FastLED.delay(10000);  
  fill_solid(leds,NUM_LEDS,CRGB::Black);
  FastLED.show();
  FastLED.delay(1000);  
}  


