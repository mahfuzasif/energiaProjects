#include <Servo.h>
#include "LCD_Launchpad.h"

Servo myservo;
LCD_LAUNCHPAD lcd;

//==============Stepper=======================
int motorPin1 = 7;
int motorPin2 = 8;
int motorPin3 = 5;
int motorPin4 = 14;
int delayTime = 2;

int c = 0;
//=============Servo===================
int pos = 90;
//============Load=====================
#define DT 11
#define SCK 12
//#define sw 2
long sample = 0;
float val = 0;
long count = 0;
int w = 0;
//==============IR=====================
int pd_1 = 17;
int pd_2 = 13;
int senRead_1 = 4;
int senRead_2 = 6;
int limit_1 = 150; //for 4 pin
int limit_2 = 900; //for 6 pin
int count_ir = 0;
//============Hall=====================
const int hall1 = 19;
const int hall2 = 18;
bool state1 = false;
bool state2 = false;
//============Button===================
const int btnPin1 = PUSH1;
const int btnPin2 = PUSH2;
const int btnPin3 = 15;

int buttonPin = 0;
int btn2 = 0;
int btn3 = 0;
//=============Extra===================
int target_floor = 0;
int present_floor = 0;
bool door_open = true;
bool align = false; //change this to false bcz necessary
bool going_up = true;
bool going_down = false;


unsigned long readCount(void)
{
  unsigned long Count;
  unsigned char i;
  pinMode(DT, OUTPUT);
  digitalWrite(DT, HIGH);
  digitalWrite(SCK, LOW);
  Count = 0;
  pinMode(DT, INPUT);
  while (digitalRead(DT));
  for (i = 0; i < 24; i++)
  {
    digitalWrite(SCK, HIGH);
    Count = Count << 1;
    digitalWrite(SCK, LOW);
    if (digitalRead(DT))
      Count++;
  }
  digitalWrite(SCK, HIGH);
  Count = Count ^ 0x800000;
  digitalWrite(SCK, LOW);
  return (Count);
}


void setup() {
  //=================stepper==================
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  //=================Servo======================
  myservo.attach(9);
  //=================Load========================
  lcd.init();
  pinMode(SCK, OUTPUT);
  calibrate();
  //===============IR===========================

  pinMode(pd_1, OUTPUT);
  digitalWrite(pd_1, HIGH);
  pinMode(pd_2, OUTPUT);
  digitalWrite(pd_2, HIGH);
  //================Hall========================
  pinMode(hall1, INPUT);
  pinMode(hall2, INPUT);
  attachInterrupt(hall1, magnet_detect1, FALLING);
  attachInterrupt(hall2, magnet_detect2, FALLING);
  //==============Button=======================
  pinMode(btnPin1, INPUT_PULLUP);
  pinMode(btnPin2, INPUT_PULLUP);
  pinMode(btnPin3, INPUT);
}

void loop() {
  if (state1 == true && state2 == false) {
    present_floor = 1;
    bool align = true;
    state1 = false;
    state2 = false;
  } else if (state1 == true && state2 == true) {
    present_floor = 2;
    bool align = true;
    state1 = false;
    state2 = false;
  } else if (state1 == false && state2 == true) {
    present_floor = 3;
    bool align = true;
    state1 = false;
    state2 = false;
  } else {
    bool align = false;
  }
  if (!door_open) {
    for (pos = 15; pos <= 100; pos += 1) {
      myservo.write(pos);
      delay(15);
    }
    door_open = true;
  }
  for (int j = 0; j < 20; j++) {
    int val_1 = analogRead(senRead_1);
    int val_2 = analogRead(senRead_2);
    if (val_2  <= limit_2)
    {
      ++count_ir;
    } else if (val_1 <= limit_1)
    {
      if (count > 0) {
        //Serial.println("Else");
        --count_ir;
      }
    }
  }
  delay(3000);

  count = readCount();
  w = (((count - sample) / val) - 2 * ((count - sample) / val));
  if (w == 1) {
    w = 0;
  }

  Serial.print("weight:");
  Serial.print((int)w);
  Serial.println("g");
  lcd.print("W");
  lcd.print(w);
  lcd.println("g");

  Serial.print("count: ");
  Serial.println(count_ir);
  Serial.println(target_floor + 1);
  // delay(3000);
  if (door_open) {
    for (pos = 100; pos >= 15; pos -= 1) {
      myservo.write(pos);
      delay(15);
    }
    door_open = false;
  }
  if (target_floor <= 1 && going_up) {
    for (int n = 0; n < 1725 ; n++) {
      anticlock();
    }

    ++target_floor;
    Serial.println(target_floor + 1);
    if (target_floor == 2) {
      going_up = false;
      going_down = true;
    }
  } else if (target_floor <= 2 && going_down) {
    for (int n = 0; n < 1725 ; n++) {
      clockwise();
    }

    --target_floor;
    Serial.println(target_floor + 1);
    if (target_floor == 0) {
      going_up = true;
      going_down = false;
    }
  }
}

void anticlock() {
  digitalWrite(motorPin4, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin1, LOW);
  delay(delayTime);

  digitalWrite(motorPin4, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin1, LOW);
  delay(delayTime);

  digitalWrite(motorPin4, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin1, LOW);
  delay(delayTime);

  digitalWrite(motorPin4, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin1, HIGH);
  delay(delayTime);
}

void clockwise() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);

  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);

  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);

  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
  delay(delayTime);
}

void magnet_detect1()
{
  state1 = true;
}

void magnet_detect2()
{
  state2 = true;
}

void calibrate()
{
  lcd.clear();
  lcd.print("Cing");
  lcd.print("Wait");
  for (int i = 0; i < 100; i++)
  {
    count = readCount();
    sample += count;
    Serial.println(count);
  }
  sample /= 100;
  //Serial.print("Avg:");
  //Serial.println(sample);
  Serial.println("Put weight and wait");
  lcd.clear();
  count = 0;
  while (count < 1000)
  {
    count = readCount();
    count = sample - count;
    Serial.println(count);
    //extra line
    if (count < 0) {
      count *= -1;
    }

  }
  lcd.clear();
  lcd.print("Wait");
  delay(2000);
  for (int i = 0; i < 100; i++)
  {
    count = readCount();
    val += sample - count;
    // Serial.println(sample - count);
    //extra line
    count *= -1;
  }
  val = val / 100.0;
  val = val / 100.0;    // put here your calibrating weight
  lcd.clear();
}


