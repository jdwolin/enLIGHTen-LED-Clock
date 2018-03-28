#include "definitions.h"
#include <NtpClientLib.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <EEPROM.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>
#include <Ticker.h>
#include <WiFiUdp.h>
#include "WS2812FX.h"
Ticker ticker;

int hour_hand = 3, minute_hand, second_hand, previous_second, previous_minute, previous_hour, looptimer, flourishtime = 10, timezone = -5;
int clockmode = 2, timerflag = 99, interuptflag = 1, hourchanged = 0, minchanged = 0, secchanged = 0, secondhandstate = 1; 
int playground = 0, decayline = 120, nightlevel = 20;
char ipstring[20];
int ipint = 0; 
uint8_t LEDr, LEDg, LEDb;
WS2812FX strip = WS2812FX(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);
WS2812FX stripsecs = WS2812FX(NUMLEDSECS, PINS, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
// variables
void tick()
{
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}


// ***************************************************************************
// EEPROM helper
// ***************************************************************************
String readEEPROM(int offset, int len) {
  String res = "";
  for (int i = 0; i < len; ++i)
  {
    res += char(EEPROM.read(i + offset));
    //DBG_OUTPUT_PORT.println(char(EEPROM.read(i + offset)));
  }
  DBG_OUTPUT_PORT.printf("readEEPROM(): %s\n", res.c_str());
  return res;
}

void writeEEPROM(int offset, int len, String value) {
  DBG_OUTPUT_PORT.printf("writeEEPROM(): %s\n", value.c_str());
  for (int i = 0; i < len; ++i)
  {
    if (i < value.length()) {
      EEPROM.write(i + offset, value[i]);
    } else {
      EEPROM.write(i + offset, NULL);
    }
  }
}


// ***************************************************************************
// Saved state handling
// ***************************************************************************
// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


// ***************************************************************************
// Callback for WiFiManager library when config mode is entered
// ***************************************************************************
//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  DBG_OUTPUT_PORT.println("Entered config mode");
  DBG_OUTPUT_PORT.println(WiFi.softAPIP());

  myWiFiManager->resetSettings();
  //if you used auto generated SSID, print it
  DBG_OUTPUT_PORT.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);

  uint16_t i;
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0, 0, 255);
  }
  strip.show();
}

//callback notifying us of the need to save config
void saveConfigCallback () {
  DBG_OUTPUT_PORT.println("Should save config");
  shouldSaveConfig = true;
}

// ***************************************************************************
// Include: Webserver
// ***************************************************************************
#include "spiffs_webserver.h"

// ***************************************************************************
// Include: Request handlers
// ***************************************************************************
#include "request_handlers.h"

// ***************************************************************************
// Include: Color modes
// ***************************************************************************
#include "colormodes.h"



