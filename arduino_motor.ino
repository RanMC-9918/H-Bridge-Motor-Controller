#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "ScoobyDoo";
const char* password = "PinkIronMan1075";
const char* host = "10.0.0.77";
const uint16_t port = 3000;

WebSocketsClient ws;

float motorControlSpeed = 0.0;
bool brakeActive = false;

void onWsEvent(WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    StaticJsonDocument<128> doc;
    if (!deserializeJson(doc, payload)) {
      if (doc.containsKey("speed")) motorControlSpeed = doc["speed"].as<float>();
      if (doc.containsKey("brake")) brakeActive = doc["brake"].as<bool>();
    }
  }
}


int pinLU = 3;
int pinLD = 10;
int pinRU = 9;
int pinRD = 5;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  ws.begin(host, port, "/?type=arduino");
  ws.onEvent(onWsEvent);
  ws.setReconnectInterval(3000);

  pinMode(pinLU, OUTPUT);
  pinMode(pinRU, OUTPUT);
  
  pinMode(pinLD, OUTPUT);
  pinMode(pinRD, OUTPUT);
}

int motorSpeed = 0;
void loop() {
  ws.loop();


  int potValue = (int)(motorControlSpeed*255);

  if(brakeActive){
      analogWrite(pinLD, 0);
      analogWrite(pinRD, 0);
    
      digitalWrite(pinLU, HIGH);
      digitalWrite(pinRU, HIGH);
      motorSpeed = 0;
  }
  else{
    if(potValue > 0 && motorSpeed > 0){
      
      digitalWrite(pinLU, LOW);
      digitalWrite(pinRU, HIGH);
      analogWrite(pinLD, potValue);
      motorSpeed = potValue;
    }
    else if(potValue < 0 && motorSpeed < 0){
      digitalWrite(pinRU, LOW);
      digitalWrite(pinLU, HIGH);
      analogWrite(pinRD, -potValue);
      motorSpeed = potValue;
    }
    else{
      digitalWrite(pinLU, LOW);
      digitalWrite(pinRU, LOW);
      analogWrite(pinLD, 0);
      analogWrite(pinRD, 0);
      motorSpeed = potValue;
    }
  }
  
}
