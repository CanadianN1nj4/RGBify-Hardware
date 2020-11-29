#include <Adafruit_NeoPixel.h>
#define LED_PIN    6
#define LED_COUNT 60
#define MAX_STRIPS 3

int numStrips;
Adafruit_NeoPixel Strips[3];

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

void addStrip(int numLEDs, int LEDPin, int Type) {
  if(numStrips == MAX_STRIPS) return;
  uint16_t t;
  switch(Type){
    case 1: 
      t = NEO_GRB + NEO_KHZ800;
      break;
    default:
      t = NEO_GRB + NEO_KHZ800;
    break;
  }
  
  Strips[numStrips] = Adafruit_NeoPixel(numLEDs, LEDPin, t);
}

void removeStrip(int stripNum){
  if(stripNum == MAX_STRIPS){
    Strips[stripNum-1] = NULL;
  }else{
    Strips[stripNum-1] = Strips[stripNum];
    Strips[stripNum] = NULL;
  }
}

void changeLength(int numLeds, int stripNum){
  Strips[stripNum-1].updateLength(numLeds);
}

//Adafruit_NeoPixel strip_a = Adafruit_NeoPixel(16, 5);
//Adafruit_NeoPixel strip_b = Adafruit_NeoPixel(16, 6);
