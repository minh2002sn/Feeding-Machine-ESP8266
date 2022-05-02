#include "uart.h"
#include "wifi.h"

void setup() {
  Serial.begin(115200);

  UART_Init();
  WIFI_Init();
}

void loop() {
  UART_Handle();
  WIFI_Handle();
}
