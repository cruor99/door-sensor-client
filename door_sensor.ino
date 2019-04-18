
#include "ESP8266WiFi.h"
#include "WiFiClientSecure.h"

const char* ssid = "";
const char* password = "";
const int SENSOR_PIN = D5;

WiFiClientSecure client;
const char* host = "";
const int httpsPort = 443;

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
  if (!client.connect(host, httpsPort)){
    Serial.println("Connection Failed");
    return false;
  }

  String url = "/sensor?request=controldevicebyvalue&ref=" + String(dvRef) + "&value=" + String(data);
  client.print(String("GET ") + url + " HTTP/1.1\r\n"+
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266-veranda\r\n" +
               "Connection: close\r\n\r\n");
  Serial.println("Request sent (device " + String(dvRef) + ", data " + String(data) +")");
}

void loop() {
  // put your main code here, to run repeatedly:
  checkSensor();
  delay(10000);
}
