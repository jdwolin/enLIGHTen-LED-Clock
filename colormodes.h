float accvar = 200;
int nestloop = 0;
int directionflag = 0;
int arccounter = 0;
int negledinc = 0;
int tailinc =0;
int ledskip = 0;
int ledoffset = 0;
int dipInterval = 10;
int darkTime = 250;
int firstsecond = 1;
long previousMillis = 0;        // will store last time LED was updated
long interval = 0;           // interval at which to blink (milliseconds)
int ledState = LOW;
int sinwave = 0;   // pendulum sinwave integer
int penduluminc = 30;  // pendulum led incrementor
int swingflag = 1; // toggle pendlum back and forth.
int twitch = 50;
int dipCount = 0;
int analogLevel = 100;
boolean timeToDip = false;
int ledStates[NUMLEDS];
int ledinc = 0, growprogress = 0;
int Red, Green, Blue;
int LEDrmin,LEDgmin,LEDbmin;
int piloop = 0;
int piloop1 = 0;
int piindex = 0;
int intervalone = 400;
 char pi[] = "14159265358979323846264338327950288419716939937510";


// ***************************************************************************
// Clear Clock Routines
// ***************************************************************************
void clearHandsRing() {
  for (byte i = 0; i <= NUMLEDSECS; i++) 
      stripsecs.setPixelColor(i, stripsecs.Color(0, 0, 0));
    }
void clearNumbers() {
      for (byte i = 0; i <= NUMLEDS; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
    }

// ************************************************************************************
// Main pixel movement routine.
// *************************************************************************************

void nextpixels(int startarc = 0, int endarc = 0, int ledindex = 0, int colorred = 0, int colorgreen = 0, int colorblue = 0){ //routine used to advance or decay pixels keeping immunity for h/s/m hands
  
  clearHandsRing();    

  if(startarc == endarc){  // if the values are identical only the dot, no arc
   stripsecs.setPixelColor(ledindex, stripsecs.Color(colorred,colorgreen,colorblue)); 
  }
  else{
  if (startarc<endarc){  // the arc is normal - doesn't wrap
    for(int arc = startarc; arc < endarc; arc=arc+2){
      stripsecs.setPixelColor(arc, stripsecs.Color(sweep_color.red,sweep_color.green,sweep_color.blue)); 
    }
  }
  else{  // the arc passes the 120 pixel mark and wraps

for(int arc = startarc; arc < 122; arc=arc+2){  // draw the first half of the arc
   stripsecs.setPixelColor(arc, stripsecs.Color(sweep_color.red,sweep_color.green,sweep_color.blue)); 
    }

for(int arc = 0; arc < endarc; arc=arc+2){  //draw the second half of the arc
      stripsecs.setPixelColor(arc, stripsecs.Color(sweep_color.red,sweep_color.green,sweep_color.blue)); 
    }
  }
}

  if (arcflag == 1){  // if arcflag is set, draw the arc 
    for(int arc = 0; arc < minute_hand; arc=arc+2){
      stripsecs.setPixelColor(arc, stripsecs.Color(sweep_color.red,sweep_color.green,sweep_color.blue)); 
    }
  }

if(ledindex>0){  //set a specific pixel
  stripsecs.setPixelColor(ledindex, stripsecs.Color(colorred,colorgreen,colorblue)); 
}

// now draw the hands
 
stripsecs.setPixelColor(minute_hand, stripsecs.Color(min_color.red,min_color.green,min_color.blue));
if(secondhandstate == 1){
stripsecs.setPixelColor(second_hand, stripsecs.Color(sec_color.red,sec_color.green,sec_color.blue));
}


stripsecs.show();  //show it  


if(hourchanged == 1 ){  // if the hour event has changed, update the numbers
  hourchanged = 0;
  clearNumbers();
     Serial.printf("hour_hand:%d \n", hour_hand); 
   //   Serial.printf("hournum_color:%d \n",);     
      if (hour_hand == 5*2) {  //one oclock
        strip.sendtime(-1,4,hournum_color.red,hournum_color.green,hournum_color.blue);
      }
      if (hour_hand == 10*2) {  //two oclock
        strip.sendtime(3,12,hournum_color.red,hournum_color.green,hournum_color.blue);
      }
      if (hour_hand == 15*2) { //three

        strip.sendtime(11,21,hournum_color.red,hournum_color.green,hournum_color.blue);
      }
      if (hour_hand == 20*2) {  //four
        strip.sendtime(20,28,hournum_color.red,hournum_color.green,hournum_color.blue);
      }
      if (hour_hand == 25*2) { // five
        strip.sendtime(27,37,hournum_color.red,hournum_color.green,hournum_color.blue);
      }
      if (hour_hand == 30*2) { //six
        strip.sendtime(36,46,hournum_color.red,hournum_color.green,hournum_color.blue);
      } 
      if (hour_hand == 35*2) {  //seven
        strip.sendtime(45,51,hournum_color.red,hournum_color.green,hournum_color.blue);
      }
      if (hour_hand == 40*2) {  //eight
        strip.sendtime(50,60,hournum_color.red,hournum_color.green,hournum_color.blue);
      }
      if (hour_hand == 45*2) {  //nine
        strip.sendtime(59,68,hournum_color.red,hournum_color.green,hournum_color.blue);
      }
      if (hour_hand == 50*2) { //ten
        strip.sendtime(67,80,hournum_color.red,hournum_color.green,hournum_color.blue);
      }
      if (hour_hand == 55*2) {  //eleven
        strip.sendtime(79,88,hournum_color.red,hournum_color.green,hournum_color.blue);
      }
      if (hour_hand == 60*2) { //twelve
        strip.sendtime(87,100,hournum_color.red,hournum_color.green,hour_color.blue);
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
 stripsecs.setPixelColor(lednumber,strip.Color(alt_color.red,alt_color.green,alt_color.blue));
}
}

if (directionflag == 0){
for(int x=0; x<12; x++){
 int lednumber = x*10-2+tailinc;
 stripsecs.setPixelColor(lednumber,strip.Color(0,0,0));
  }
}

  stripsecs.show();
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

void startup(){
 accvar = 50; // reset acceleration variable
 clearHandsRing();
 clearNumbers();
 ledinc = -2;
 for(int y=0; y<122; ++y){
  ledinc++;
  stripsecs.setPixelColor(ledinc, stripsecs.Color(0,0,200));
  stripsecs.show();
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
    stripsecs.setPixelColor(i, stripsecs.Color(0, 0, z));
  }

  stripsecs.show();
  delay(2);

}
clearHandsRing();
hourchanged = 1;
minchanged = 1;
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

  for (int i = 0; i < NUMLEDS; i++)
  {
    uint16_t index = (i % 3 == 0) ? 400 : random(0, 767);
    hsb2rgbAN1(index, 200, ledStates[i], i);
  }
  strip.show();
}

static void temp(uint16_t t, uint16_t high, uint16_t low) {
  Red = 0;   //reset the colors
  Blue = 255;
  clearHandsRing();
  for (uint16_t i = 0; i < t; i++) { // Begin with Full Blue and Begin to Fade to full red
    strip.setPixelColor(i, Red = Red + 4, 0, Blue = Blue - 4); // increment by 4
    strip.show();
    delay(25);
  }

  clearHandsRing();
  strip.setPixelColor(t, 0, 255, 0); //set the target temperature green
  strip.show();
  int incr = t + 1;
  int incrminus = t - 1;
  int limitlow = t - low;
  int limithigh = high - t;
  int limit = 0;

  if (limitlow > limithigh) { //find out the greater delta with respect to target temp
    limit = limitlow;
  }
  else {
    limit = limithigh;
  }
  for (uint16_t i = 0; i < limit; i++) { //use that delta as the end parameter in the loop.
    if (limitlow > i) {
      strip.setPixelColor(incr++, 255, 0, 0); // if we haven't reached the low keep drawing
      strip.show();
      delay(50);
    }
    if (limithigh > i) {
      strip.setPixelColor(incrminus--, 0, 0, 255); // if we haven't reached the high keep drawing
      strip.show();
      delay(50);
    }
  }
  delay(1500);
  clearHandsRing();
}

void weather() {
  temp(30, 35, 5);
}


void eventcall() {
  weather();
}


// See: http://forum.mysensors.org/topic/85/phoneytv-for-vera-is-here/13


void one() {         // LED's that make 1 o'clock
for (byte i=0; i<=3;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
  strip.show();
}
}

void two() {         // LED's that make 2 o'clock
for (byte i=4; i<=11;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}
strip.show();

}

void three() {         // LED's that make 3 o'clock
for (byte i=12; i<=20;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}
strip.show();

}

void four() {         // LED's that make 4 o'clock
for (byte i=21; i<=27;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}
strip.show();

}

void five() {         // LED's that make 5 o'clock
for (byte i=28; i<=36;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}
strip.show();

}

void six() {         // LED's that make 6 o'clock
for (byte i=37; i<=45;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}
strip.show();

}
void seven() {         // LED's that make 7 o'clock
for (byte i=46; i<=50;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));
}
strip.show();
}

