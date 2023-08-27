#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include <PololuMaestro.h>
#include <SoftwareSerial.h>


// Pin 2 => periscope LED
// Pin 3 => rigth and Center LED
// Pin 4 => left Eye
// Pin 7, 8 => RX to pololu maestro


// Format of the message M:1:100
// M => Mode could be B (Button) M (Mode) P (Potenciometer)
// 1 => button 0 (SWA) 1 (SWB) 2 (SWC) 3 (SWD) 4 (VRA) 5 (VRB)
// 100 => value of the element 


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

#define NUM_LEDS    1
#define PERISCOPE_LED 2
#define RIGTH_CENTER_EYE 3
#define LEFT_EYE 4
#define LADER_LIGTHS 5
#define NUMPIXELS_RIGTH_CENTER 14
#define NUMPIXELS_LEFT 1
#define NUMPIXELS_LADER_LIGTH 11

SoftwareSerial maestroSerial(7,8);
MiniMaestro maestro(maestroSerial);

enum Eye {
  LEFT_DROID_EYE,
  CENTER_DROID_EYE,
  RIGHT_DROID_EYE,
  LADER
};

class ChopperServoController {
  public: 
    void startHeadAnimation(String message){
      int* messageValue = getBodyMessage(message);
      if(previousValues[messageValue[0]] != messageValue[1]){
         startAnimation(messageValue[0], messageValue[1]);
         previousValues[messageValue[0]] = messageValue[1];
      }  
      delete[] messageValue;
    }

    ChopperServoController(){
      int rcvPreviousValues[] = {101, 0, -100, 0, 94, 94};
      int rcvAnimations[] = {0, 1, 2, 4, 3};
      for(int i = 0 ; i < 6 ; i++){
        previousValues[i] = rcvPreviousValues[i];
      }
      for(int i = 0 ; i < 5 ; i++){
        animations[i] = rcvAnimations[i];
      }
    }

    void setServoMode(int mode){
      currentMode_ = mode;
    }
     
  private:
    int previousValues[6];
    int animations[18];
    int currentMode_ = 0;

    void startAnimation(int button, int value){
      int animation = (currentMode_*6) + (button * 2);
      if(value > 0 ){
        animation = animation + 1;
      }
      maestro.restartScript(animations[value]);

    }
    int* getBodyMessage(String value){
      int * arr = new int[2];
      int indexSeparator = value.indexOf(':');
      arr[0] = value.substring(0, indexSeparator).toInt();
      Serial.println(value.substring(0, indexSeparator));
      Serial.println(value.substring(indexSeparator+1, value.length()));
      arr[1] = value.substring(indexSeparator+1, value.length()).toInt();
      return arr;
    }
};

class ChopperLigths {
  public:
    ChopperLigths(){
      centerEye_ = Adafruit_NeoPixel(NUMPIXELS_RIGTH_CENTER, RIGTH_CENTER_EYE,  NEO_RGBW);
      leftEye_ = Adafruit_NeoPixel(NUMPIXELS_LEFT, LEFT_EYE,  NEO_GRB);
      laderLigth_ = Adafruit_NeoPixel(NUMPIXELS_LADER_LIGTH, LADER_LIGTHS, NEO_GRB + NEO_KHZ800);
      for(int i = 0 ; i < 3 ; i++){
        animationsTime_[i] = millis();
      }
    }
    void begin(){
      centerEye_.begin();
      leftEye_.begin();
      laderLigth_.begin();
      pinMode(PERISCOPE_LED, OUTPUT);
      digitalWrite(PERISCOPE_LED, LOW);
    }

    void setAnimation(int animation){
      animation_ = animation;
    }
    void animate(){
      updateLaderLigth();
      switch(animation_){
        case 0:
          standByAnimation();
        break;
      }
    }
  private: 
    Adafruit_NeoPixel centerEye_;
    Adafruit_NeoPixel leftEye_;
    Adafruit_NeoPixel laderLigth_;
    bool laderBottomDirection_ = true;
    bool standByRigthTurnOn_ = true;
    int periscopeLigth_;
    int animation_ = 0;
    int currentLaderLigth_ = 0;
    long animationsTime_[3];
    

