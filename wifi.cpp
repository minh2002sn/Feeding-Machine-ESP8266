#include "wifi.h"

#define SENDING_SSID_TIMEOUT 20000

WIFI_DATA_HandleTypeDef WIFI_Data;

static char new_ssid[20] = {};
static char new_pass[20] = {};
static char ssid[30] = {};
static char pass[30] = {};
static uint8_t num_of_wifi;
static uint8_t *ssid_list;
static uint8_t num_of_sending_str;
static bool enable_sending_ssid = 0;
static uint32_t sending_ssid_timer = 0;

static bool check_str(const char p_str[]){
  for(int i = 0; i < strlen(p_str); i++){
    if(p_str[i] > 127){
      return false;
    }
  }
  return true;
}

void Enable_Sending_SSID(){
  enable_sending_ssid = 1;
  sending_ssid_timer = millis();
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
//    case WIFI_SMART_CONFIG_STATE:
//      SM_Execute();
//      WIFI_Set_State(WIFI_IDLE_STATE);
//      break;
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
      WIFI_Set_State(WIFI_SENDING_SSID_STATE);
      break;
    case WIFI_SENDING_SSID_STATE:
      WIFI_Send_SSID();
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
  static uint32_t t_timer = 0;
  t_stt = WiFi.status();
  if(t_stt != t_last_stt){
    if(t_stt == WL_CONNECTED){
      Serial.print("WIFI CONNECTED\n");
    } else{
      Serial.print("WIFI DISCONNECTED\n");
    }
    t_last_stt = t_stt;
    t_timer = millis();
  }

  if(!WiFi.getAutoConnect()){
    WiFi.setAutoConnect(true);
    WiFi.persistent(true);
  }

  if(millis() - t_timer > 30000){
    if(t_stt == WL_CONNECTED){
      Serial.print("WIFI CONNECTED\n");
    } else{
      Serial.print("WIFI DISCONNECTED\n");
    }
    t_timer = millis();
  }
  if(millis() - t_timer > 30000){
    if(t_stt == WL_CONNECTED){
      Serial.print("WIFI CONNECTED\n");
    } else{
      Serial.print("WIFI DISCONNECTED\n");
    }
    t_timer = millis();
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

  if(num_of_wifi > 6) num_of_wifi = 6;
  num_of_sending_str = num_of_wifi + 2; // include "SSID_START" and "SSID_END"

  Enable_Sending_SSID();
}

void WIFI_Send_SSID(){
  if(millis() - sending_ssid_timer < SENDING_SSID_TIMEOUT){
    if(enable_sending_ssid){
      if(num_of_sending_str == num_of_wifi + 2){
        Serial.println("SSID_START");
      } else if(num_of_sending_str == 1){
        Serial.println("SSID_END");
      } else if(num_of_sending_str > 1){
        Serial.printf("%s\n", WiFi.SSID(ssid_list[num_of_wifi - num_of_sending_str + 1]).c_str());
      }
      if(num_of_sending_str == 0){
        WIFI_Set_State(WIFI_IDLE_STATE);
      }
      num_of_sending_str--;
      enable_sending_ssid = 0;
    }
  } else{
    WIFI_Set_State(WIFI_IDLE_STATE);
  }
}

void WIFI_Connect(){
  WiFi.scanDelete();
  strcpy(ssid, WiFi.SSID().c_str());
  strcpy(pass, WiFi.psk().c_str());
  Serial.print("WIFI CONNECTING\n");
  WiFi.disconnect();
  WiFi.begin(new_ssid, new_pass);
  if(WiFi.waitForConnectResult(20000) == WL_CONNECTED){
    Serial.print("WIFI CONNECTED\n");
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  } else{
    Serial.print("WIFI DISCONNECTED\n");
    WiFi.disconnect();
    WiFi.begin(ssid, pass);
    WiFi.waitForConnectResult(20000);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
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

WIFI_STATE_HandleTypeDef WIFI_Get_State(){
  return WIFI_Data.state;
}
