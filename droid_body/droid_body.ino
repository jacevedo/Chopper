#include <SoftwareSerial.h>

// Define Input Connections
#define CH3 53
#define CH4 51
#define CH5 49
#define CH6 47
#define CH7 45
#define CH8 43
#define CH9 41
#define CH10 39

int pinsData [] = {49, 47, 45, 43, 41, 39};
int previousValues [] = {0, 0, -100, 0, 94, 94};
String stringName [] = {"0", "1", "2", "3", "4", "5"};
int differenceArray = 2;

int ch3Value;
int ch4Value;
bool ch5Value;
bool ch6Value;
int ch7Value;
bool ch8Value;
int ch9Value;
int ch10Value;


// Read the number of a specified channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue){
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}
 

// Read the switch channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue){
  int intDefaultValue = (defaultValue)? 100: 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}

void setup() {
  // Set up serial monitor
  Serial.begin(9600);
  Serial1.begin(1000000);

  // Set all pins as inputs
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);
  pinMode(CH7, INPUT);
  pinMode(CH8, INPUT);
  pinMode(CH9, INPUT);
  pinMode(CH10, INPUT);

}

void loop() {
  for(int i = 0 ; i < sizeof(pinsData) / sizeof(pinsData[0]); i++ ){
    int newValue = readChannel(pinsData[i], -100, 100, 0);
    if(newValue < previousValues[i] - 5 || newValue > previousValues[i] + 5) {
      previousValues[i] = newValue;
      String valueUpdate = String( stringName[i] + ':' + newValue + '|');
      Serial.print("need to send: ");
      Serial.println(valueUpdate.c_str());
      Serial1.write(valueUpdate.c_str());
      Serial1.flush();
      //delay(1000);
    }
  }
}
