int saveFile(camera_fb_t * fb, configObj configurationObj){
  
  //Serial.println("Starting SD Card");
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return 10;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return 20;
  }

   // initialize EEPROM with predefined size
//  EEPROM.begin(EEPROM_SIZE);
//  pictureNumber = EEPROM.read(0) + 1;
 

  // Path where new picture will be saved in SD Card
  String path = "/picture-" + configurationObj.timestampStr + String(millis()) +".jpg";

  fs::FS &fs = SD_MMC; 
  Serial.printf("Picture file name: %s\n", path.c_str());
  
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode");
    return 30;
  } 
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
  file.close();

  return 0;
}
