#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "real_time.h"

WiFiUDP udp;
NTPClient ntp(udp, "0.vn.pool.ntp.org", 7*3600);
static String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", 
                            "Thursday", "Friday", "Saturday"};
static uint8_t day_data[7] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40};

static uint8_t minute = 0;
static uint8_t hour = 0;
static uint8_t day = 1;

void RT_Init(){
  ntp.begin();
}

void RT_Execute(){
  Serial.print("TIME ");
  
  Serial.print(hour);  
  Serial.print(" ");

  Serial.print(minute);
  Serial.print(" ");
  
  Serial.print(day);
  Serial.print("\n");
}

void RT_Handle(){
  static uint8_t t_last_hour = 0;
  static uint8_t t_last_minute = 0;
  static uint8_t t_last_day = 1;
  static uint32_t t_timer = 0;

  if(WiFi.status() == WL_CONNECTED){
    ntp.update();

    minute = ntp.getMinutes();
    hour = ntp.getHours();
    day = day_data[ntp.getDay()];

    t_timer = millis();
  } else{
    if(millis() - t_timer >= 60000){
      if(minute == 59){
        minute = 0;
        if(hour == 23){
          hour = 0;
          if(day == 0b01000000){
            day = 0b00000001;
          } else{
            day = day << 1;
          }
        } else{
          hour++;
        }
      } else{
        minute++;
      }
      t_timer = millis();
    }
  }

  if(t_last_hour != hour || t_last_minute != minute || t_last_day != day){
    RT_Execute();
  }

  t_last_hour = hour;
  t_last_minute = minute;
  t_last_day = day;
}
