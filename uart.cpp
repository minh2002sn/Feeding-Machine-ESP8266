#include "uart.h"
#include "real_time.h"
#include "smart_config.h"

#define MAX_UART_LEN 100

static uint8_t uart_buffer[MAX_UART_LEN] = {};
static uint8_t uart_pointer = 0;
static uint8_t uart_flag = 0;

void UART_Init(){
  
}

void UART_Receive(uint8_t Rx_Data){
  if(Rx_Data == '\n'){
    uart_buffer[uart_pointer++] = '\0';
    uart_flag = 1;
  } else{
    uart_buffer[uart_pointer++] = Rx_Data;
  }
}

void UART_Handle(){
  if(Serial.available() > 0){
    UART_Receive(Serial.read());
  }
  if(uart_flag){
    char arg_value[10][20];
    uint8_t arg_num = 0;
    char *token = strtok((char*)uart_buffer, " ");
    while(token != NULL){
      strcpy(arg_value[arg_num++], token);
      token = strtok(NULL, " ");
    }

    if(strstr(arg_value[0], "GET_REAL_TIME") != NULL){
      WIFI_Set_State(WIFI_GET_REAL_TIME_STATE);
    } else if(strstr(arg_value[0], "SMART_CONFIG") != NULL){
      WIFI_Set_State(WIFI_SMART_CONFIG_STATE);
    } else if(strstr(arg_value[0], "GET_CONNECTION") != NULL){
      WIFI_Set_State(WIFI_GET_CONNECTION_STATE);
    }
    
    uart_pointer = 0;
    uart_flag = 0;
  }
}
