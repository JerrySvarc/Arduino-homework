#include "funshield.h"

//mode -> True = normal, False = config
bool ModeType;
bool Running;

//buttons
const int buttons[3] = {button1_pin, button2_pin, button3_pin};
const int ButtonCount = 3;


//Display
int Position;
const byte numbers[10] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};
const byte positions[4] = { 0x08, 0x04, 0x02, 0x01};
byte PositionVals[4] = {0xc0,0xc0,0x00,0x00};
byte TypeVal[2] = {0xc0,0xc0};
const byte None = 0xff;
const byte LetterD = 0b10100001;   // d
int LargestDigit;

//Dice 
int ThrowCount;
const int DiceTypes[7] = {4,6,8,10,12,20,100};
int DiceIndex;

//Timer and hashing
long Sum;
unsigned long PressedTime;


//Animation
byte AnimationVal= 0xf6;
int CurrentTime;
int LastTime;
unsigned int DeltaTime;
int AnimationPos;


void GlyphBitmask(byte glyph, byte posmask) {
  digitalWrite(latch_pin, LOW);
  shiftOut(data_pin, clock_pin, MSBFIRST, glyph);
  shiftOut(data_pin, clock_pin, MSBFIRST, posmask);
  digitalWrite(latch_pin, HIGH);
} 

void translateNumber(long a, byte Val[], int Border ){
  for(int i = 0; i <= Border; ++i){
    unsigned  digit = a%10;
    a /= 10;
    Val[i]=numbers[digit];
    }
}

void NumOfPositions(long sum) {
  LargestDigit = 0;
  while(sum >= 10){
    sum /= 10;
    ++LargestDigit;
  }
}

void displayNum(){
    NumOfPositions(Sum);
    translateNumber(Sum,PositionVals,LargestDigit);
    if (Position <= LargestDigit){GlyphBitmask(PositionVals[Position], positions[Position]);}
    else{GlyphBitmask(None, positions[Position]);}
}

unsigned long hash(unsigned long x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

void displayConfig(){
  translateNumber(DiceTypes[DiceIndex],TypeVal,1);
  GlyphBitmask(numbers[ThrowCount], positions[3]);
  GlyphBitmask(LetterD, positions[2]);
  GlyphBitmask(TypeVal[1], positions[1]);
  GlyphBitmask(TypeVal[0], positions[0]);
}

class Button {
 public:
      bool LastState;
      bool State;
      explicit Button(int tlacitko){
          button = tlacitko;
      }
void process( bool State){
  if (State != LastState){
    switch (button){
      case 0:
        Button0();
        break;
      case 1:
        Button1();
        break;
      case 2:
        Button2();
        break;
    }
    LastState = State;
  }
  
}
private:
      int button;
      long PressTime;
      long PressDelta;
      long DeltaTime;

void TimeHash(long Time){
  for (int i = 0; i < ThrowCount; ++i){
    Sum += (hash(Time+i) % DiceTypes[DiceIndex])+1;
  }
}
      
void Button0(){
    if (ModeType && State == LOW && Running){
      Sum = 0;
      for (int i = 0; i < 4; ++i){
        PositionVals[i] = 0xc0;
      }
      PressTime = micros();
    }
    else if (ModeType && State == HIGH && Running){
      PressedTime = micros() - PressTime;
      TimeHash(PressedTime);
    }
    //Pri prechodu z config -> normal, aby se jen prehodil mod, ale nedoslo hned ke generovani cisla
    else if (!ModeType && State == LOW){
      ModeType = true;
    }
    else if (ModeType && State == HIGH ){
      Running = true;
    }
}

void Button1(){
  if (!ModeType && State == LOW){
    ThrowCount = (ThrowCount %9) + 1;
  }
  else if (ModeType && State == LOW){
    ModeType = false;
    Running = false;
  }
}

void Button2(){
    if (!ModeType && State == LOW){
      DiceIndex = (DiceIndex + 1 ) % 7;
    }
    else if (ModeType && State == LOW){
      ModeType = false;
      Running = false;
    }
  } 
};

Button ButtonObj[3] = {Button(0), Button(1), Button(2)};

void setup() {
    pinMode(latch_pin, OUTPUT); 
    pinMode(clock_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);
    for (int j = 0; j < ButtonCount; ++j){ 
      pinMode(buttons[j], INPUT); 
    } 
    Position = 0;
    LargestDigit = 3;
    ModeType = true;
    Running = true; 
    ThrowCount = 1;
    PressedTime = 0;
    DiceIndex = 0;
    Sum = 0;
    AnimationPos = 0;
    for (int i = 0; i < ButtonCount; ++i){
        ButtonObj[i].LastState = digitalRead(buttons[i]);
    }
}

void Animation(){
  GlyphBitmask(AnimationVal, positions[Position]);
  LastTime = CurrentTime;
}

void Display(int delta, bool state){
  if(state != LOW){
    displayNum();
    Position = (Position + 1) % 4;
  } 
  else {
    if(delta >= 75 && Running){
        Animation();
        Position = (Position + 1) % 4;}
  }
}

void loop() {
  CurrentTime = millis();
  DeltaTime = CurrentTime - LastTime;
  
  for (int i = 0; i < ButtonCount; ++i){
    ButtonObj[i].State = digitalRead(buttons[i]);
    ButtonObj[i].process( ButtonObj[i].State);
  }
  
  if(ModeType){            
    Display(DeltaTime, ButtonObj[0].State);
  }
  else{
    displayConfig();
    Position = (Position + 1) % 4;
  }
}

   
