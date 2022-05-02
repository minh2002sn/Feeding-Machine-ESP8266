#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "Arduino.h"
#include "real_time.h"
#include "smart_config.h"

typedef enum{
  WIFI_IDLE_STATE,
  WIFI_SMART_CONFIG_STATE,
  WIFI_GETTING_REAL_TIME_STATE,
  WIFI_GETTING_CONNECTION_STATE,
  WIFI_SCANNING_NETWORK_STATE,
  WIFI_CONNECTING_NETWORK_STATE,
}WIFI_STATE_HandleTypeDef;

typedef struct{
  WIFI_STATE_HandleTypeDef state;
}WIFI_DATA_HandleTypeDef;

void WIFI_Init();
void WIFI_Handle();
void WIFI_Check_Connection();
void WIFI_Scan();
void WIFI_Connect();

void WIFI_Set_State(WIFI_STATE_HandleTypeDef p_state);
void WIFI_Set_State_Connecting(uint8_t p_ssid_index, char p_pass[]);

#endif
