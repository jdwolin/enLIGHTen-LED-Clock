// ***************************************************************************
// Color modes
// ***************************************************************************

int dipInterval = 10;
int darkTime = 250;
unsigned long currentDipTime;
unsigned long dipStartTime;
unsigned long currentMillis;
int ledState = LOW;
long previousMillis = 0; 
int led = 5;
int interval = 2000;
int twitch = 50;
int dipCount = 0;
int analogLevel = 100;
boolean timeToDip = false;
int ledStates[NUMLEDS];
float accvar = 200;
int nestloop = 0;
int directionflag = 0;
int arccounter = 0;
int negledinc = 0;
int tailinc =0;
int ledskip = 0;
int ledoffset = 0;
int firstsecond = 1;
int sinwave = 0;   // pendulum sinwave integer
int penduluminc = 30;  // pendulum led incrementor
int swingflag = 1; // toggle pendlum back and forth.
int growprogress = 0;
int Red, Green, Blue;
int LEDrmin,LEDgmin,LEDbmin;
int intervalone = 400;
int ledinc = 0;


void restoresettings(){
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
    strip.setColor(main_color.red, main_color.green, main_color.blue);
  }
sprintf(last_state, "STA|%2d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d|%3d", mode, ws2812fx_mode, ws2812fx_speed, brightness, main_color.red, main_color.green, main_color.blue, hournum_color.red, hournum_color.green, hournum_color.blue, sec_color.red, sec_color.green, sec_color.blue, sweep_color.red, sweep_color.green, sweep_color.blue, min_color.red, min_color.green, min_color.blue, clockmode, dimflag, arcflag, dayflag,timezone);

}


void hsb2rgbAN1(uint16_t index, uint8_t sat, uint8_t bright, uint8_t myled) {
    // Source: https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
    uint8_t temp[5], n = (index >> 8) % 3;
    temp[0] = temp[3] = (uint8_t)((                                         (sat ^ 255)  * bright) / 255);
    temp[1] = temp[4] = (uint8_t)((((( (index & 255)        * sat) / 255) + (sat ^ 255)) * bright) / 255);
    temp[2] =           (uint8_t)(((((((index & 255) ^ 255) * sat) / 255) + (sat ^ 255)) * bright) / 255);

    strip.setPixelColor(myled, temp[n + 2], temp[n + 1], temp[n]);
}


void updateLed (int led, int brightness) {
	ledStates[led] = brightness;
	
	for (int i=0; i<NUMLEDS; i++)
	{
		uint16_t index = (i%3 == 0) ? 400 : random(0,767);
		hsb2rgbAN1(index, 200, ledStates[i], i);
	}
	strip.show();
}


void clearHandsRing() {
  for (byte i = 0; i <= NUMLEDSECS; i++) {
      strip.setPixelColor(i+stripoffset, 0, 0, 0);
      
}
    }
void clearNumbers() {
      for (byte i = 0; i <= NUMLEDS; i++) {
        
        strip.setPixelColor(i, 0, 0, 0);
        
      }
    }


