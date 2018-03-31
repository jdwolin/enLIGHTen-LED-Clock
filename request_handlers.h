// ***************************************************************************
// Request handlers

// ***************************************************************************
void getArgs() {
  if (server.arg("rgb") != "") {
    uint32_t rgb = (uint32_t) strtol(server.arg("rgb").c_str(), NULL, 16);
    main_color.red = ((rgb >> 16) & 0xFF);
    main_color.green = ((rgb >> 8) & 0xFF);
    main_color.blue = ((rgb >> 0) & 0xFF);
  } else {
    main_color.red = server.arg("r").toInt();
    main_color.green = server.arg("g").toInt();
    main_color.blue = server.arg("b").toInt();
  }
  ws2812fx_speed = constrain(server.arg("s").toInt(), 0, 255);
  if (server.arg("s") == "") {
    ws2812fx_speed = 196;
  }

  if (server.arg("m") != "") {
    ws2812fx_mode = constrain(server.arg("m").toInt(), 0, strip.getModeCount() - 1);
  }

  main_color.red = constrain(main_color.red, 0, 255);
  main_color.green = constrain(main_color.green, 0, 255);
  main_color.blue = constrain(main_color.blue, 0, 255);

  DBG_OUTPUT_PORT.print("Mode: ");
  DBG_OUTPUT_PORT.print(mode);
  DBG_OUTPUT_PORT.print(", Color: ");
  DBG_OUTPUT_PORT.print(main_color.red);
  DBG_OUTPUT_PORT.print(", ");
  DBG_OUTPUT_PORT.print(main_color.green);
  DBG_OUTPUT_PORT.print(", ");
  DBG_OUTPUT_PORT.print(main_color.blue);
  DBG_OUTPUT_PORT.print(", Speed:");
  DBG_OUTPUT_PORT.print(ws2812fx_speed);
  DBG_OUTPUT_PORT.print(", Brightness:");
  DBG_OUTPUT_PORT.println(brightness);
}


long convertSpeed(int mcl_speed) {
  long ws2812_speed = mcl_speed * 256;
  ws2812_speed = SPEED_MAX - ws2812_speed;
  if (ws2812_speed < SPEED_MIN) {
    ws2812_speed = SPEED_MIN;
  }
  if (ws2812_speed > SPEED_MAX) {
    ws2812_speed = SPEED_MAX;
  }
  return ws2812_speed;
}


// ***************************************************************************
// Handler functions for WS
// ***************************************************************************
void handleSetMainColor(uint8_t * mypayload) {
  // decode rgb data
  uint32_t rgb = (uint32_t) strtol((const char *) &mypayload[1], NULL, 16);
  main_color.red = ((rgb >> 16) & 0xFF);
  main_color.green = ((rgb >> 8) & 0xFF);
  main_color.blue = ((rgb >> 0) & 0xFF);
  strip.setColor(main_color.red, main_color.green, main_color.blue);
}

void handleSetTimezone(uint8_t * mypayload) {
uint32_t zone = (uint32_t) strtol((const char *) &mypayload[1], NULL, 16);
timezone = (int) zone;  // we need to cast from uint32_t to int  
hourchanged = 1;
}

void handleSaveSettings(uint8_t * mypayload) {
uint32_t settings = (uint32_t) strtol((const char *) &mypayload[1], NULL, 16);
}



void handleSetMinColor(uint8_t * mypayload) {
  // decode rgb data
  uint32_t rgb = (uint32_t) strtol((const char *) &mypayload[1], NULL, 16);
  min_color.red = ((rgb >> 16) & 0xFF);
  min_color.green = ((rgb >> 8) & 0xFF);
  min_color.blue = ((rgb >> 0) & 0xFF);
 // stripsecs.setColor(min_color.red, min_color.green, min_color.blue);
}

void handleSetHourColor(uint8_t * mypayload) {
  // decode rgb data
  uint32_t rgb = (uint32_t) strtol((const char *) &mypayload[1], NULL, 16);
  hour_color.red = ((rgb >> 16) & 0xFF);
  hour_color.green = ((rgb >> 8) & 0xFF);
  hour_color.blue = ((rgb >> 0) & 0xFF);
//  stripsecs.setColor(hour_color.red, hour_color.green, hour_color.blue);
}


void handleSetHourNumberColor(uint8_t * mypayload) {
  // decode rgb data
  uint32_t rgb = (uint32_t) strtol((const char *) &mypayload[1], NULL, 16);
  hournum_color.red = ((rgb >> 16) & 0xFF);
  hournum_color.green = ((rgb >> 8) & 0xFF);
  hournum_color.blue = ((rgb >> 0) & 0xFF);
  //strip.setColor(hournum_color.red, hournum_color.green, hournum_color.blue);
  hourchanged = 1;
}


