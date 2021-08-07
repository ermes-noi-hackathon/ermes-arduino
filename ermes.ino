// show the awake times of the CPU on output LED_BUILTIN
#define AWAKE_INDICATION_PIN LED_BUILTIN
// delay sleep by 100 milli seconds to allow finishing serial write
#define SLEEP_DELAY 100
#define SUPERVISION_CALLBACK_TIMEOUT NO_SUPERVISION

//STRUCT

struct configObj{
  bool paused;
  String pxFormat;
  int resolution;
  int ore[50];
  unsigned long timestamp;
  String timestampStr;
};
typedef struct configObj ConfigObj;

//LIBRARIES
#include <DeepSleepScheduler.h>
#include <ArduinoJson.h>
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <HTTPClient.h>
#include "esp_camera.h"
#include <Base64.h>

//VARIABLES
ConfigObj configurationObj;

void sendPingInterval() {
  // usage of F() puts the strings to the flash and therefore saves main memory
  Serial.print(F("Sending Ping()\n"));    

  String initial = "";
  sendPing(configurationObj,initial);

  // If you use around 1 second or less, millis() and scheduler.getMillis()
  // will be equal because the CPU is not put to SLEEP_MODE_PWR_DOWN in that case.
  scheduler.scheduleDelayed(sendPingInterval, 60000);
}

void takePhotoInterval(){
   // usage of F() puts the strings to the flash and therefore saves main memory
  Serial.print(F("taking photo()\n"));

  takePhoto(configurationObj);
      
  scheduler.scheduleDelayed(takePhotoInterval, 30000);
 
}

void setup() {
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  
  Serial.begin(115200);

  Serial.println(millis());

  initialize();
  
  scheduler.schedule(sendPingInterval);
  scheduler.schedule(takePhotoInterval);  
  
}

void loop() {
  scheduler.execute();  
}

void initialize(){

  digitalWrite(4, HIGH);

  //initialize wifi until gets connection
  initWifiConnection();
  
  //get initial configurations
  String initial = "?initialConnection=1";
  while(sendPing(configurationObj, initial)<0){
    //set configurations
    ;
  }
  
  digitalWrite(4, LOW);
  delay(200);
  digitalWrite(4, HIGH);
  delay(200);
  digitalWrite(4, LOW);
  delay(200);
  digitalWrite(4, HIGH);
  delay(200);
  digitalWrite(4, LOW);
  delay(200);
  digitalWrite(4, HIGH);
  delay(200);
  digitalWrite(4, LOW);
  
}
