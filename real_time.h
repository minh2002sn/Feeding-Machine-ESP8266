#ifndef REAL_TIME_H
#define REAL_TIME_H

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "wifi.h"

void RT_Init();
void RT_Execute();
void RT_Handle();

#endif
