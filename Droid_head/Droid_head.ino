#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include <PololuMaestro.h>
#include <SoftwareSerial.h>


// Pin 2 => periscope LED
// Pin 3 => rigth and Center LED
// Pin 4 => left Eye
// Pin 7, 8 => RX to pololu maestro
SoftwareSerial maestroSerial(7,8);

// SWG
// up => Mode 1
// middle => Mode 2
// down => Mode 3

// MODE 1
// Controller Animation 
// SWA up => Hidden arms
// SWA down => Show arms
// SWB up => Down persicope
// SWB down => up Periscope
// VRA => Move periscope X

// Mode 2
// SWA up => Arms animation 1
// SWA down => Arms animation 2
// SWB up => Angry animation 
// SWB Down => Cool animation

// Serial Comunication
// Event 0 => Stand by (current animation continue, left and rith eye on blue, periscope turned of  and left eye in red)
// Event 1 => Periscope search
// Event 2 => take arms outside
// Event 3 => move arms 1 
// Event 4 => move arms 2
// Event 5 => move arms 3
// Event 6 => move arms 4
// Event 7 => Ligths normal
// Event 8 => Ligths angry (With arms movement)
// Event 9 => Ligths questions (with arms movement)

#define NUM_LEDS    1
#define PERISCOPE_LED 2
#define RIGTH_CENTER_EYE 3
#define LEFT_EYE 4
#define LADER_LIGTHS 5
#define NUMPIXELS_RIGTH_CENTER 14
#define NUMPIXELS_RIGTH 1
#define NUMPIXELS_LADER_LIGTH 11

Adafruit_NeoPixel pixels(NUMPIXELS_RIGTH_CENTER, RIGTH_CENTER_EYE,  NEO_RGBW);
Adafruit_NeoPixel pixels_rigth_eye(NUMPIXELS_RIGTH, LEFT_EYE,  NEO_GRB);
Adafruit_NeoPixel lader_ligth(NUMPIXELS_LADER_LIGTH, LADER_LIGTHS, NEO_GRB + NEO_KHZ800);
MiniMaestro maestro(maestroSerial);
int previousValues [] = {101, 0, -100, 0, 94, 94};
int incomingByte = 0; // for incoming serial data
bool justStarted = true;
int currentAnimation = 0;
long laderTime = 0;
long animationTime = 0;
bool isLaderBottom = true;
int currentLaderLigth = 0;
int currentMode = 0;

int targetLowbrigthness = 100;

int leftEyeAnimationCurrentBrigthnes = 255;
int centerEyeAnimationCurrentBrigthnes = 150;
int centerPixelToAnimateOnStandby = random(7);
int leftPixelToAnimateOnStandby = random(7, 14);
int standByAnimationDecreasingCenterBrigtness = true;
int standByAnimationDecreasingLeftBrigtness = false;
bool standByRigthTurnOn = true;
enum Eye {
  LEFT_DROID_EYE,
  CENTER_DROID_EYE,
  RIGHT_DROID_EYE,
  LADER
};

void setup() {
  Serial1.begin(1000000); // initialize serial communication:
  Serial.begin(4800); // initialize serial communication:
  maestroSerial.begin(9600);
  pinMode(PERISCOPE_LED, OUTPUT);
  pixels.begin();
  pixels_rigth_eye.begin();
  lader_ligth.begin();
  pixels.setBrightness(255);
  digitalWrite(PERISCOPE_LED, HIGH);  
}

void loop() {
  if ( Serial1.available()) // Check to see if at least one character is available
  {
      String value = Serial1.readStringUntil('|');
      int* bodyValue = getBodyValue(value);
      if(bodyValue[0] == 0 && previousValues[0] != bodyValue[1]){
        if(bodyValue[1]>0){
            maestro.restartScript(1);
            Serial.println("restart animation 1");
        } else {
            maestro.restartScript(0);
            Serial.println("restart animation 0");

        }
         previousValues[0] = bodyValue[1];
      }
      if(bodyValue[0] == 1 && previousValues[1] != bodyValue[1]){
        if(bodyValue[1]>0){
            maestro.restartScript(4);
            Serial.println("restart animation 1");
        } else {
            maestro.restartScript(2);
            Serial.println("restart animation 0");

        }
        
         previousValues[1] = bodyValue[1];
      }
  }
  if(justStarted){
    //animate motors per first time
    justStarted = false;
  }
  if (Serial.available() > 0) {
    String test = Serial.readStringUntil('\n');
    int newAnimation = test.toInt();
    if(newAnimation != currentAnimation){
      stopAnimation();
      currentAnimation = newAnimation;
    }
    

    // say what you got:
    Serial.print("I received: ");
    Serial.println(test);
  }
  updateLaderLigth();

  switch(currentAnimation){
    case 0:
    standByEyeAnimation();
    break;
    case 1:
    break;
    case 2:
    break;
    case 3:
    break;
    case 4:
    break;
    case 5:
    break;
    case 6:
    break;
    case 7:
    break;
    case 8:
    break;
  }
}

int* getBodyValue(String value){
  int * arr = new int[2];
  int indexSeparator = value.indexOf(':');
  arr[0] = value.substring(0, indexSeparator).toInt();
  Serial.println(value.substring(0, indexSeparator));
  Serial.println(value.substring(indexSeparator+1, value.length()));
  arr[1] = value.substring(indexSeparator+1, value.length()).toInt();
  return arr;
}

void stopAnimation(){
  animationTime = 0;
}

