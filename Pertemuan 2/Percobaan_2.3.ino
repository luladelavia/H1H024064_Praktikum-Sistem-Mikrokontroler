#include <Arduino.h>
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};

const int buttonUp = 3;   
const int buttonDown = 2; 
int counter = 0;

bool lastUpState = HIGH;
bool lastDownState = HIGH;

byte digitPattern[16][8] = {
  {1,1,1,1,1,1,0,0}, //0
  {0,1,1,0,0,0,0,0}, //1
  {1,1,0,1,1,0,1,0}, //2
  {1,1,1,1,0,0,1,0}, //3
  {0,1,1,0,0,1,1,0}, //4
  {1,0,1,1,0,1,1,0}, //5 
  {1,0,1,1,1,1,1,0}, //6
  {1,1,1,0,0,0,0,0}, //7
  {1,1,1,1,1,1,1,0}, //8
  {1,1,1,1,0,1,1,0}, //9
  {1,1,1,0,1,1,1,0}, //A
  {0,0,1,1,1,1,1,0}, //b
  {1,0,0,1,1,1,0,0}, //C
  {0,1,1,1,1,0,1,0}, //d
  {1,0,0,1,1,1,1,0}, //E
  {1,0,0,0,1,1,1,0}  //F
};

void displayDigit(int num) {
  for(int i=0; i<8; i++) {
    digitalWrite(segmentPins[i], !digitPattern[num][i]);
  }
}

void setup() {
  for(int i=0; i<8; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }

  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);

  displayDigit(counter);
}

void loop() {
  bool currentUpState = digitalRead(buttonUp);
  bool currentDownState = digitalRead(buttonDown);


  if (lastUpState == HIGH && currentUpState == LOW) {
    counter++;
    if(counter > 15) counter = 0; 
    displayDigit(counter);
    delay(200); 
  }

  if (lastDownState == HIGH && currentDownState == LOW) {
    counter--;
    if(counter < 0) counter = 15; 
    displayDigit(counter);
    delay(200); 
  }

  lastUpState = currentUpState;
  lastDownState = currentDownState;
}