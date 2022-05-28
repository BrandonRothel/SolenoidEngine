const byte rotEnc1 = 3;
const byte rotEnc2 = 4;
const byte hallEff = 2;
const byte sol1 = 5;
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
  digitalWrite(sol1, LOW);
  pinMode(rotEnc1, INPUT_PULLUP);
  pinMode(rotEnc2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(rotEnc1), updatePos, CHANGE);
  attachInterrupt(digitalPinToInterrupt(hallEff), rotComplete, FALLING);
  Serial.begin(9600);

}

void loop() {
  if (count >= flywheelTeeth) {
    count = count - flywheelTeeth;
    newTime = millis();
    //Serial.println(newTime - oldTime);
    oldTime = newTime;
  }
  if (count < 0) {
    count += flywheelTeeth;
    newTime = millis();
    //Serial.println((newTime - oldTime));
    oldTime = newTime;
  }
  if(printnow){
      //Serial.println(count);
      printnow = false;
  }

  if (count > 10 && count < 20) {
    digitalWrite(sol1, HIGH);
    //Serial.println("ON");
  } else {
    digitalWrite(sol1, LOW);
  }
  if(rot){
    Serial.println(count);
    rot = false;
    count = 47;
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

void rotComplete(){
  rot = true;
}