// ***************************************************************************
// MAIN
// ***************************************************************************
void setup() {
  DBG_OUTPUT_PORT.begin(115200);
  EEPROM.begin(512);
  NTP.begin("es.pool.ntp.org", 0 , true); // get time from NTP server pool.
  NTP.setInterval(63);

  // set builtin led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.5, tick);

  // ***************************************************************************
  // Setup: Neopixel
  // ***************************************************************************
  strip.init();
  strip.setBrightness(brightness);
  strip.setSpeed(convertSpeed(ws2812fx_speed));
  strip.setColor(main_color.red, main_color.green, main_color.blue);
  strip.start();
  stripsecs.init();
  stripsecs.setBrightness(brightness);
  stripsecs.setSpeed(convertSpeed(ws2812fx_speed));
  stripsecs.setColor(main_color.red, main_color.green, main_color.blue);
  stripsecs.start();

  // ***************************************************************************
  // Setup: WiFiManager
  // ***************************************************************************

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(HOSTNAME)) {
    DBG_OUTPUT_PORT.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    wifiManager.resetSettings();
    ESP.restart();
  }

  //if you get here you have connected to the WiFif
  DBG_OUTPUT_PORT.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);


  // ***************************************************************************
  // Configure MDNS
  // ***************************************************************************

  MDNS.begin(HOSTNAME);  //mdns responder
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);  // start the socket.
  DBG_OUTPUT_PORT.print("Open http://");
  DBG_OUTPUT_PORT.print(WiFi.localIP());
  DBG_OUTPUT_PORT.println("/ to open McLighting.");
  DBG_OUTPUT_PORT.print("Use http://");
  DBG_OUTPUT_PORT.print(HOSTNAME);
  DBG_OUTPUT_PORT.println(".local/ when you have Bonjour installed.");
  DBG_OUTPUT_PORT.print("New users: Open http://");
  DBG_OUTPUT_PORT.println("/upload to upload the webpages first.");  

  char buffer[100];
  sprintf(buffer, WiFi.localIP().toString().c_str());
  DBG_OUTPUT_PORT.print(buffer); 

  for (int z=0; z<15; z++){   //output ip address so users know where to connect.
    DBG_OUTPUT_PORT.println("");
    DBG_OUTPUT_PORT.println(buffer[z]);
    delay(300);
    for (byte i = 0; i <= NUMLEDSECS; i++) {
    stripsecs.setPixelColor(i, stripsecs.Color(0, 0, 0));
    }
    stripsecs.show();
    clearNumbers();
    delay(100);


    if(buffer[z]=='1'){
      one();
    }
    if(buffer[z]=='2'){
      two();
    }
    if(buffer[z]=='3'){
      three();
    }
    if(buffer[z]=='4'){
      four();
    }
    if(buffer[z]=='5'){
      five();
    }
    if(buffer[z]=='6'){
      six();
    }
    if(buffer[z]=='7'){
      seven();
    }
    if(buffer[z]=='8'){
      eight();
    }
    if(buffer[z]=='9'){
      nine();
    }
    if(buffer[z]=='0'){
      zero();
    }
    if(buffer[z]=='.'){
      dot();
    }
  }

