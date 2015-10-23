
#include "UnoJoy.h"

boolean usePPM = true;

#define RC_CHANS 5
volatile uint16_t rcValue[RC_CHANS] = {1502, 1502, 1502, 1502, 1502};

#define LED_PIN 13
byte led = 1;
int led_cnt = 0;


const bool testAutoSendMode = false;

enum {
  THROTTLE,
  ROLL,
  PITCH,  
  YAW,
  AUX1
};

void setup(){
  setupPins();
  //setupUnoJoy();
  if (testAutoSendMode)
  {
    Joystick.begin();
  }
  else
  {
    Joystick.begin(false);
  }
}

void loop(){
  // Always be getting fresh data
  dataForController_t controllerData = getControllerData();
  setControllerDataJoystick(controllerData);
  Joystick.sendState();

  Serial.print(controllerData.rightStickX); Serial.print("     ");
  Serial.print(controllerData.rightStickY);      Serial.print("     ");
  Serial.print(controllerData.leftStickX);    Serial.print("     ");
  Serial.print(controllerData.leftStickY);     Serial.println("     ");

  led_cnt=led_cnt++;
  if(led_cnt>200){
    led=!led;
    digitalWrite(LED_PIN,led);
    led_cnt=0;
  }
  
  delay(5);
}

void setControllerDataJoystick(dataForController_t dataToSet){
  Joystick.setXAxis(dataToSet.rightStickX-127);
  Joystick.setYAxis(dataToSet.rightStickY-127);
  Joystick.setXAxisRotation(dataToSet.leftStickX);
  Joystick.setZAxis(dataToSet.leftStickY-127);
}

void setupPins(void){
  // Set all the digital pins as inputs
  // with the pull-up enabled, except for the 
  // two serial line pins
  for (int i = 2; i <= 12; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }
  pinMode(A4, INPUT);
  digitalWrite(A4, HIGH);
  pinMode(A5, INPUT);
  digitalWrite(A5, HIGH);
  
  if ( usePPM ) 
    attachInterrupt(digitalPinToInterrupt(0), rxInt, RISING);

  pinMode(LED_PIN,OUTPUT);
}

dataForController_t getControllerData(void){
  
  // Set up a place for our controller data
  //  Use the getBlankDataForController() function, since
  //  just declaring a fresh dataForController_t tends
  //  to get you one filled with junk from other, random
  //  values that were in those memory locations before
  dataForController_t controllerData = getBlankDataForController();
  // Since our buttons are all held high and
  //  pulled low when pressed, we use the "!"
  //  operator to invert the readings from the pins
  if ( !usePPM )
    controllerData.triangleOn = !digitalRead(2); //pin 2 is used for PPM input
  controllerData.circleOn = !digitalRead(3);
  controllerData.squareOn = !digitalRead(4);
  controllerData.crossOn = !digitalRead(5);
  controllerData.dpadUpOn = !digitalRead(6);
  controllerData.dpadDownOn = !digitalRead(7);
  controllerData.dpadLeftOn = !digitalRead(8);
  controllerData.dpadRightOn = !digitalRead(9);
  controllerData.l1On = !digitalRead(10);
  controllerData.r1On = !digitalRead(11);
  controllerData.selectOn = !digitalRead(12);
  controllerData.startOn = !digitalRead(A4);
  controllerData.homeOn = !digitalRead(A5);
  
  // Set the analog sticks
  //  Since analogRead(pin) returns a 10 bit value,
  //  we need to perform a bit shift operation to
  //  lose the 2 least significant bits and get an
  //  8 bit number that we can use  
  controllerData.leftStickX = analogRead(A0) >> 2;
  controllerData.leftStickY = analogRead(A1) >> 2;
  controllerData.rightStickX = analogRead(A2) >> 2;
  controllerData.rightStickY = analogRead(A3) >> 2;
  
  if ( usePPM ) {
    // use AUX1 switch to press the triangle button
    controllerData.triangleOn = rcValue[AUX1] > 1500;
    
    // use stick positions from TX instead of analogReads
    controllerData.leftStickX = stickValue(rcValue[YAW]);
    controllerData.leftStickY = stickValue(rcValue[THROTTLE]);
    controllerData.rightStickX = stickValue(rcValue[ROLL]);
    controllerData.rightStickY = stickValue(rcValue[PITCH]);
  }
  
  // And return the data!
  return controllerData;
}

////////////////////////////////////////////////////////////////////////////////////

// Convert a value in the range 1000-2000 to 0-255
byte stickValue(int rcVal) {
  return map( constrain(rcVal - 1000, 0, 1000), 0, 1000, 0, 255);
}

// Some adjustments I used because my cheapass TX has no trims.
// If your TX has trims you will not need this and you can simply
// return the 'diff' value unchanged.
uint16_t adjust(uint16_t diff, uint8_t chan) {
  /*switch (chan) {
    case THROTTLE: return diff + 50;
    case YAW:      return diff + 60;
    case PITCH:    return diff + 60;
    case ROLL:     return diff + 90;
    case AUX1:     return diff + 10;
  }*/
  return diff;
}

// PPM interrupt routine courtesy of MultiWii source code
void rxInt(void) {
  uint16_t now,diff;
  static uint16_t last = 0;
  static uint8_t chan = 0;

  now = micros();
  sei();
  diff = now - last;
  last = now;
  if(diff>3000) chan = 0;
  else {
    if(900<diff && diff<2200 && chan<RC_CHANS ) {
      rcValue[chan] = adjust(diff,chan);
    }
    chan++;
  }
}

