int takePhoto(configObj configurationObj){

  //take photo
  camera_fb_t * fb = NULL;
  int err = getPhoto(&fb);
  //errors -> 10, 20
  if(err > 0){
    return err;

    sendError(err);
    
  }else{     

    Serial.println("ciao");
    
    //upload file
    int errorCode = uploadEncoded(fb);   
          
    Serial.println(errorCode);
    
    int err = saveFile(fb, configurationObj);
    
    if(err > 0){
        Serial.println("asdasdas" + String(err));
        sendError(err );
    }     

    esp_camera_fb_return(fb); 
    
  }
  
}