void setColorEye(Eye eye, uint32_t color ){
  switch(eye){
    case LEFT_DROID_EYE:
    for(int i = 0 ; i < NUMPIXELS_RIGTH_CENTER / 2 ; i++){
      pixels.setPixelColor(i, color);
    }
    pixels.show();
    break;
    case CENTER_DROID_EYE:
      for(int i = NUMPIXELS_RIGTH_CENTER / 2 ; i < NUMPIXELS_RIGTH_CENTER  ; i++){
      pixels.setPixelColor(i, color);
    }
    pixels.show();
    break;
    case LADER:
    for(int i = 0 ; i < NUMPIXELS_LADER_LIGTH  ; i++){
      lader_ligth.setPixelColor(i, color);
    }
    lader_ligth.show();
    break;
    default:
      pixels_rigth_eye.setPixelColor(0, color);
      pixels_rigth_eye.show();
    break;
  }
}

void angryAnimation(){
  
}

void standByEyeAnimation(){
  long currentTime = millis();
  if(animationTime == 0 || (currentTime - animationTime) >= 50){
    digitalWrite(PERISCOPE_LED, LOW);
    setColorEye(LEFT_DROID_EYE,pixels.Color(0, 0, 40));
    setColorEye(CENTER_DROID_EYE,pixels.Color(0, 0, 40));
    setColorEye(RIGHT_DROID_EYE,pixels.Color(255, 0, 0));
    int changeStatusRigth = random(1000);
    if(changeStatusRigth < 5){
      standByRigthTurnOn = !standByRigthTurnOn;
    }
    if(standByRigthTurnOn){
      setColorEye(RIGHT_DROID_EYE,pixels.Color(255, 0, 0));
    } else {
      setColorEye(RIGHT_DROID_EYE,pixels.Color(0, 0, 0));
    }
    
    if(standByAnimationDecreasingLeftBrigtness){
       leftEyeAnimationCurrentBrigthnes = leftEyeAnimationCurrentBrigthnes - 3;
       if(leftEyeAnimationCurrentBrigthnes <= targetLowbrigthness){
         leftEyeAnimationCurrentBrigthnes = targetLowbrigthness;
          standByAnimationDecreasingLeftBrigtness = false;
       }
    } else {
      leftEyeAnimationCurrentBrigthnes = leftEyeAnimationCurrentBrigthnes + 3;
      if(leftEyeAnimationCurrentBrigthnes >= 255 ){
        leftPixelToAnimateOnStandby = random(7, 14); 
        standByAnimationDecreasingLeftBrigtness = true;
        leftEyeAnimationCurrentBrigthnes = 255;
      }
    }

    if(standByAnimationDecreasingCenterBrigtness){
       centerEyeAnimationCurrentBrigthnes = centerEyeAnimationCurrentBrigthnes - 3;
       if(centerEyeAnimationCurrentBrigthnes <= targetLowbrigthness){
         centerEyeAnimationCurrentBrigthnes = targetLowbrigthness;
          standByAnimationDecreasingCenterBrigtness = false;
       }
    } else {
      centerEyeAnimationCurrentBrigthnes = centerEyeAnimationCurrentBrigthnes + 3;
      if(centerEyeAnimationCurrentBrigthnes >= 255 ){
        centerPixelToAnimateOnStandby = random(7);
        standByAnimationDecreasingCenterBrigtness = true;
        centerEyeAnimationCurrentBrigthnes = 255;
      }
    }
    pixels.setPixelColor(getAdjasentLigth(centerPixelToAnimateOnStandby, 0, 6, true), pixels.Color(0, 0, centerEyeAnimationCurrentBrigthnes));
    pixels.setPixelColor(centerPixelToAnimateOnStandby, pixels.Color(0, 0, centerEyeAnimationCurrentBrigthnes));
    pixels.setPixelColor(getAdjasentLigth(centerPixelToAnimateOnStandby, 0, 6, false), pixels.Color(0, 0, centerEyeAnimationCurrentBrigthnes));
    pixels.setPixelColor(getAdjasentLigth(leftPixelToAnimateOnStandby, 7, 13, true), pixels.Color(0, 0, leftEyeAnimationCurrentBrigthnes));
    pixels.setPixelColor(leftPixelToAnimateOnStandby, pixels.Color(0, 0, leftEyeAnimationCurrentBrigthnes));
    pixels.setPixelColor(getAdjasentLigth(leftPixelToAnimateOnStandby, 7, 13, false), pixels.Color(0, 0, leftEyeAnimationCurrentBrigthnes));
    pixels.show();
    animationTime = currentTime;
   }

}

int getAdjasentLigth(int currentLigth, int bottomLimit, int topLimit, bool shouldSubstract){
  if(shouldSubstract){
    if(currentLigth >= bottomLimit){
      return topLimit;
    }
    return currentLigth - 1;
  } else {
    if(currentLigth <= topLimit){
      return bottomLimit;
    }
    return currentLigth + 1;
  }
}

void updateLaderLigth() { 
  long currentTime = millis();
  if(laderTime == 0 || (currentTime - laderTime) >= 100){
    if(isLaderBottom){
       currentLaderLigth++;
      if(currentLaderLigth == NUMPIXELS_LADER_LIGTH){
        isLaderBottom = false;
        currentLaderLigth = NUMPIXELS_LADER_LIGTH - 2;
      }
    } else {
      currentLaderLigth--;
      if(currentLaderLigth == -1){
        isLaderBottom = true;
        currentLaderLigth = 1;
      }
    }
    setColorEye(LADER, pixels.Color(10, 0, 0));
    lader_ligth.setPixelColor(currentLaderLigth, pixels.Color(255, 0, 0));
    laderTime = currentTime;
    lader_ligth.show();
  }
}



