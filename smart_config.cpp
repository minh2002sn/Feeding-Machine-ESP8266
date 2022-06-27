#include "smart_config.h"

static char ssid[30] = {};
static char pass[30] = {};

void SM_Init(){
  WiFi.begin();
  WiFi.setAutoConnect(true);
  WiFi.persistent(true);
  Serial.println();
  Serial.print("WIFI CONNECTING\n");
  delay(500);
  if(WiFi.waitForConnectResult(20000) == WL_CONNECTED){
    Serial.print("WiFi CONNECTED\n");
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  } else{
    Serial.print("WIFI DISCONNECTED\n");
  }
}

void SM_Execute(){
  strcpy(ssid, WiFi.SSID().c_str());
  strcpy(pass, WiFi.psk().c_str());
//  Serial.println(ssid);
//  Serial.println(pass);

  Serial.print("SMART_CONFIG_RUN\n");
  WiFi.mode(WIFI_STA);
  WiFi.beginSmartConfig();
  uint32_t timer = millis();
  while(!WiFi.smartConfigDone()){
    delay(500);
    if(millis() - timer > 30000){
      WiFi.stopSmartConfig();
      Serial.print("WIFI CONNECTING\n");
      WiFi.begin(ssid, pass);
      if(WiFi.waitForConnectResult(20000) == WL_CONNECTED){
        Serial.print("WIFI CONNECTED\n");
        WiFi.setAutoReconnect(true);
        WiFi.persistent(true);
      } else{
        Serial.print("WIFI DISCONNECTED\n");
      }
      return;
    }
  }
  Serial.print("WIFI CONNECTING\n");
  delay(500);
  if(WiFi.waitForConnectResult(20000) == WL_CONNECTED){
    Serial.print("WiFi CONNECTED\n");
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  } else{
    Serial.print("WIFI DISCONNECTED\n");
  }
}
