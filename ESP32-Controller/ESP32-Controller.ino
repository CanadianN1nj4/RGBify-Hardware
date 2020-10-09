#include "WiFi.h"
#include "aREST.h"

// ---- Variables for Rest API Start ----
aREST rest = aREST(); 
WiFiServer server(80); 

const char* ssid = "";   // Put in your Wifi Network Name
const char* password =  "";  // Put in your WiFi Password

String name = "RGBify";
// ---- Variables for Rest API END ----


void restApiSetup() {
  rest.function("setLedState", setLedState);
  rest.function("toggleLedState", toggleLedState);
  
  //For Demo
  rest.variable("name",&name);
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
 
  server.begin();
  
}

void setup() {
  Serial.begin(115200);
  restApiSetup();
}

void restApiLoop() {
  WiFiClient client = server.available();
  if (client) {
 
    while(!client.available()){
      delay(5);
    }
    rest.handle(client);
  }
}
 
void loop() {
  restApiLoop();

}

// RestAPI Recieves String containing int ledNum and String state
int setLedState(String command) {
    int divLoc = command.indexOf(",");

    int stripNum = (command.substring(0,(divLoc))).toInt();
    String state = command.substring(divLoc + 1, command.length());

    // TODO: Do something with new information

    //Demo Purposes
    Serial.println(command);
    Serial.println(stripNum);
    Serial.println(state);

}

//RestAPI Takes in LED strip to inverse status of (on / off)
int toggleLedState(String command) {
  int stripNum;
  
  try {
    stripNum = command.toInt();
  } 
  catch (int e) {
    int divLoc = command.indexOf(","); 
    stripNum = (command.substring(0,(divLoc))).toInt();
  }

  // TODO: Do something with new information

  //Demo Purposes
  Serial.println(stripNum);

}
