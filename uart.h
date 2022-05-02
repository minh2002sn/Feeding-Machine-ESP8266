#ifndef COMMAND_LINE_INTERFACE_H
#define COMMAND_LINE_INTERFACE_H

#include "Arduino.h"

void UART_Init();
void UART_Receive(uint8_t p_Rx_Buffer);
void UART_Handle();

#endif
