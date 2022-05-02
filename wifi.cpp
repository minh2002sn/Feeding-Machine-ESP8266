#include "wifi.h"

WIFI_DATA_HandleTypeDef WIFI_Data;

static char new_ssid[20] = {};
static char new_pass[20] = {};
static uint8_t num_of_wifi;
static uint8_t *ssid_list;

static bool check_str(const char p_str[]){
  for(int i = 0; i < strlen(p_str); i++){
    if(p_str[i] > 127){
      return false;
    }
  }
  return true;
}

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
    case WIFI_GETTING_REAL_TIME_STATE:
      RT_Execute();
      WIFI_Set_State(WIFI_IDLE_STATE);
      break;
    case WIFI_GETTING_CONNECTION_STATE:
      Serial.print((WiFi.status() == WL_CONNECTED) ? "WIFI CONNECTED\n" : "WIFI DISCONNECTED\n");
      WIFI_Set_State(WIFI_IDLE_STATE);
      break;
    case WIFI_SCANNING_NETWORK_STATE:
      WIFI_Scan();
      WIFI_Set_State(WIFI_IDLE_STATE);
      break;
    case WIFI_CONNECTING_NETWORK_STATE:
      WIFI_Connect();
      WIFI_Set_State(WIFI_IDLE_STATE);
      break;
    default:
      break;
  }
}

void WIFI_Check_Connection(){
  static int8_t t_stt = WL_CONNECTED;
  static int8_t t_last_stt = WL_CONNECTED;
  t_stt = WiFi.status();
  if(t_stt != t_last_stt){
    if(t_stt == WL_CONNECTED){
      Serial.print("WiFi CONNECTED\n");
    } else{
      Serial.print("WiFi DISCONNECTED\n");
    }
    t_last_stt = t_stt;
  }
  if(!WiFi.getAutoConnect()){
    WiFi.setAutoConnect(true);
  }
}

void WIFI_Scan(){
  WiFi.scanDelete();
  num_of_wifi = WiFi.scanNetworks();
  ssid_list = (uint8_t*)malloc(num_of_wifi * sizeof(int));
  for(int i = 0; i < num_of_wifi; i++){
    ssid_list[i] = i;
  }

  // Sort by RSSI (Received Signal Strength Indicator)
  for(int i = 0; i < num_of_wifi - 1; i++){
    for(int j = i + 1; j < num_of_wifi; j++){
      if(WiFi.RSSI(ssid_list[i]) < WiFi.RSSI(ssid_list[j])){
        uint8_t t_ssid_index = ssid_list[i];
        ssid_list[i] = ssid_list[j];
        ssid_list[j] = t_ssid_index;
      }
    }
  }

  // Remove SSID has non-ASCII characters
  for(int i = 0; i < num_of_wifi; i++){
    if(!check_str(WiFi.SSID(ssid_list[i]).c_str())){
      for(int j = i; i < num_of_wifi - 1; i++){
        ssid_list[j] = ssid_list[j+1];
      }
      num_of_wifi--;
    }
  }

  Serial.println("SSID_START");
  for(int i = 0; i < ((num_of_wifi <= 6) ? num_of_wifi : 6); i++){
    Serial.printf("SSID %s\n", WiFi.SSID(ssid_list[i]).c_str());
  }
  Serial.println("SSID_END");
}

void WIFI_Connect(){
  WiFi.begin(new_ssid, new_pass);
  Serial.print("WIFI CONNECTING\n");
  if(WiFi.waitForConnectResult(20000) == WL_CONNECTED){
    Serial.print("WiFi CONNECTED\n");
    WiFi.setAutoReconnect(true);
  } else{
    Serial.print("WIFI DISCONNECTED\n");
  }
}

void WIFI_Set_State(WIFI_STATE_HandleTypeDef p_state){
  WIFI_Data.state = p_state;
}

void WIFI_Set_State_Connecting(uint8_t p_ssid_index, char p_pass[]){
  WIFI_Data.state = WIFI_CONNECTING_NETWORK_STATE;
  strcpy(new_ssid, WiFi.SSID(ssid_list[p_ssid_index]).c_str());
  strcpy(new_pass, p_pass);
}
