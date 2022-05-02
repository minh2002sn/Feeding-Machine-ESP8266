#include "wifi.h"

WIFI_DATA_HandleTypeDef WIFI_Data;

void WIFI_Init(){
  SM_Init();
  RT_Init();
  WIFI_Data.state = WIFI_IDLE_STATE;
}

void WIFI_Handle(){
  switch(WIFI_Data.state){
    case WIFI_IDLE_STATE:
      WIFI_Check_Connection();
      RT_Handle();
      break;
    case WIFI_SMART_CONFIG_STATE:
      SM_Execute();
      WIFI_Set_State(WIFI_IDLE_STATE);
      break;
    case WIFI_GET_REAL_TIME_STATE:
      RT_Execute();
      WIFI_Set_State(WIFI_IDLE_STATE);
      break;
    case WIFI_GET_CONNECTION_STATE:
      Serial.print((WiFi.status() == WL_CONNECTED) ? "WIFI CONNECTED\n" : "WIFI DISCONNECTED\n");
      WIFI_Set_State(WIFI_IDLE_STATE);
      break;
    default:
      break;
  }
}

void WIFI_Check_Connection(){
  static int8_t stt = WL_CONNECTED;
  static int8_t lastStt = WL_CONNECTED;
  stt = WiFi.status();
  if(stt != lastStt){
    if(stt == WL_CONNECTED){
      Serial.print("WiFi CONNECTED\n");
    } else{
      Serial.print("WiFi DISCONNECTED\n");
    }
  }
  lastStt = stt;
  if(!WiFi.getAutoConnect()){
    WiFi.setAutoConnect(true);
  }
}

void WIFI_Set_State(WIFI_STATE_HandleTypeDef state){
  WIFI_Data.state = state;
}
