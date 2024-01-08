#include <Arduino.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>
#include <FS.h>

int switch_mode_sec=5; //second(s)

#include "bottom_func.h"
#include "button_setup.h"

#include "oled_setup.h"
#include "oled_pic.h"
Oled oled1;

#include "poweron.h"
#include "wifi_setup.h"
#include "time_server.h"
#include "weather_server.h"


const int resetDuration = 3; // 按下RST鍵3秒後重設WiFi
unsigned long resetTime = 0;
bool resetFlag = false;

int restarttime=12; //hour(s)





void setup(void) {
    pinMode(rstread,INPUT_PULLUP);
    Serial.begin(115200);
    SPIFFS.begin();
    Serial.println("PowerOn");
    powerondisplay();
    Serial.println("disp");
    wifi_try();
    Serial.println("wifi_try()");
    delay(3000);
}

void loop(void) { 
    oled1.show();
    Serial.println("oled.show");
    if(millis()>=restarttime*3600*1000){
      ESP.restart();
    }
    if(!connect_suc){
        server.handleClient();
        Serial.println("wifi not connected");
    }
    
    unsigned long startResetTime = millis();
    while (digitalRead(rstread) == LOW) {
        if (millis() - startResetTime >= (resetDuration * 1000)) {
            resetFlag = true;
            break;
        }
        delay(10);
    }
    if (resetFlag) {
        resetWifiCredentials();
    }

    if(connect_suc){
        doWiFiTick();
        int tmp1=(millis()/(switch_mode_sec*1000))%2;
        if(tmp1==1){
          gettime(false);
          oled_outp_weather_daybig();
        }else if(tmp1==0){
          gettime(true);
        }
        delay(100);
    }
    
    
}