void handleSetSecDotColor(uint8_t * mypayload) {
  // decode rgb data
  uint32_t rgb = (uint32_t) strtol((const char *) &mypayload[1], NULL, 16);
  sec_color.red = ((rgb >> 16) & 0xFF);
  sec_color.green = ((rgb >> 8) & 0xFF);
  sec_color.blue = ((rgb >> 0) & 0xFF);
 // stripsecs.setColor(sec_color.red, sec_color.green, sec_color.blue);
}


void handleSetSweepColor(uint8_t * mypayload) {
  // decode rgb data
  uint32_t rgb = (uint32_t) strtol((const char *) &mypayload[1], NULL, 16);
  sweep_color.red = ((rgb >> 16) & 0xFF);
  sweep_color.green = ((rgb >> 8) & 0xFF);
  sweep_color.blue = ((rgb >> 0) & 0xFF);
 // stripsecs.setColor(sweep_color.red, sweep_color.green, sweep_color.blue);
}


void handleSetAltColor(uint8_t * mypayload) {
  // decode rgb data
  uint32_t rgb = (uint32_t) strtol((const char *) &mypayload[1], NULL, 16);
  alt_color.red = ((rgb >> 16) & 0xFF);
  alt_color.green = ((rgb >> 8) & 0xFF);
  alt_color.blue = ((rgb >> 0) & 0xFF);
 // stripsecs.setColor(alt_color.red, alt_color.green, alt_color.blue);
}

void handleSetAllMode(uint8_t * mypayload) {
  // decode rgb data
  uint32_t rgb = (uint32_t) strtol((const char *) &mypayload[1], NULL, 16);

  main_color.red = ((rgb >> 16) & 0xFF);
  main_color.green = ((rgb >> 8) & 0xFF);
  main_color.blue = ((rgb >> 0) & 0xFF);

  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, main_color.red, main_color.green, main_color.blue);
  }
  strip.show();
  DBG_OUTPUT_PORT.printf("WS: Set all leds to main color: [%u] [%u] [%u]\n", main_color.red, main_color.green, main_color.blue);
  exit_func = true;
  mode = ALL;
}

void handleSetSingleLED(uint8_t * mypayload, uint8_t firstChar = 0) {
  // decode led index
  char templed[3];
  strncpy (templed, (const char *) &mypayload[firstChar], 2 );
  uint8_t led = atoi(templed);

  //DBG_OUTPUT_PORT.printf("led value: [%i]. Entry threshold: <= [%i]\n", led, strip.numPixels() );
  if (led <= strip.numPixels()) {
    char redhex[3];
    char greenhex[3];
    char bluehex[3];
    strncpy (redhex, (const char *) &mypayload[2], 2 );
    strncpy (greenhex, (const char *) &mypayload[4], 2 );
    strncpy (bluehex, (const char *) &mypayload[6], 2 );
    ledstates[led].red =   strtol(redhex, NULL, 16);
    ledstates[led].green = strtol(greenhex, NULL, 16);
    ledstates[led].blue =  strtol(bluehex, NULL, 16);
    DBG_OUTPUT_PORT.printf("rgb.red: [%i] rgb.green: [%i] rgb.blue: [%i]\n", strtol(redhex, NULL, 16), strtol(greenhex, NULL, 16), strtol(bluehex, NULL, 16));
    DBG_OUTPUT_PORT.printf("WS: Set single led [%i] to [%i] [%i] [%i] (%s)!\n", led, ledstates[led].red, ledstates[led].green, ledstates[led].blue, mypayload);


    strip.setPixelColor(led, ledstates[led].red, ledstates[led].green, ledstates[led].blue);
    strip.show();
  }
  exit_func = true;
  mode = CUSTOM;
}

void handleSetDifferentColors(uint8_t * mypayload) {
  uint8_t* nextCommand = 0;
  nextCommand = (uint8_t*) strtok((char*) mypayload, "+");
  while (nextCommand) {
    handleSetSingleLED(nextCommand, 0);
    nextCommand = (uint8_t*) strtok(NULL, "+");
  }
}

