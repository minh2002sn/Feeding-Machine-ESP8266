#include "uart.h"
#include "real_time.h"
#include "smart_config.h"

#define MAX_UART_LEN 100

static uint8_t uart_buffer[MAX_UART_LEN] = {};
static uint8_t uart_pointer = 0;
static uint8_t uart_flag = 0;

void UART_Init(){
  
}

void UART_Receive(uint8_t p_Rx_Data){
  if(p_Rx_Data == '\n'){
    uart_buffer[uart_pointer++] = '\0';
    uart_flag = 1;
  } else{
    uart_buffer[uart_pointer++] = p_Rx_Data;
  }
}

void UART_Handle(){
  // Receive UART string
  if(Serial.available() > 0){
    UART_Receive(Serial.read());
  }

  // Handle UART string
  if(uart_flag){
    char t_arg_value[10][20];
    uint8_t t_arg_num = 0;
    char *t_token = strtok((char*)uart_buffer, " ");
    while(t_token != NULL){
      strcpy(t_arg_value[t_arg_num++], t_token);
      t_token = strtok(NULL, " ");
    }

    if(strstr(t_arg_value[0], "GET_REAL_TIME") != NULL){
      WIFI_Set_State(WIFI_GETTING_REAL_TIME_STATE);
    } else if(strstr(t_arg_value[0], "SMART_CONFIG") != NULL){
      WIFI_Set_State(WIFI_SMART_CONFIG_STATE);
    } else if(strstr(t_arg_value[0], "GET_CONNECTION") != NULL){
      WIFI_Set_State(WIFI_GETTING_CONNECTION_STATE);
    } else if(strstr(t_arg_value[0], "GET_NETWORKS") != NULL){
      WIFI_Set_State(WIFI_SCANNING_NETWORK_STATE);
    } else if(strstr(t_arg_value[0], "CONNECT_WIFI") != NULL){
      if(t_arg_num == 3){
        WIFI_Set_State_Connecting(atoi(t_arg_value[1]), t_arg_value[2]);
      }
    } else if(strstr(t_arg_value[0], "OK") != NULL){
      if(WIFI_Get_State() == WIFI_SENDING_SSID_STATE){
        Enable_Sending_SSID();
      }
    }
    
    uart_pointer = 0;
    uart_flag = 0;
  }
}