void nextpixels(int startarc = 0, int endarc = 0, int ledindex = 0, int colorred = 0, int colorgreen = 0, int colorblue = 0){ //routine used to advance or decay pixels keeping immunity for h/s/m hands
clearHandsRing();    
if(startarc == endarc){  // if the values are identical only the dot, no arc
   strip.setPixelColor(ledindex+stripoffset,colorred,colorgreen,colorblue); 
  }
  else{
  if (startarc<endarc){  // the arc is normal - doesn't wrap
    for(int arc = startarc; arc < endarc; arc=arc+2){
      
      strip.setPixelColor(arc+stripoffset, sweep_color.red,sweep_color.green,sweep_color.blue); 
    }
  }
  else{  // the arc passes the 120 pixel mark and wraps

for(int arc = startarc; arc < 122; arc=arc+2){  // draw the first half of the arc
   strip.setPixelColor(arc+stripoffset, sweep_color.red,sweep_color.green,sweep_color.blue); 
}

for(int arc = 0; arc < endarc; arc=arc+2){  //draw the second half of the arc
  
      strip.setPixelColor(arc+stripoffset, sweep_color.red,sweep_color.green,sweep_color.blue); 
    }
  }
}

  if (arcflag == 1){  // if arcflag is set, draw the arc 
    for(int arc = 0; arc < minute_hand; arc=arc+2){
      
      strip.setPixelColor(arc+stripoffset, sweep_color.red,sweep_color.green,sweep_color.blue); 
    }
  }

if(ledindex>0){  //set a specific pixel
  
  strip.setPixelColor(ledindex+stripoffset, colorred,colorgreen,colorblue); 
}

// now draw the hands
 
strip.setPixelColor(minute_hand+stripoffset, min_color.red,min_color.green,min_color.blue);
if(secondhandstate == 1){
strip.setPixelColor(second_hand+stripoffset, sec_color.red,sec_color.green,sec_color.blue);
}

strip.show();  //show it  


if(hourchanged == 1 ){  // if the hour event has changed, update the numbers
  hourchanged = 0;
  clearNumbers();
     Serial.printf("hour_hand:%d \n", hour_hand); 
   //   Serial.printf("hournum_color:%d \n",);     
      if (hour_hand == 5*2) {  //one oclock
      hoursmall = -1;
      hourtall =4;
      }
      if (hour_hand == 10*2) {  //two oclock
      hoursmall = 3;
      hourtall = 12;
      }
      if (hour_hand == 15*2) { //three

      hoursmall = 11;
      hourtall = 21;
      }
      if (hour_hand == 20*2) {  //four
      hoursmall = 20;
      hourtall = 28;
      }
      if (hour_hand == 25*2) { // five
      hoursmall = 27;
      hourtall = 37;
      }
      if (hour_hand == 30*2) { //six
      hoursmall = 36;
      hourtall = 46;
      } 
      if (hour_hand == 35*2) {  //seven
      hoursmall = 45;
      hourtall = 51; 
      }
      if (hour_hand == 40*2) {  //eight
      hoursmall = 50;
      hourtall = 60;
      }
      if (hour_hand == 45*2) {  //nine
      hoursmall = 59;
      hourtall = 68;
      }
      if (hour_hand == 50*2) { //ten
      hoursmall = 67;
      hourtall = 80;
      }
      if (hour_hand == 55*2) {  //eleven
      hoursmall = 79;
      hourtall = 88;
      }
      if (hour_hand == 60*2) { //twelve
      hoursmall = 87;
      hourtall = 100;
      }      
    strip.show();  // now show it
  }
}

// *************************************************************************************
// Clock Modes    
// *************************************************************************************

void swinghoop(){
  if (ledinc == 120) { //if we made it around the circle
      ledinc = 0;  // reset ledcounter
      accvar = 200; // reset acceleration variable
      if (rotationcounter == 1){
        rotationcounter = 0;
      }
      else {
        rotationcounter = 1;
      }
    }
    ledinc=ledinc+2;
  if (rotationcounter == 1) { //color alternator  - this one draws color
    nextpixels(0,ledinc);
  }
    else {  // color alternator - this one erases color
     nextpixels(ledinc,120);
   }
    if (ledinc < 60) { // we've got the halfway point.  Now decelarate
    accvar = accvar - (accvar * 0.09);
  }
  else {
    accvar = accvar + (accvar * 0.09);
  }
  delay(round(accvar));
}

void onceaminutetemplate(){
  unsigned long currentMillis = millis();

if (minchanged == 0){
  nextpixels(0, 0);  //draw the current arc
}

    if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;  


   if(tailinc == 60){  //we've finished
    minchanged = 0;
  }

 if(minchanged==1){
 if(minchangedreset==1) {  // initialization routine
     accvar = 250;
     minchangedreset = 0;
     tailinc = 0;
   }

}
// code here

}
}

void spider(){
  unsigned long currentMillis = millis();

if (minchanged == 0){
  nextpixels(0, 0);  //draw the current arc
}
    if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;  

 if(minchanged==1){
 if(minchangedreset==1) {  // initialization routine
     accvar = 200;
     minchangedreset = 0;
     tailinc = 0;
     directionflag = 1;
   }

 if(tailinc == 10){  //we've finished first routine now erase
    directionflag = 0;
  }
   if(tailinc == 20){  //we've finished second routine, reset minchanged.
    minchanged = 0;
    tailinc = 0;
    directionflag = 1;
  }

if (directionflag == 1){
for(int x=0; x<12; x++){
 int lednumber = x*10-2+tailinc;
 strip.setPixelColor(lednumber+stripoffset,alt_color.red,alt_color.green,alt_color.blue);
}
}

if (directionflag == 0){
for(int x=0; x<12; x++){
 int lednumber = x*10-2+tailinc;
 strip.setPixelColor(lednumber+stripoffset,0,0,0);
  }
}

  strip.show();
  tailinc = tailinc + 2;
  accvar = accvar - (accvar * 0.09);  //decrease the delay (ie. accelerate)    
  interval = accvar;  //set the interval

}
}
}
void chasetails(int tails){
  unsigned long currentMillis = millis();

if (minchanged == 0){
  nextpixels(0, 0);  //draw the current arc
}

    if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;  


    if(minchanged==1){
    if(minchangedreset==1) {  // initialization routine
     ledinc = minute_hand;
     negledinc = minute_hand;
     accvar = 200;
     minchangedreset = 0;
     tailinc = 0;
     directionflag = 1;
   }


  if(tailinc == 60){  //we've finished
 directionflag = 0;
  }
   if(tailinc == 119){  //we've finished
    minchanged = 0;
  }
       
        
        tailinc = tailinc + 1;  // loop counter
        
        if(directionflag == 1){
        if(ledinc==120){  //we passed the 0 marker
          ledinc=-2;  //reset the led index
        }
        ledinc = ledinc + 2;          
        }
        else{
        if(ledinc==0){  //we passed the 0 marker
          ledinc=122;  //reset the led index
        }
         ledinc = ledinc - 2;           
        }
 //          Serial.printf("ledinc:%d \n", ledinc);

        if(tails==1){  //if we want tails 
          int endpixel = ledinc + 12;
          if (endpixel > 120){
            endpixel = endpixel - 120;
          }
          nextpixels(ledinc,endpixel);
        }
        else{
          nextpixels(0,0,ledinc,alt_color.red,alt_color.green,alt_color.blue);
          }
        accvar = accvar - (accvar * 0.09);  //decrease the delay (ie. accelerate)    
        interval = accvar;  //set the interval
        
      }

    }
  }