void handleRangeDifferentColors(uint8_t * mypayload) {
  uint8_t* nextCommand = 0;
  nextCommand = (uint8_t*) strtok((char*) mypayload, "R");
  //While there is a range to process R0110<00ff00>

  while (nextCommand) {
    //Loop for each LED.
    char startled[3] = { 0, 0, 0 };
    char endled[3] = { 0, 0, 0 };
    char colorval[7] = { 0, 0, 0, 0, 0, 0, 0 };
    strncpy ( startled, (const char *) &nextCommand[0], 2 );
    strncpy ( endled, (const char *) &nextCommand[2], 2 );
    strncpy ( colorval, (const char *) &nextCommand[4], 6 );
    int rangebegin = atoi(startled);
    int rangeend = atoi(endled);
    DBG_OUTPUT_PORT.printf("Setting RANGE from [%i] to [%i] as color [%s] \n", rangebegin, rangeend, colorval);

    while ( rangebegin <= rangeend ) {
      char rangeData[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      if ( rangebegin < 10 ) {
        //Create the valid 'nextCommand' structure
        sprintf(rangeData, "0%d%s", rangebegin, colorval);
      }
      if ( rangebegin >= 10 ) {
        //Create the valid 'nextCommand' structure
        sprintf(rangeData, "%d%s", rangebegin, colorval);
      }
      //Set one LED
      handleSetSingleLED((uint8_t*) rangeData, 0);
      rangebegin++;
    }

    //Next Range at R
    nextCommand = (uint8_t*) strtok(NULL, "R");
  }
}

void handleSetNamedMode(String str_mode) {
  exit_func = true;

  if (str_mode.startsWith("=off")) {
    mode = OFF;
  }
  if (str_mode.startsWith("=all")) {
    mode = ALL;
  }
  if (str_mode.startsWith("=wipe")) {
    mode = WIPE;
  }
  if (str_mode.startsWith("=rainbow")) {
    mode = RAINBOW;
  }
  if (str_mode.startsWith("=rainbowCycle")) {
    mode = RAINBOWCYCLE;
  }
  if (str_mode.startsWith("=theaterchase")) {
    mode = THEATERCHASE;
  }
  if (str_mode.startsWith("=theaterchaseRainbow")) {
    mode = THEATERCHASERAINBOW;
  }
  if (str_mode.startsWith("=tv")) {
    mode = TV;
  }
  //  clock modes on Pane 1

  if (str_mode.startsWith("=justaminute")) {
    clockmode = 1;
    minchanged = 1;
    minchangedreset = 1;
  }
  if (str_mode.startsWith("=bumpme")) {
    clockmode = 2;
    minchanged = 1;
    minchangedreset = 1;
  }
    if (str_mode.startsWith("=pendulum")) {
    clockmode = 3;
    minchanged = 1;
    minchangedreset = 1;
  }
  if (str_mode.startsWith("=chasetails")) {
    clockmode = 4;
    minchanged = 1;
    minchangedreset = 1;
  }
  if (str_mode.startsWith("=notails")) {
    clockmode = 5;
    minchanged = 1;
    minchangedreset = 1;
  }
  if (str_mode.startsWith("=minimalist")) {
    clockmode = 6;
    minchanged = 1;
    minchangedreset = 1;
  }
    if (str_mode.startsWith("=perpetualmotion")) {
    clockmode = 7;
    minchanged = 1;
    minchangedreset = 1;
  }
  if (str_mode.startsWith("=falling")) {
    clockmode = 8;
    minchanged = 1;
    minchangedreset = 1;
  }
  if (str_mode.startsWith("=springs")) {
    clockmode = 9;
    minchanged = 1;
    minchangedreset = 1;
  }
  if (str_mode.startsWith("=fountain")) {
    clockmode = 10;
    minchanged = 1;
    minchangedreset = 1;
  }
    if (str_mode.startsWith("=living")) {
    clockmode = 11;
    minchanged = 1;
    minchangedreset = 1;
  }
  if (str_mode.startsWith("=stars")) {
    clockmode = 12;
    minchanged = 1;
    minchangedreset = 1;
  }
  if (str_mode.startsWith("=hope")) {
    clockmode = 13;
    minchanged = 1;
    minchangedreset = 1;
  }
}

void handleSetWS2812FXMode(uint8_t * mypayload) {  // I think this is what sets the mode.
  mode = HOLD;
  uint8_t ws2812fx_mode = (uint8_t) strtol((const char *) &mypayload[1], NULL, 10);
  ws2812fx_mode = constrain(ws2812fx_mode, 0, 255);
  strip.setColor(main_color.red, main_color.green, main_color.blue);
  strip.setMode(ws2812fx_mode);
  strip.start();

}

char* listStatusJSON() {
  char json[255];

  char modeName[30];
  strncpy_P(modeName, (PGM_P)strip.getModeName(strip.getMode()), sizeof(modeName)); // copy from progmem

  snprintf(json, sizeof(json), "{\"mode\":%d, \"ws2812fx_mode\":%d, \"ws2812fx_mode_name\":\"%s\", \"speed\":%d, \"brightness\":%d, \"color\":[%d, %d, %d]}",
           mode, strip.getMode(), modeName, ws2812fx_speed, brightness, main_color.red, main_color.green, main_color.blue);
  return json;
}

void getStatusJSON() {
  server.send ( 200, "application/json", listStatusJSON() );
}

String listModesJSON() {
  String modes = "[";
  for (uint8_t i = 0; i < strip.getModeCount(); i++) {
    modes += "{\"mode\":";
    modes += i;
    modes += ", \"name\":\"";
    modes += strip.getModeName(i);
    modes += "\"},";
  }
  modes += "{}]";
  return modes;
}

void getModesJSON() {
  server.send ( 200, "application/json", listModesJSON() );
}


// ***************************************************************************
// HTTP request handlers
// ***************************************************************************
void handleMinimalUpload() {
  char temp[1500];

  snprintf ( temp, 1500,
             "<!DOCTYPE html>\
    <html>\
      <head>\
        <title>ESP8266 Upload</title>\
        <meta charset=\"utf-8\">\
        <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
      </head>\
      <body>\
        <form action=\"/edit\" method=\"post\" enctype=\"multipart/form-data\">\
          <input type=\"file\" name=\"data\">\
          <input type=\"text\" name=\"path\" value=\"/\">\
          <button>Upload</button>\
         </form>\
      </body>\
    </html>"
           );
  server.send ( 200, "text/html", temp );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );
}

