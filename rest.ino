const char* ssid = "hackathon";
const char* password = "summerhack21";

//Your Domain name with URL path or IP address with path
byte serverIP[] = { 10, 10, 206, 38 };
String serverName = "http://10.10.206.38";
String serverPath = "/api/machines/";
const int serverPort = 3000;

WiFiClient client;

void initWifiConnection() { 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
}

int sendError(int errorCode){    
    
    HTTPClient http;      

    String serverString = serverName + ":" + serverPort + serverPath + WiFi.macAddress() + "/errors";
    Serial.println(serverString);
    http.begin(client, serverString);
    
    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"errorCode\":" + String(errorCode) + "}");
   
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
      
    // Free resources
    http.end(); 
}

int sendPing(ConfigObj& configuration, String& initialize){

    WiFiClient client;
    HTTPClient http;
    StaticJsonDocument<200> doc;

    String serverString = serverName + ":" + serverPort + serverPath + WiFi.macAddress() + initialize;

    Serial.println(serverString);

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverString.c_str());
    
    // Send HTTP GET request
    // If you need an HTTP request with a content type: application/json, use the following:    
    int httpResponseCode = http.GET();
    
    if (httpResponseCode >0 && httpResponseCode < 400) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);

      //read json
      DeserializationError errorJson = deserializeJson(doc, payload);

      // Test if parsing succeeds.
      if (errorJson) {
        Serial.print(F("deserializeJson() failed: "));        
        Serial.println(errorJson.c_str());
      } else {
        configuration.paused = doc["paused"];
        configuration.pxFormat = doc["pxFormat"].as<String>();
        configuration.resolution = doc["resolution"];        
        configuration.timestamp = atol(doc["timestamp"].as<String>().c_str());
        configuration.timestampStr = doc["timestamp"].as<String>();
                     
        Serial.println(configuration.timestampStr);
         
      }
    }
    else {
      String payload = http.getString();
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.println(payload);
    }
    // Free resources
    http.end();

    delay(5000);
    
    return httpResponseCode;
 
}


int uploadPhoto(camera_fb_t * fb){

  String getAll;
  String getBody;
  
  Serial.println("Connecting to server: " + serverName); 

  String postPath = serverPath + WiFi.macAddress() + "/image";
  Serial.println(postPath);

  if (client.connect(serverIP, serverPort)) {   
    
    Serial.println("Connection successful!");    
    String head = "--ERMES\r\nContent-Disposition: form-data; name=\"image\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--ERMES--\r\n";
   
    uint32_t imageLen = fb->len;    

    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;
   
    client.println("POST " + postPath + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=ERMES");
    client.println();
    client.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0; n<fbLen; n=n+1024) {
      if (n+1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        client.write(fbBuf, remainder);
      }
    }   
    client.print(tail);

    Serial.println("ciao");
    esp_camera_fb_return(fb);
    Serial.println("ciao");
    
    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      delay(100);      
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (getAll.length()==0) { state=true; }
          getAll = "";
        }
        else if (c != '\r') { getAll += String(c); }
        if (state==true) { getBody += String(c); }
        startTimer = millis();
      }
      if (getBody.length()>0) { break; }
    }
    Serial.println();
    client.stop();
    Serial.println(getBody);
        
  }
  else {
    getBody = "Connection to " + postPath +  " failed.";
    Serial.println(getBody);
  }

  return 1;
  
}

int uploadEncoded(camera_fb_t * fb){
    HTTPClient http;

    String imgDataB64 = base64::encode(fb->buf, fb->len);
       
    // Your Domain name with URL path or IP address with path

    String serverString = serverName + ":" + serverPort + serverPath + WiFi.macAddress() + "/image";
    
    http.begin(client, serverString);
    
    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"image\":\"" + imgDataB64 + "\"}");
   
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
      
    // Free resources
    http.end();

    return httpResponseCode;
}