void eight() {         // LED's that make 8 o'clock
for (byte i=51; i<=59;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}
strip.show();

}
void nine() {         // LED's that make 9 o'clock
for (byte i=60; i<=67;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}
strip.show();

}

void ten() {         // LED's that make 10 o'clock
for (byte i=68; i<=79;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}

strip.show();
}

void eleven() {         // LED's that make 11 o'clock
for (byte i=80; i<=87;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}
strip.show();    
}

void twelve() {         // LED's that make 12 o'clock
for (byte i=88; i<=99;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}
strip.show();

}

void zero(){         // LED's that make 10 o'clock
for (byte i=72; i<=79;i++) {
  strip.setPixelColor(i,strip.Color(main_color.red,main_color.green,main_color.blue));  
}
strip.show();
}

void dot(){
  for(int x=1; x<12; x++){
    stripsecs.setPixelColor(x*10-2,strip.Color(main_color.red,main_color.green,main_color.blue));
  }
  stripsecs.show();
}

void tv() {
unsigned long currentMillis = millis();
if(currentMillis - previousMillis > intervalone) {
    previousMillis = currentMillis;   
if(piloop == 0){
      strip.setColor(0, 0, 0);
      strip.setMode(FX_MODE_STATIC);
      mode = HOLD;
three();
delay(500);
dot();
delay(500);
piloop = 1;
piindex = 0;
}
else{
piindex = piindex + 1;
if(piindex < 50){

if(pi[piindex]=='0'){
  zero();
}
if(pi[piindex]=='1'){
  one();
}
if(pi[piindex]=='2'){
  two();
}
if(pi[piindex]=='3'){
  three();
}
if(pi[piindex]=='4'){
  four();
}
if(pi[piindex]=='5'){
  five();
}
if(pi[piindex]=='6'){
  six();
}
if(pi[piindex]=='7'){
  seven();
}
if(pi[piindex]=='8'){
  eight();
}
if(pi[piindex]=='9'){
  nine();
}
}
else{
  piindex = 0;
  piloop = 0;

}
}
}

}
