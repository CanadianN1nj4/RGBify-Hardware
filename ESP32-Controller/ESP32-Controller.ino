#include <LinkedList.h>
#include <BluetoothSerial.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#define MAX_STRIPS 3
#define MAX_OPTIONS 3
#define NUM_ANIMATIONS 3

struct RGBColour {
  int R;
  int G;
  int B;
};

//Bluetooth
BluetoothSerial Bluetooth;
char input; // to store input character received via BT.
String BTData;
const int relay = 16;

//RGB
int StripPins[] = {25,26,27};
Adafruit_NeoPixel Strips[MAX_STRIPS] = {
  Adafruit_NeoPixel(60, StripPins[0], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(60, StripPins[1], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(60, StripPins[2], NEO_GRB + NEO_KHZ800),
};

//Animation variables
int lastAnimation = 0;
int animation = 0;
int currentStep = 0;
int lastAnimatedStrip = 0;
int currentAnimatedStrip = 0;
int animationSpeed = 200;
int cycle = 0;
bool forward = true;
RGBColour animationColour;
Adafruit_NeoPixel animatedStrip = Strips[currentAnimatedStrip];

void setup() {
  Serial.begin(9600);
  Bluetooth.begin("RGBify"); //Bluetooth device name
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
}


void printString(String s) {
  for(char& c : s) {
    Bluetooth.print(c);
  }
  Bluetooth.println("");
}

void handleCommand(String s) {
  // make lights on and off programatic
  // use '.' for the input
  // second input is what string number
  // have a boolean array with max number of strips
  // not the boolean and then give that input for the digital write
  if(s.indexOf("Wifi:")==0){
      printString("Wifi");
      WiFi.begin("", "");
      while(WiFi.status() != WL_CONNECTED) {
        delay(100);
        printString("connecting");

      }
      printString("connected");
  }
  if(s == "LightsON"){
    // Turn Lights on
    digitalWrite(relay, LOW);
    Serial.println("on");
    printString("Lights On");
  }
  else if(s == "LightsOFF"){
    //Turn Lights off
    digitalWrite(relay, HIGH);
    Serial.println("off");
    printString("Lights Off");
  }
  else if(s.charAt(0) == '+'){
    addStrip(s);
  }
  else if(s.charAt(0) == '-'){
    removeStrip(s);
  }
  else if(s.charAt(0) == '*'){
    changeStrip(s);
  }
  else if(s.charAt(0) == '.'){
    
  }
  else{
    Bluetooth.println("invalid command");
  }
  BTData = "";
  Bluetooth.flush();

}

void loop() {

  if (Bluetooth.available())
  {
    input=(Bluetooth.read());

      if (input != '!') {
        BTData += input;
      }
      else {
        handleCommand(BTData);
        BTData = "";
      }
  }

  if(animation != 0){
    if(animationUpdateCheck()){
      animatedStrip = Strips[currentAnimatedStrip-1];
    }
    runAnimations(animatedStrip);
  }
  
}

void addStrip(String s){
  LinkedList<String> variables;
  parseInput(s, variables);
  /*
   * variables:
   *  RGB strip number*
   *  Numer of LEDs*
   *    TODO -> Optional for non-addressable
   *  Strip Type
   *    Addressable or non-addressable
   *    Default to addressable
   *  Voltage
   *    5v or 12v
   *    Default to 5v
   */
  if(variables.size() >= 2){
    int stripNum = variables.get(0).toInt();
    int numLEDs = variables.get(1).toInt();
    //checks if number for the strip is possilbe and there are LEDs added
    if(!checkStripNum(stripNum) || numLEDs <= 0) return;
    //Add Addressable strip, only option for now
    add_ARGB_Strip(numLEDs, stripNum);
  }
}

void add_ARGB_Strip(int numLEDs, int stripNum){
  int indexedStripNumber = stripNum-1;
  if(Strips[indexedStripNumber].numPixels() == numLEDs) return;
  Strips[indexedStripNumber].updateLength(numLEDs);
}

void removeStrip(String s){
  String stripNum = s.substring(1,2);
  int stripNum_int;
  stripNum_int = stripNum.toInt();
  if(!checkStripNum(stripNum_int)) return;
  //Remove a strip
  remove_ARGB_Strip(stripNum_int);
}

void remove_ARGB_Strip(int stripNum){
  if(stripNum == currentAnimatedStrip){
    animation = 0;
    currentAnimatedStrip = 0;
    lastAnimatedStrip = 0;
    lastAnimation = 0;
    animatedStrip.clear();
    animatedStrip.show();
  }else{
    Adafruit_NeoPixel strip = Strips[stripNum-1];
    strip.clear();
    strip.show();
  }
}

void changeStrip(String s){
  LinkedList<String> variables;
  parseInput(s, variables);
  /*
   * variables:
   *  which change option*:
   *    1 -> whole strip
   *    2 -> animation
   *    3 -> certain leds
   *  Strip number*
   *  All the other specific options, minimum of 3
   */
  if(variables.size() >= 3){
    //shift removes from the front
    int optionNum = variables.shift().toInt();
    int stripNum = variables.shift().toInt();
    if(!checkOptionNum(optionNum) || !checkStripNum(stripNum)) return;
//    if(stripNum == currentAnimatedStrip){
//      animation = 0;
//      currentAnimatedStrip = 0;
//      lastAnimatedStrip = 0;
//      lastAnimation = 0;
//    }
    switch(optionNum){
      case 1:
        wholeStrip(variables, stripNum);
        break;
      case 2:
        animationSetup(variables, stripNum);
        break;
      case 3:
        addressableLEDs(variables, stripNum);
        break;
      default:
        return;
    }
  }
}

void wholeStrip(LinkedList<String> &variables, int stripNum){
  //returns if there isn't enough variables for the colour
  if(variables.size() < 3) return;
  
  //sets the colurs
  int red   = checkColour(variables.shift().toInt());
  int green = checkColour(variables.shift().toInt());
  int blue  = checkColour(variables.shift().toInt());
  
  //gets the colour in the form the strip can read
  uint32_t colour = Strips[stripNum-1].Color(red,green,blue);
  
  //sets the colour on the strip
  Strips[stripNum-1].fill(colour);
  Strips[stripNum-1].show();
}

void animationSetup(LinkedList<String> &variables, int stripNum){

  //if it doesn't have enough variables
  if(variables.size() < 1) return;

  //gets the number of the animation
  int animationNum = variables.shift().toInt();

  //if it's not a valid animation leave
  if(!checkAnimationNum(animationNum)){
    return;
  }
  else if(animationNum == 2){
    if(variables.size() < 3) return;
    animationColour.R = checkColour(variables.shift().toInt());
    animationColour.G = checkColour(variables.shift().toInt());
    animationColour.B = checkColour(variables.shift().toInt());
  }
  else if(animationNum == 3){
//    if(stripNum == currentAnimatedStrip){
//      animatedStrip.clear();
//      animatedStrip.show();
//    }else{
//      Adafruit_NeoPixel strip = Strips[stripNum-1];
//      strip.clear();
//      strip.show();
//    }
  }

  //order is important
  currentAnimatedStrip = stripNum;
  animation = animationNum;
  currentStep = 0;
}

void addressableLEDs(LinkedList<String> &variables, int stripNum){
  return; 
}

int checkColour(int c){
  if(c < 0){
    return 0;
  }else if(c > 255){
    return 255;
  }else{
    return c;
  }
}

bool checkStripNum(int stripNum){
  if(stripNum <= 0 || stripNum > MAX_STRIPS){
    return false;
  }else{
    return true;
  }
}

bool checkOptionNum(int optionNum){
  if(optionNum <=0 || optionNum > MAX_OPTIONS){
    return false;
  }else{
    return true;
  }
}

bool checkAnimationNum(int animationNum){
  if(animationNum <= 0 || animationNum > NUM_ANIMATIONS){
    return false;
  }else{
    return true;
  }
}

void parseInput(String s, LinkedList<String> &variables){
  int sizeOfString = s.length() + 1;
  char str[sizeOfString];
  s.toCharArray(str, sizeOfString);
  char * tokens[50];
  size_t n = 0;
  char * vout;

  for (char * p = strtok(str, "*+,!/"); p; p = strtok(NULL, "*+,!/"))
  {
//      if (n >= 50)
//      {
//          // maximum number of storable tokens exceeded
//          break;
//      }
      //adds the token to the list
      tokens[n++] = p;
  }

  //adds to the variable linked list
  for (size_t i = 0; i != n; ++i) {
    variables.add(String(tokens[i]));
  }
}

void runAnimations(Adafruit_NeoPixel &strip){
  //sets the animation speed
  if(cycle % animationSpeed == 0){
    switch(animation){
      case 0:
        return;
      case 1:
        // everything is one colour and everything transitions together
        rainbow(strip);
        break;
      case 2:
        //Has a set colour that it dims and brightens
        breathing(strip);
        break;
      case 3:
        // random leds turn a random colour
        rain(strip);
        break;
      default:
        return;
    }
  }
//  Serial.println("exited if statement");
  cycle++;
//  Serial.println("end of function");
}

void rainbow(Adafruit_NeoPixel &strip){

  //uses gamma correction and gets the right colour
  uint32_t rgbcolor = strip.gamma32(strip.ColorHSV(currentStep));

  //fills the strip with the colour and shows
  strip.fill(rgbcolor);
  strip.show();

  //check if the if the current step is the max
  if(currentStep == 65536){
    currentStep = 0;
  }else{
    currentStep += 256;
  }
}

void breathing(Adafruit_NeoPixel &strip){
  if(currentStep == 255 || currentStep == 0){
    forward = !forward;
    Serial.println(String(forward));
  }
  Serial.println(String(currentStep));

  //gets the weighted colours
  int rAdjusted = brightnessWeighted(animationColour.R, currentStep);
  int gAdjusted = brightnessWeighted(animationColour.G, currentStep);
  int bAdjusted = brightnessWeighted(animationColour.B, currentStep);

  //gets the weighted strip colour
  uint32_t colour = strip.Color(rAdjusted, gAdjusted, bAdjusted);

  //fills the strip with the colour and shows
  strip.fill(colour);
  strip.show();

  //checks to see what to do next
  if(forward){
    currentStep++;
  }else{
    currentStep--;
  }
}

void rain(Adafruit_NeoPixel &strip){
  //sets up the strip for later use

  //gets a random pixel
  int randomPixel = random(strip.numPixels());

  //gets a random number to set the pixel to
  int randomHue = random(65536);

  strip.setPixelColor(randomPixel, strip.gamma32(strip.ColorHSV(randomHue)));
  strip.show();
}

//TODO: Add colourWipe

int brightnessWeighted(int c, int brightness){
  return (brightness*c/255);
}

bool animationUpdateCheck(){
  //checks to see if animation changed
  if(lastAnimation == animation && lastAnimatedStrip == currentAnimatedStrip){
    //animation didn't change
    return false;
  }else{
    //animation did change
    return true;
  }
}