delay(300);


  // ***************************************************************************
  // Setup: SPIFFS
  // ***************************************************************************
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }

    FSInfo fs_info;
    SPIFFS.info(fs_info);
    DBG_OUTPUT_PORT.printf("FS Usage: %d/%d bytes\n\n", fs_info.usedBytes, fs_info.totalBytes);
  }

  // ***************************************************************************
  // Setup: SPIFFS Webserver handler
  // ***************************************************************************
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);
  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/esp_status", HTTP_GET, []() {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    // json += ", \"analog\":" + String(analogRead(A0));
    // json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      handleNotFound();
  });

  server.on("/upload", handleMinimalUpload);
  server.on("/restart", []() {
    DBG_OUTPUT_PORT.printf("/restart\n");
    server.send(200, "text/plain", "restarting..." );
    ESP.restart();
  });

  server.on("/reset_wlan", []() {
    DBG_OUTPUT_PORT.printf("/reset_wlan\n");
    server.send(200, "text/plain", "Resetting WLAN and restarting..." );
    WiFiManager wifiManager;

    wifiManager.resetSettings();
    ESP.restart();
  });

  server.on("/start_config_ap", []() {
    DBG_OUTPUT_PORT.printf("/start_config_ap\n");
    server.send(200, "text/plain", "Starting config AP ..." );
    WiFiManager wifiManager;
    WiFi.mode(WIFI_STA);
    wifiManager.startConfigPortal(HOSTNAME);
  });


  // ***************************************************************************
  // Setup: SPIFFS Webserver handler
  // ***************************************************************************
  server.on("/set_brightness", []() {
    if (server.arg("c").toInt() > 0) {
      brightness = (int) server.arg("c").toInt() * 2.55;
    } else {
      brightness = server.arg("p").toInt();
    }
    if (brightness > 255) {
      brightness = 255;
    }
    if (brightness < 0) {
      brightness = 0;
    }
    strip.setBrightness(brightness);
    if (mode == HOLD) {
      mode = ALL;
    }
    getStatusJSON();
  });

  server.on("/get_brightness", []() {
    String str_brightness = String((int) (brightness / 2.55));
    server.send(200, "text/plain", str_brightness );

 //   DBG_OUTPUT_PORT.print("/get_brightness: ");
 //   DBG_OUTPUT_PORT.println(str_brightness);
  });
  server.on("/get_dimflag", []() {
  String str_dimflag = "[";
        str_dimflag += "{\"mode\":";
    str_dimflag += String ((int) strip.getMode());
    str_dimflag += ", \"name\":\"";
    str_dimflag += "clockmode";
    str_dimflag += "\"},";
        str_dimflag += "{\"mode\":";
    str_dimflag += String ((int)(clockmode));
    str_dimflag += ", \"name\":\"";
    str_dimflag += "ringmode";
    str_dimflag += "\"},";
        str_dimflag += "{\"mode\":";
    str_dimflag += String ((int) (ws2812fx_speed));
    str_dimflag += ", \"name\":\"";
    str_dimflag += "speed";
    str_dimflag += "\"},";
        str_dimflag += "{\"mode\":";
    str_dimflag += String ((int) (brightness));
    str_dimflag += ", \"name\":\"";
    str_dimflag += "brightness";
    str_dimflag += "\"},";
        str_dimflag += "{\"mode\":";
    str_dimflag += String((int) (dimflag));
    str_dimflag += ", \"name\":\"";
    str_dimflag += "dimflag";
    str_dimflag += "\"},";
        str_dimflag += "{\"mode\":";
    str_dimflag += String ((int) (arcflag));
    str_dimflag += ", \"name\":\"";
    str_dimflag += "arcflag";
    str_dimflag += "\"},";
        str_dimflag += "{\"mode\":";
    str_dimflag += String ((int) (secondhandstate));
    str_dimflag += ", \"name\":\"";
    str_dimflag += "secondhandstate";
    str_dimflag += "\"},";
        str_dimflag += "{\"mode\":";
    str_dimflag += String ((int) (dayflag));
    str_dimflag += ", \"name\":\"";
    str_dimflag += "dayflag";
    str_dimflag += "\"},";
    str_dimflag += "{}]";


 //   String str_dimflag =  String ((int) strip.getMode()) + "," + String ((int) (ws2812fx_speed)) + "," + String ((int) (brightness)) + "," + String ((int) (clockmode)) + "," +  String((int) (dimflag)) + "," + String ((int) (arcflag)) + "," + String ((int) (secondhandstate)) + "," + String ((int) (dayflag));
    server.send(200, "application/json", str_dimflag );
  });

  server.on("/set_speed", []() {
    if (server.arg("d").toInt() >= 0) {
      ws2812fx_speed = server.arg("d").toInt();
      ws2812fx_speed = constrain(ws2812fx_speed, 0, 255);
      strip.setSpeed(convertSpeed(ws2812fx_speed));
    }
    getStatusJSON();
  });

 


  server.on("/status", []() {
    getStatusJSON();
  });

  server.on("/off", []() {
    exit_func = true;
    mode = OFF;
    getArgs();
    getStatusJSON();
  });

  server.on("/all", []() {
    exit_func = true;
    mode = ALL;
    getArgs();
    getStatusJSON();
  });

  server.on("/get_modes", []() {
    getModesJSON();
  });

  server.on("/set_mode", []() {
    getArgs();
    mode = SET_MODE;
    getStatusJSON();
  });

  server.begin();


