#include <LinkedList.h>
#include <BluetoothSerial.h>
#include <Adafruit_NeoPixel.h>
#define MAX_STRIPS 3

//Bluetooth
BluetoothSerial Bluetooth;
char input; // to store input character received via BT.
String BTData;
const int relay = 16;

//RGB
Adafruit_NeoPixel Strips[MAX_STRIPS];

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
    LinkedList<String> variables;
    parseInput(s, variables);
    //grab variables from input
//    if(
//    (!isDigit(c) || c == '0' || c_int > MAX_STRIPS) ? return : stripNum = c_int;
    
    //Add a strip
    //addStrip(60, 17, 1);
  }
  else if(s.charAt(0) == '-'){
    //Remove a strip
    removeStrip(1);
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
      }
  }
}

void addStrip(int numLEDs, int LEDPin, int stripNum){
  Strips[stripNum-1] = Adafruit_NeoPixel(numLEDs, LEDPin, NEO_GRB + NEO_KHZ800);
}

void removeStrip(int stripNum){
  Strips[stripNum-1].clear();
}

//void initializeStrips(){
//  for(int i = 0; i < MAX_STRIPS; i++){
//    Strips[i] = Adafruit_NeoPixel(0,4,NEO_GRB + NEO_KHZ800)
//  }
//}

void parseInput(String s, LinkedList<String> &variables){
  int sizeOfString = (sizeof(s) / sizeof(s[0]));
  char str[sizeOfString];
  s.toCharArray(str, sizeOfString);
  char * tokens[50];
  size_t n = 0;

  for (char * p = strtok(str, "+,!"); p; p = strtok(NULL, "/,!"))
  {
//      if (n >= 50)
//      {
//          // maximum number of storable tokens exceeded
//          break;
//      }
      tokens[n++] = p;
  }
  
  for (size_t i = 0; i != n; ++i) {
    variables.add(String(tokens[i]));
  }
}
