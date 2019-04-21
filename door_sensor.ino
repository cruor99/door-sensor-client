
#include "ESP8266WiFi.h"
// https://github.com/bblanchon/ArduinoJson.git
#include "ArduinoJson.h"

const char* ssid = "";
const char* password = "";
const int SENSOR_PIN = D5;

WiFiClient client;
const char* host = "";
const int httpsPort = 5000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Wifi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(SENSOR_PIN, INPUT_PULLUP);

}


int lastReading = -999;

void checkSensor(){
  Serial.println("Checking door sensor");
  int proximity = digitalRead(SENSOR_PIN);
  Serial.println(proximity);
  if (lastReading != proximity){
    if (proximity == LOW){
      sendDataToServer(556, String(0));
      Serial.println("Switch closed");
    }
    else if (proximity == HIGH){
      sendDataToServer(556, String(1));
      Serial.println("Switch open");
    }
    lastReading = proximity;
  }
  Serial.println("End of sensor reading");
}

void checkWifi(){
    Serial.println("Doing WiFi check");
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wifi dropped, reconnecting...");
      WiFi.begin(ssid, password);
    }
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }
    Serial.println("Ended WiFi check");
}

bool sendDataToServer(int dvRef, String data){

  if (client.connect(host,5000))
  { 
    /*
    String getStr = "/sensor?dvref="+String(dvRef)+"&devicedata="+String(data);
    
    client.println("GET "+getStr);
    client.println(" HTTP/1.1");
    client.println("Host: "+String(host));
    client.println("User-Agent: NodeMCU-veranda");
    client.println("Connection: close");

    client.println();
    */

    String postEndpoint = "/api/v1/sensor";
    String postData = "";
    StaticJsonDocument<200> jsonData;
    jsonData["device_reference"] = String(dvRef);
    jsonData["device_status"] = String(data);

    serializeJson(jsonData, postData);
    Serial.println(postData);
    
    client.print("POST "+postEndpoint);
    client.println(" HTTP/1.1");
    client.println("Host: "+String(host));
    client.println("User-Agent: NodeMCU-veranda");
    client.println("Cache-Control: no-cache");
    client.print("Content-Length:");
    client.println(postData.length());
    client.println("Content-Type: application/json");
    client.println();
    client.println(postData);

    while(client.available()){
  char c = client.read();
  Serial.print(c);
    }
    Serial.println("");
  }
  client.stop();
  Serial.println("Request sent (device " + String(dvRef) + ", data " + String(data) +")");
}

void loop() {
  // put your main code here, to run repeatedly:
  checkSensor();
  delay(10000);
}