void bumpme(){
unsigned long currentMillis = millis();  // get the current clock time
  if(currentMillis - previousMillis > interval) {  // has it been long enough?
    previousMillis = currentMillis;   // if yes reset the timer
    if (minchanged == 1) {  // are we at a minute event?
        if (minchangedreset == 1){ // if this is the first time entering the loop, reset the key variables.
      growprogress = -2;  // set ledcounter
      accvar = 300; // reset acceleration variable
      minchangedreset = 0; // reset the once a minute timer
      nestloop = 0; // start animation sequence in the first loop
    }


 if (nestloop == 0){ // we are in the first loop
      growprogress = growprogress + 2;  //++
       if (growprogress < 120){   // first pass around the circle
        nextpixels(0,growprogress);
        accvar = accvar - (accvar * 0.09);  //decrease the delay (ie. accelerate)
        interval = round(accvar);  // set interval dely timer
      }
      else{
        nestloop = 1;  
        growprogress = -2;
      }
    }

    if (nestloop == 1){  // now enter the second loop that will end on minute hand
      growprogress = growprogress + 2;
      if (growprogress < minute_hand)
      {
        nextpixels(0,growprogress);
        accvar = accvar - (accvar * 0.09);  //decrease the delay (ie. accelerate)
        interval = round(accvar);
      }
      else{
        nestloop = 2;  // move on to the growing arc loop
        accvar = 20;  //reset the acceleration variable.
        arccounter = 0;
      }
    }

if (nestloop == 2 ){   // now time to grow the arc ascending...
      if (growprogress == 120){
        growprogress = -2;
      }
      growprogress = growprogress + 2;
      arccounter = arccounter + 1;
      if (arccounter < 18)
      {
        nextpixels(0,minute_hand, growprogress, alt_color.red,alt_color.green,alt_color.blue);
        accvar = accvar + (accvar * 0.09);  // increase the delay (ie. deccelerate)
        interval = round(accvar);
      }
      else{
        nestloop = 3;
        arccounter = 0;
      }
    }

if (nestloop == 3 ){   // now time to shrink the arc descending...
      if (growprogress == 0){
        growprogress = 122;
      }
      growprogress = growprogress - 2;
      arccounter = arccounter + 1;
      if (arccounter < 18)
      {
        nextpixels(0,minute_hand, growprogress, alt_color.red,alt_color.green,alt_color.blue);
        accvar = accvar - (accvar * 0.09);  //decrease the delay (ie. accelerate)
        interval = round(accvar);
      }
      else{
        nestloop = 4;
        minchanged = 0;  // animation is complete
      }
    }
  }
}

if (minchanged == 0){
  nextpixels(0, minute_hand);  //draw the current arc
  }
}

void pendulum(){
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   
    if(penduluminc<40){
      swingflag = 1;
      sinwave = 90;
    }

    if(penduluminc>78){
      swingflag = 0;
      sinwave = 270;
    }

    if(swingflag == 1){
      penduluminc = penduluminc + 2;
      sinwave = sinwave + 9;
      nextpixels(penduluminc-2,penduluminc+2,penduluminc,alt_color.red,alt_color.green,alt_color.blue);         
        accvar = abs(sin(sinwave/57.3));  // convert from radians to degrees
        interval = round(accvar*35);
        // Serial.printf("descent:%d \n", interval); 
 }
 if(swingflag == 0){
  penduluminc = penduluminc - 2;
  sinwave = sinwave - 9;
  nextpixels(penduluminc-2,penduluminc+2,penduluminc,alt_color.red,alt_color.green,alt_color.blue);      
      accvar = abs(sin(sinwave/57.3));  // convert from radians to degrees
      interval = round(accvar*35);
       // Serial.printf("ascent:%d \n", interval); 
  }

}
}




