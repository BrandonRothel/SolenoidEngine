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
TM1637 tm(CLK, DIO);

const int flywheelTeeth = 48;

volatile int count = 0;
unsigned long oldTime = 0;
unsigned long newTime = 0;
volatile bool valid = true;
volatile bool printnow = true;
volatile bool forward = true;
volatile bool rot = false;
const int arraySize = 5;
int rotationNumber = 0;
int rpmArray[arraySize];
int desiredRPM = 200;
int avgRPM = 0;
int sol1ToothPower = 40;
int sol2ToothPower = 15;
int solPowerWidth = 5;

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
  tm.set(1);
  for (int i = 0; i < arraySize; i++) {
    rpmArray[i] = 0;
  }
  displayNumber(6969);
  while (!rot) {
    delay(10);
  }
}

void loop() {
  if (count >= flywheelTeeth) {
    count = count - flywheelTeeth;
    newTime = millis();
    float curRPM = 1 / (float(newTime - oldTime) / 1000) * 60;
    displayNumber(curRPM);
    oldTime = newTime;
    rpmArray[rotationNumber % arraySize] = curRPM;
    rotationNumber++;
    avgRPM = 0;
    for (int i = 0; i < arraySize; i++) {
      Serial.print(rpmArray[i]);
      Serial.print(" ");
      avgRPM += rpmArray[i];
    }
    Serial.print("\n");
    avgRPM = avgRPM/arraySize;
    displayNumber(avgRPM);
  }
  if (count < 0) {
    count += flywheelTeeth;
    newTime = millis();
    float curRPM = 1 / (float(newTime - oldTime) / 1000) * 60;
    displayNumber(curRPM);
    oldTime = newTime;
    rpmArray[rotationNumber % arraySize] = curRPM;
    rotationNumber++;
    avgRPM = 0;
    for (int i = 0; i < arraySize; i++) {
      Serial.print(rpmArray[i]);
      Serial.print(" ");
      avgRPM += rpmArray[i];
    }
    Serial.print("\n");
    avgRPM = avgRPM/arraySize;
    displayNumber(avgRPM);
    if(rotationNumber%10 == 0){
      if(avgRPM < desiredRPM){
        solPowerWidth++;
      } else {
        solPowerWidth--;
      }
    }
  }
  if (printnow) {
    Serial.println(count);
    printnow = false;
  }
  //Spin away from you
  //  if (count > 32 && count < 35) {
  //    //if (count > 14 && count < 37) {
  //    digitalWrite(sol1, HIGH);
  //    //Serial.println("ON 1");
  //  } else {
  //    digitalWrite(sol1, LOW);
  //  }
  //  if (count > 8 && count < 11) {
  //    //if (count > 0 && count < 12) {
  //    digitalWrite(sol2, HIGH);
  //    //Serial.println("ON 2");
  //  } else {
  //    digitalWrite(sol2, LOW);
  //  }

  //Spin towards you
  if (count > sol1ToothPower && count < sol1ToothPower+solPowerWidth) {
    //if (count > 14 && count < 37) {
    digitalWrite(sol1, HIGH);
    //Serial.println("ON 1");
  } else {
    digitalWrite(sol1, LOW);
  }
  if (count > sol2ToothPower && count < sol2ToothPower+solPowerWidth) {
    //if (count > 0 && count < 12) {
    digitalWrite(sol2, HIGH);
    //Serial.println("ON 2");
  } else {
    digitalWrite(sol2, LOW);
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
  if (forward) {
    if (count != 2) {
      Serial.println("forward wrong!");
      Serial.println(count);
      Serial.println("end");
    }
    count = 2;
  } else {
    if (count != 0) {
      Serial.println("backward wrong!");
      Serial.println(count);
      Serial.println("end");
    }
    count = 0;
  }

}

void displayNumber(int num) {
  tm.display(3, num % 10);
  tm.display(2, num / 10 % 10);
  tm.display(1, num / 100 % 10);
  tm.display(0, num / 1000 % 10);
}

