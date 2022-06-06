#include <TM1637.h>

//Pin Definitions
const byte rotEnc1 = 3;
const byte rotEnc2 = 4;
const byte hallEff = 2;
const byte sol1 = 5;
const byte sol2 = 6;
const byte CLK = 7;
const byte DIO = 8;
const byte pot = A0;
TM1637 tm(CLK,DIO);

const int flywheelTeeth = 48;

volatile int count = 0;
unsigned long oldTime = 0;
unsigned long newTime = 0;
volatile bool valid = true;
volatile bool printnow = true;
volatile bool forward = true;
volatile bool rot = false;
void setup() {
  pinMode(sol1, OUTPUT);
  pinMode(sol2, OUTPUT);
  digitalWrite(sol1, LOW);
  digitalWrite(sol2, LOW);
  pinMode(rotEnc1, INPUT_PULLUP);
  pinMode(rotEnc2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(rotEnc1), updatePos, CHANGE);
  attachInterrupt(digitalPinToInterrupt(hallEff), rotComplete, FALLING);
  Serial.begin(9600);
  tm.init();
  tm.set(2);
  displayNumber(6969);
  
  while (!rot) {
    delay(10);
  }
}

void loop() {
  displayNumber(analogRead(A0));
  Serial.println(analogRead(A0));
  if (count >= flywheelTeeth) {
    count = count - flywheelTeeth;
    newTime = millis();
    Serial.println(newTime - oldTime);
    oldTime = newTime;
  }
  if (count < 0) {
    count += flywheelTeeth;
    newTime = millis();
    Serial.println(newTime - oldTime);
    oldTime = newTime;

  }
  if (printnow) {
    //Serial.println(count);
    printnow = false;
  }

  if (count > 33 && count < 37) {
  //if (count > 14 && count < 37) {
    digitalWrite(sol1, HIGH);
    //Serial.println("ON 1");
  } else {
    digitalWrite(sol1, LOW);
  }
  if (count > 8 && count < 12) {
  //if (count > 0 && count < 12) {
    digitalWrite(sol2, HIGH);
    //Serial.println("ON 2");
  } else {
    digitalWrite(sol2, LOW);
  }
  if (rot) {
    rot = false;
    if (forward) {
      if (count != 44) {
        Serial.println("Somethings wrong!");
      }
      count = 44;
    } else {
      if (count != 46) {
        Serial.println("Somethings wrong!");
      }
      count = 46;
    }
  }
}

void updatePos() {
  if (digitalRead(rotEnc1)) {
    if (digitalRead(rotEnc2)) {
      count++;
      forward = true;
    } else {
      count--;
      forward = false;
    }
    valid = false;
  } else {
    if (digitalRead(rotEnc2)) {
      count--;
      forward = true;
    } else {
      count++;
      forward = false;
    }
    valid = false;
  }
  printnow = true;
}

void rotComplete() {
  rot = true;
}

void displayNumber(int num){   
    tm.display(3, num % 10);   
    tm.display(2, num / 10 % 10);   
    tm.display(1, num / 100 % 10);   
    tm.display(0, num / 1000 % 10);
}

