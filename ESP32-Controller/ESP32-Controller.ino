#include <BluetoothSerial.h>
BluetoothSerial Bluetooth;

char input; // to store input character received via BT.
String data;

void setup() {
  //Serial.begin(9600);
  Bluetooth.begin("ESP32"); //Bluetooth device name
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
    printString(s);
  }
  else if(s == "LightsOFF"){
    //Turn Lights off
    printString(s);
  }
  else{
    Bluetooth.println("invalid command");
  }
  data = "";
  Bluetooth.flush();

}

void loop() { 

  if (Bluetooth.available())
  {
    input=(Bluetooth.read());

      if (input != '!') {
        data += input;
      }
      else {
        handleCommand(data);        
      }
  }
}