    void standByAnimation(){
      long currentTime = millis();
      if(animationsTime_[0] == 0 || (currentTime - animationsTime_[0]) >= 50){
        setColorEye(LEFT_DROID_EYE, centerEye_.Color(0, 0, 40));
        setColorEye(CENTER_DROID_EYE, centerEye_.Color(0, 0, 40));
        setColorEye(RIGHT_DROID_EYE, centerEye_.Color(255, 0, 0));
        updateRandomPixel(0, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(0, 0, 100));
        updateRandomPixel(0, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(0, 0, 100));
        updateRandomPixel(0, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(0, 0, 100));
        updateRandomPixel(NUMPIXELS_RIGTH_CENTER / 2, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(0, 0, 100));
        updateRandomPixel(NUMPIXELS_RIGTH_CENTER / 2, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(0, 0, 100));
        updateRandomPixel(NUMPIXELS_RIGTH_CENTER / 2, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(0, 0, 100));
        animationsTime_[0] = currentTime;
      }
    }
    void angryAnimation(){
       long currentTime = millis();
      if(animationsTime_[0] == 0 || (currentTime - animationsTime_[0]) >= 50){
        setColorEye(LEFT_DROID_EYE, centerEye_.Color(40, 0, 0));
        setColorEye(CENTER_DROID_EYE, centerEye_.Color(40, 0, 0));
        setColorEye(RIGHT_DROID_EYE, centerEye_.Color(255, 0, 0));
        updateRandomPixel(0, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(100,0,0));
        updateRandomPixel(0, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(100,0,0));
        updateRandomPixel(0, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(100,0,0));
        updateRandomPixel(NUMPIXELS_RIGTH_CENTER / 2, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(100,0,0));
        updateRandomPixel(NUMPIXELS_RIGTH_CENTER / 2, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(100,0,0));
        updateRandomPixel(NUMPIXELS_RIGTH_CENTER / 2, NUMPIXELS_RIGTH_CENTER, centerEye_, centerEye_.Color(100,0,0));
        animationsTime_[0] = currentTime;
      }
    }

    void leftEyeStandByAnimation(){
      long currentTime = millis();
      if(animationsTime_[1] == 0 || (currentTime - animationsTime_[1]) >= 50){
        int changeStatusRigth = random(1000);
        if(changeStatusRigth < 5){
          standByRigthTurnOn_ = !standByRigthTurnOn_;
        }
        if(standByRigthTurnOn_){
          setColorEye(RIGHT_DROID_EYE,leftEye_.Color(255, 0, 0));
        } else {
          setColorEye(RIGHT_DROID_EYE,leftEye_.Color(0, 0, 0));
        }
        animationsTime_[1] = currentTime;
      }
    }



    void setColorEye(Eye eye, uint32_t color ){
      switch(eye){
        case RIGHT_DROID_EYE:
          changeEyeColor(0, NUMPIXELS_RIGTH_CENTER, centerEye_, color);
        break;
        case CENTER_DROID_EYE:
          changeEyeColor(NUMPIXELS_RIGTH_CENTER / 2, NUMPIXELS_RIGTH_CENTER, centerEye_, color);
        break;
        case LEFT_DROID_EYE:
          changeEyeColor(0, 1, leftEye_, color);
        break;
        case LADER:
          changeEyeColor(0, NUMPIXELS_LADER_LIGTH, laderLigth_, color);
        break;
      }
    }
    void changeEyeColor(int start, int end, Adafruit_NeoPixel eye, uint32_t color ){
      for(int i = start / 2 ; i < end ; i++){
        eye.setPixelColor(i, color);
      }
      eye.show();
    }
    void updateLaderLigth() { 
      long currentTime = millis();
      if(animationsTime_[2] == 0 || (currentTime - animationsTime_[2]) >= 100){
        if(laderBottomDirection_){
          currentLaderLigth_++;
          if(currentLaderLigth_ == NUMPIXELS_LADER_LIGTH){
            laderBottomDirection_ = false;
            currentLaderLigth_ = NUMPIXELS_LADER_LIGTH - 2;
          }
        } else {
          currentLaderLigth_--;
          if(currentLaderLigth_ == -1){
            laderBottomDirection_ = true;
            currentLaderLigth_ = 1;
          }
        }
        setColorEye(LADER, laderLigth_.Color(10, 0, 0));
        laderLigth_.setPixelColor(currentLaderLigth_, laderLigth_.Color(255, 0, 0));
        animationsTime_[2] = currentTime;
        laderLigth_.show();
      }
    }
    void updateRandomPixel(int start, int end, Adafruit_NeoPixel eye, uint32_t color){
      int pixel = random(start, end);
      eye.setPixelColor(pixel, color);
      eye.show();
    }
};



ChopperServoController servoCotroller;
ChopperLigths chopperLigths;


void setup() {
  Serial1.begin(1000000); // initialize serial communication:
  Serial.begin(4800); // initialize serial communication:
  maestroSerial.begin(9600);
  chopperLigths.begin();
}

void loop() {
  if ( Serial1.available()) // Check to see if at least one character is available
  {
    String value = Serial1.readStringUntil('|');
    String* completeValue = getActionValue(value);
    if(value[0] == 'M')
    {
       String* valueOfMode = getActionValue(completeValue[1]);
       int value = valueOfMode[1].toInt();
       Serial.println(value);
       if(value < -50){
          servoCotroller.setServoMode(0);
       } else if(value > 50){
          servoCotroller.setServoMode(2);
       } else {
          servoCotroller.setServoMode(1);
       } 

       delete[] valueOfMode;
    }
    delete[] completeValue;  
  }
  chopperLigths.animate();
}

String* getActionValue(String message){
    String * arr = new String[2];
    int indexSeparator = message.indexOf(':');
    arr[0] = message.substring(0, indexSeparator);
    arr[1] = message.substring(indexSeparator+1, message.length());
    return arr;
}