#ifdef ENABLE_STATE_SAVE
  // Load state string from EEPROM
  String saved_state_string = readEEPROM(256, 128);
  String chk = getValue(saved_state_string, '|', 0);
  if (chk == "STA") {
    DBG_OUTPUT_PORT.printf("Found saved state: %s\n", saved_state_string.c_str());
    String str_mode = getValue(saved_state_string, '|', 1);
    mode = static_cast<MODE>(str_mode.toInt());
    String str_ws2812fx_mode = getValue(saved_state_string, '|', 2);
    ws2812fx_mode = str_ws2812fx_mode.toInt();
    String str_ws2812fx_speed = getValue(saved_state_string, '|', 3);
    ws2812fx_speed = str_ws2812fx_speed.toInt();
    String str_brightness = getValue(saved_state_string, '|', 4);
    brightness = str_brightness.toInt();
    String str_red = getValue(saved_state_string, '|', 5);
    main_color.red = str_red.toInt();
    String str_green = getValue(saved_state_string, '|', 6);
    main_color.green = str_green.toInt();
    String str_blue = getValue(saved_state_string, '|', 7);
    main_color.blue = str_blue.toInt();
    str_red = getValue(saved_state_string, '|', 8);
    hournum_color.red = str_red.toInt();
    str_green = getValue(saved_state_string, '|', 9);
    hournum_color.green = str_green.toInt();
    str_blue = getValue(saved_state_string, '|', 10);
    hournum_color.blue = str_blue.toInt();
    str_red = getValue(saved_state_string, '|', 11);
    sec_color.red = str_red.toInt();
    str_green = getValue(saved_state_string, '|', 12);
    sec_color.green = str_green.toInt();
    str_blue = getValue(saved_state_string, '|', 13);
    sec_color.blue = str_blue.toInt();
    str_red = getValue(saved_state_string, '|', 14);
    sweep_color.red = str_red.toInt();
    str_green = getValue(saved_state_string, '|', 15);
    sweep_color.green = str_green.toInt();
    str_blue = getValue(saved_state_string, '|', 16);
    sweep_color.blue = str_blue.toInt();
    str_red = getValue(saved_state_string, '|', 17);
    min_color.red = str_red.toInt();
    str_green = getValue(saved_state_string, '|', 18);
    min_color.green = str_green.toInt();
    str_blue = getValue(saved_state_string, '|', 19);
    min_color.blue = str_blue.toInt();
    String str_temp = getValue(saved_state_string, '|', 20);
    clockmode = str_temp.toInt();
    str_temp = getValue(saved_state_string, '|', 21);
    dimflag = str_temp.toInt();
    str_temp = getValue(saved_state_string, '|', 22);
    arcflag = str_temp.toInt();
    str_temp = getValue(saved_state_string, '|', 23);
    dayflag = str_temp.toInt();
    str_temp = getValue(saved_state_string, '|', 24);
    timezone = str_temp.toInt();
//    DBG_OUTPUT_PORT.printf("ws2812fx_mode: %d\n", ws2812fx_mode);
    strip.setMode(ws2812fx_mode);
    strip.setSpeed(convertSpeed(ws2812fx_speed));
    strip.setBrightness(brightness);
    stripsecs.setBrightness(brightness);
    strip.setColor(main_color.red, main_color.green, main_color.blue);
  }
sprintf(last_state, "STA|%2d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d", mode, ws2812fx_mode, ws2812fx_speed, brightness, main_color.red, main_color.green, main_color.blue, hournum_color.red, hournum_color.green, hournum_color.blue, sec_color.red, sec_color.green, sec_color.blue, sweep_color.red, sweep_color.green, sweep_color.blue, min_color.red, min_color.green, min_color.blue, clockmode, dimflag, arcflag, dayflag,timezone);
#endif

MDNS.addService("http","tcp",80);
startup();
hourchanged=1;
}


