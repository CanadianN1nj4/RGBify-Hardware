#include <BluetoothSerial.h>
BluetoothSerial Bluetooth;

char input; // to store input character received via BT.
String BTData;
const int relay = 16;

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