// See: http://forum.mysensors.org/topic/85/phoneytv-for-vera-is-here/13
void tv() {
	checkForRequests();
	if (exit_func) {
		exit_func = false;
		return;
	}
	
	if (timeToDip == false)
	{
		currentMillis = millis();
		if(currentMillis-previousMillis > interval) 
		{
			previousMillis = currentMillis;
			interval = random(750,4001);//Adjusts the interval for more/less frequent random light changes
			twitch = random(40,100);// Twitch provides motion effect but can be a bit much if too high
			dipCount = dipCount++;
		}
		if(currentMillis-previousMillis<twitch)
		{
			led=random(0, (strip.numPixels()-1));
			analogLevel=random(50,255);// set the range of the 3 pwm leds
			ledState = ledState == LOW ? HIGH: LOW; // if the LED is off turn it on and vice-versa:
			
			updateLed(led, (ledState) ? 255 : 0);
			
			if (dipCount > dipInterval)
			{ 
				DBG_OUTPUT_PORT.println("dip");
				timeToDip = true;
				dipCount = 0;
				dipStartTime = millis();
				darkTime = random(50,150);
				dipInterval = random(5,250);// cycles of flicker
			}
			//strip.show();
		} 
	}
	else
	{
		DBG_OUTPUT_PORT.println("Dip Time");
		currentDipTime = millis();
		if (currentDipTime - dipStartTime < darkTime)
		{
			for (int i=3; i<strip.numPixels(); i++)
			{
				updateLed(i, 0);
			}
		}
		else
		{
			timeToDip = false;
		}
		strip.show();
	}
}



time_t compileTime()
{
    const time_t FUDGE(10);     // fudge factor to allow for compile time (seconds, YMMV)
    const char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char chMon[3], *m;
    tmElements_t tm;

    strncpy(chMon, compDate, 3);
    chMon[3] = '\0';
    m = strstr(months, chMon);
    tm.Month = ((m - months) / 3 + 1);

    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);
    time_t t = makeTime(tm);
    return t + FUDGE;           // add fudge factor to allow for compile time
    
}

// format and print a time_t value, with a time zone appended.
void printDateTime(time_t t, const char *tz)
{
    char buf[32];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
        hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
    Serial.println(buf);
}


void startup(){
accvar = 50; // reset acceleration variable
clearHandsRing();
clearNumbers();
int ledinc = 0;
 for(int y=0; y<122; ++y){
  ledinc++;
  strip.setPixelColor(ledinc+stripoffset, 0,0,200);
  strip.show();
    if (ledinc < 60) { // we've got the halfway point.  Now decelarate
    accvar = accvar - (accvar * 0.09);
  }
  else {
    accvar = accvar + (accvar * 0.09);
  }
  delay(round(accvar));
} 

for(int z=200; z > 0; z=z-1){
  for (byte i = 0; i <= NUMLEDSECS; i++) {
    strip.setPixelColor(i+stripoffset, 0, 0, z);
  }
  strip.show();


}
clearHandsRing();
hourchanged = 1;
minchanged = 1;
}



void one() {         // LED's that make 1 o'clock
for (byte i=0; i<=3;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
  
  strip.show();
}
}

void two() {         // LED's that make 2 o'clock
for (byte i=4; i<=11;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();

}

void three() {         // LED's that make 3 o'clock
for (byte i=12; i<=20;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();

}

void four() {         // LED's that make 4 o'clock
for (byte i=21; i<=27;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();

}

void five() {         // LED's that make 5 o'clock
for (byte i=28; i<=36;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();

}

void six() {         // LED's that make 6 o'clock
for (byte i=37; i<=45;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();

}
void seven() {         // LED's that make 7 o'clock
for (byte i=46; i<=50;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);
}

strip.show();
}

void eight() {         // LED's that make 8 o'clock
for (byte i=51; i<=59;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();

}
void nine() {         // LED's that make 9 o'clock
for (byte i=60; i<=67;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();

}

void ten() {         // LED's that make 10 o'clock
for (byte i=68; i<=79;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();
}

void eleven() {         // LED's that make 11 o'clock
for (byte i=80; i<=87;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();    
}

void twelve() {         // LED's that make 12 o'clock
for (byte i=88; i<=99;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();

}

void zero(){         // LED's that make 10 o'clock
for (byte i=72; i<=79;i++) {
  strip.setPixelColor(i,main_color.red,main_color.green,main_color.blue);  
}

strip.show();
}

void dot(){
  for(int x=1; x<12; x++){
    strip.setPixelColor(x*10-2+stripoffset,main_color.red,main_color.green,main_color.blue);
  }
  
strip.show();
}