// automatic cycling
Ticker autoTicker;
int autoCount = 0;

void autoTick() {
  strip.setColor(autoParams[autoCount][0]);
  strip.setSpeed(convertSpeed((uint8_t)autoParams[autoCount][1]));
  strip.setMode((uint8_t)autoParams[autoCount][2]);
  autoTicker.once((float)autoParams[autoCount][3], autoTick);
  DBG_OUTPUT_PORT.print("autoTick ");
  DBG_OUTPUT_PORT.println(autoCount);
  autoCount++;
  if (autoCount >= (sizeof(autoParams) / sizeof(autoParams[0]))) autoCount = 0;

}

void handleAutoStart() {
strip.setBrightness(brightness);
stripsecs.setBrightness(brightness);
}

void handleAutoStop() {
strip.setBrightness(brightness);
stripsecs.setBrightness(brightness);
}



// ***************************************************************************
// WS request handlers
// ***************************************************************************
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch (type) {
    case WStype_DISCONNECTED:
      DBG_OUTPUT_PORT.printf("WS: [%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        DBG_OUTPUT_PORT.printf("WS: [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");

      }
      break;

    case WStype_TEXT:
      DBG_OUTPUT_PORT.printf("WS: [%u] get Text: %s\n", num, payload);

      // # ==> Set main color
      if (payload[0] == '#') {
        handleSetMainColor(payload);
        webSocket.sendTXT(num, "OK");
      }

      if (payload[0] == 'a') {// we are setting minute color
        handleSaveSettings(payload);
        webSocket.sendTXT(num, "OK");
      }

      if (payload[0] == 'j') {// we are setting minute color
        handleSetMinColor(payload);
        webSocket.sendTXT(num, "OK");
      }

      if (payload[0] == 'k') { // we are setting hour color
        handleSetHourColor(payload);
        webSocket.sendTXT(num, "OK");
      }

      if (payload[0] == 'l') { // we are setting hour number color
        handleSetHourNumberColor(payload);
        webSocket.sendTXT(num, "OK");
      }

      if (payload[0] == 'm') { // we are setting second dot color
        handleSetSecDotColor(payload);
        webSocket.sendTXT(num, "OK");
      }

      if (payload[0] == 'n') { // we are setting sweep color
        handleSetSweepColor(payload);
        webSocket.sendTXT(num, "OK");
      }

      if (payload[0] == 'o') { // we are setting alt color
        handleSetAltColor(payload);
        webSocket.sendTXT(num, "OK");
      }
      if (payload[0] == 'p') { // we are setting main color
        handleSetMainColor(payload);
        webSocket.sendTXT(num, "OK");
      }
      if (payload[0] == 'q') { // we are setting timzone
        handleSetTimezone(payload);
        webSocket.sendTXT(num, "OK");
      }
      // ? ==> Set speed
      if (payload[0] == '?') {
        uint8_t d = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        ws2812fx_speed = constrain(d, 0, 255);
        strip.setSpeed(convertSpeed(ws2812fx_speed));
        DBG_OUTPUT_PORT.printf("WS: Set speed to: [%u]\n", ws2812fx_speed);
        webSocket.sendTXT(num, "OK");
      }

      // % ==> Set brightness
      if (payload[0] == '%') {
        uint8_t b = (uint8_t) strtol((const char *) &payload[1], NULL, 10);
        brightness = ((b >> 0) & 0xFF);
        DBG_OUTPUT_PORT.printf("WS: Set brightness to: [%u]\n", brightness);
        strip.setBrightness(brightness);
        stripsecs.setBrightness(brightness);
        webSocket.sendTXT(num, "OK");
      }

      // * ==> Set main color and light all LEDs (Shortcut)
      if (payload[0] == '*') {
        handleSetAllMode(payload);
        webSocket.sendTXT(num, "OK");
      }

      // ! ==> Set single LED in given color
      if (payload[0] == '!') {
        handleSetSingleLED(payload);
        webSocket.sendTXT(num, "OK");
      }

      // + ==> Set multiple LED in the given colors
      if (payload[0] == '+') {
        handleSetDifferentColors(payload);
        webSocket.sendTXT(num, "OK");
      }

      // + ==> Set range of LEDs in the given color
      if (payload[0] == 'R') {
        handleRangeDifferentColors(payload);
        webSocket.sendTXT(num, "OK");
      }

      // = ==> Activate named mode
      if (payload[0] == '=') {
        // we get mode data
        String str_mode = String((char *) &payload[0]);

        handleSetNamedMode(str_mode);

        DBG_OUTPUT_PORT.printf("Activated mode [%u]!\n", mode);
        webSocket.sendTXT(num, "OK");
      }

      // $ ==> Get status Info.
      if (payload[0] == '$') {
        DBG_OUTPUT_PORT.printf("Get status info.");

        String json = listStatusJSON();
        DBG_OUTPUT_PORT.println(json);
        webSocket.sendTXT(num, json);
      }


      // ~ ==> Get WS2812 modes.
      if (payload[0] == '~') {
        DBG_OUTPUT_PORT.printf("Get WS2812 modes.");

        String json = listModesJSON();
        DBG_OUTPUT_PORT.println(json);
        webSocket.sendTXT(num, json);
      }

      // / ==> Set WS2812 mode.
      if (payload[0] == '/') {
        handleSetWS2812FXMode(payload);
        webSocket.sendTXT(num, "OK");
      }
      // ***************************************************************  Here's the entry point from the web
      // start auto cycling
      if (strcmp((char *)payload, "start") == 0 ) {
        handleAutoStart();
        webSocket.sendTXT(num, "OK");
      }

      // stop auto cycling
      if (strcmp((char *)payload, "stop") == 0 ) {
        handleAutoStop();
        webSocket.sendTXT(num, "OK");
      }

      if (strcmp((char *)payload, "sechandon") == 0 ) {
        secondhandstate = 1;
        webSocket.sendTXT(num, "OK");
      }

      // stop auto cycling
      if (strcmp((char *)payload, "sechandoff") == 0 ) {
        secondhandstate = 0;
        webSocket.sendTXT(num, "OK");
      }

      if (strcmp((char *)payload, "daylighton") == 0 ) {
        dayflag = 1;
        webSocket.sendTXT(num, "OK");
      }

      if (strcmp((char *)payload, "daylightoff") == 0 ) {
        dayflag = 0;
        webSocket.sendTXT(num, "OK");
      }

      if (strcmp((char *)payload, "arcon") == 0 ) {
        arcflag = 1;
        webSocket.sendTXT(num, "OK");
      }
      if (strcmp((char *)payload, "arcoff") == 0 ) {
        arcflag = 0;
        webSocket.sendTXT(num, "OK");
      }

      if (strcmp((char *)payload, "dimon") == 0 ) {
        dimflag = 1;
        webSocket.sendTXT(num, "OK");
      }
      if (strcmp((char *)payload, "dimoff") == 0 ) {
        dimflag = 0;
        webSocket.sendTXT(num, "OK");
      }

     if (strcmp((char *)payload, "getvariabledata") == 0 ) {   // initial variable data routine
        webSocket.sendTXT(num, "test");
        
      }
      break;
  }
}

void checkForRequests() {
  webSocket.loop();
  server.handleClient();
}