void loop() {
  webSocket.loop();
  server.handleClient();

    // Simple statemachine that handles the different modes
    if (mode == SET_MODE) {
      DBG_OUTPUT_PORT.printf("SET_MODE: %d %d\n", ws2812fx_mode, mode);
      strip.setMode(ws2812fx_mode);
      mode = HOLD;
    }
    if (mode == OFF) {
      strip.setColor(0, 0, 0);
      strip.setMode(FX_MODE_STATIC);
      mode = HOLD;
    }
    if (mode == ALL) {
      strip.setColor(main_color.red, main_color.green, main_color.blue);
      strip.setMode(FX_MODE_STATIC);
      mode = HOLD;
    }
   
    if (mode == HOLD || mode == CUSTOM) {
      if (exit_func) {
        exit_func = false;
      }
    }
  strip.service();
  minute_hand = minute() *2;

  //now lets draw the clock

hour_hand = hour() + timezone;
  if (hour_hand < 0){
  hour_hand = hour_hand + 24;
}


if (dimflag == 1){
  if (hour_hand > 19 || hour_hand < 6){
    if(brightness != nightlevel){  // if it's night but we haven't reduced lighting yet
    storebrightness = brightness;  // save daylight brightness
    brightness = nightlevel; // set nighttime brightness
    strip.setBrightness(brightness); //commit
    stripsecs.setBrightness(brightness);      
    }
  }
  else{
    if(brightness == nightlevel){  // if we made it out of first loop for the first time
      brightness = storebrightness;  //restore daylight brightness
      strip.setBrightness(brightness); //commit
      stripsecs.setBrightness(brightness);      
    }
  }
}
else{ // perhaps the dim setting is turned off at night.  We need to restore day brightness.
    if(brightness == nightlevel){  // the nighttime brightness will be set to 10
      brightness = storebrightness;  //restore daylight brightness
      strip.setBrightness(brightness); //commit
      stripsecs.setBrightness(brightness);      
    }
  }

  if (hour_hand >= 12 ) {  // convert from 24 hour time
    hour_hand = hour_hand  - 12;
  }
  
  hour_hand = (hour_hand * 5 ) *2 + 10; // convert to num of pixels;
  second_hand = second() *2;  // multiply * 2 since we are using every other pixel 120leds per revolution.

  if (minute_hand != previous_minute) {  //stuff that happens once a minute
    previous_minute = minute_hand;
    minchanged = 1;
    minchangedreset = 1;
  }

  if (hour_hand != previous_hour) { //stuff that happens once an hour
    previous_hour = hour_hand;
    hourchanged = 1;  //set the flag so that we can erase the previous hour in drawhands routine
  }

  if (second_hand != previous_second) { // stuff that happens once a second
    previous_second = second_hand;
    secchanged = 1;
   // Serial.printf("hour_hand:%d \n", hour_hand);
   // Serial.printf("hour():%d \n", hour());
   // Serial.printf("timezone:%d \n", timezone);
  }

  // swinging hoop clockmode
  if (clockmode == 1) {
  nextpixels();  // simple mode, no animations.
  }

  if (clockmode == 2) {
    bumpme();  //  default
  }
  if(clockmode == 3){
    pendulum();  // pendulum mode
  }

 if(clockmode == 4){
  chasetails(1);  // tails
 }

  if(clockmode == 5){
  chasetails(0);  //no tails
 }

 if(clockmode == 6){
   spider();   // spidey
 }
 if(clockmode == 7){
   swinghoop();   // perpetual motion
 }

#ifdef ENABLE_STATE_SAVE
  // Check for state changes
  sprintf(current_state, "STA|%2d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d", mode, strip.getMode(), ws2812fx_speed, brightness, main_color.red, main_color.green, main_color.blue, hournum_color.red, hournum_color.green, hournum_color.blue, sec_color.red, sec_color.green, sec_color.blue, sweep_color.red, sweep_color.green, sweep_color.blue, min_color.red, min_color.green, min_color.blue,clockmode,dimflag, arcflag, dayflag, timezone);
  
  if (strcmp(current_state, last_state) != 0) {
    
    strcpy(last_state, current_state);

    time_statechange = millis();
    state_save_requested = true;
  }
  if (state_save_requested && time_statechange + timeout_statechange_save <= millis()) {
    time_statechange = 0;
    state_save_requested = false;

    writeEEPROM(256, 128, last_state); // 256 --> last_state (reserved 32 bytes)
    EEPROM.commit();
    DBG_OUTPUT_PORT.printf("STATE CHANGED: %s / %s\n", last_state, current_state);
  }
#endif



}








