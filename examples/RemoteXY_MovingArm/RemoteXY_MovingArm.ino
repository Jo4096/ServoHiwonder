#include <ServoHiwonder.hpp>
/*
   -- New project --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.13 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.14.08 or later version;
     - for iOS 1.11.2 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#if defined(ESP8266)
  #define REMOTEXY_MODE__WIFI_POINT
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #define REMOTEXY_MODE__WIFI_POINT
  #include <WiFi.h>
#elif defined(PICO_W)
  #define REMOTEXY_MODE__WIFI_POINT
  #include <WiFi.h>
#endif


// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "ESP8266_ARM"
#define REMOTEXY_WIFI_PASSWORD "CHANGE_ME"
#define REMOTEXY_SERVER_PORT 6377
#define REMOTEXY_ACCESS_PASSWORD "CHANGE_ME"

#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 43 bytes
  { 255,3,0,0,0,36,0,18,0,0,0,28,1,200,84,1,1,3,0,4,
  6,4,15,72,0,36,26,4,25,3,15,73,0,135,26,4,44,3,15,73,
  0,178,26 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t xSlider; // from 0 to 100
  int8_t ySlider; // from 0 to 100
  int8_t zSlider; // from 0 to 100

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



void setup() 
{
  RemoteXY_Init (); 
  RemoteXY.xSlider = 0;
  RemoteXY.ySlider = 30;
  RemoteXY.zSlider = 40;  

  INIT_SERIAL_COM();
}

float lastX = 0;
float lastY = 0;
float lastZ = 0;
float l1 = 13, l2 = 13.6;
ServoController servoCont(Serial);


bool getXYZ(int16_t* x, int16_t* y, int16_t* z)
{
  if(!x || !y || !z){return false;}
  *x = map(RemoteXY.xSlider, 0, 100, 0, 1000);
  *y = map(RemoteXY.ySlider, 0, 100, 0, 1000);
  *z = map(RemoteXY.zSlider, 0, 100, 0, 1000);
  return true;
}

bool updateIfNeeded(const int16_t x, const int16_t y, const int16_t z)
{
  bool returnVal =  (x != lastX || y != lastY || z == lastZ);
  if(returnVal)
  {
    lastX = x;
    lastY = y;
    lastZ = z;
  } 
  return returnVal;
}

void updateServos(uint8_t id1, uint8_t id2, uint8_t id3)
{
  int16_t x = 0;
  int16_t y = 0;
  int16_t z = 0;
  if(!getXYZ(&x, &y, &z)){return;}

  if(updateIfNeeded(x, y, z))
  {
    servoCont.moveWithTime(1, x, 1000);
    servoCont.moveWithTime(2, y, 1000);
    servoCont.moveWithTime(3, z, 1000);
  }
}


void loop() 
{ 
  RemoteXY_Handler();
  updateServos(1, 2, 3);
}