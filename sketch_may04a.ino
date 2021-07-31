#include "funshield.h"

const int buttons[3] = {button1_pin, button2_pin, button3_pin};
const byte numbers[10] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};
const byte positions[4] = { 0x08, 0x04, 0x02, 0x01};
const byte period = 0x7f;
const byte None = 0xff;
const int hranice = 10000;
const int  ButtonCount = 3;
byte PositionVals[4] = {0x00,0x00,0x00,0x00};
int Position;
bool isRunning; 
bool Refresh;
long DisplayCounter;
long counter;
int LargestDigit;
long CurrTime;
long LastTime;
long Show;
long ShowDelta;

void GlyphBitmask(byte glyph, byte posmask) {
  digitalWrite(latch_pin, LOW);
  shiftOut(data_pin, clock_pin, MSBFIRST, glyph);
  shiftOut(data_pin, clock_pin, MSBFIRST, posmask);
  digitalWrite(latch_pin, HIGH);
} 
void translateNumber(long a ){
  for(int i = 0; i <= LargestDigit; ++i){
    unsigned digit = a%10;
    a /= 10;
    if (i == 1){PositionVals[i] = (numbers[digit] & period);}
    else{ PositionVals[i]=numbers[digit];}
}
}
void NumOfPositions() {
  if(DisplayCounter >= 0 && DisplayCounter < 100){
    LargestDigit = 1;
   }
  else if(DisplayCounter >= 100 && DisplayCounter < 1000){
     LargestDigit = 2;
  }
  else if(DisplayCounter >= 1000 && DisplayCounter < 10000){
    LargestDigit = 3;
  }
}
void displayNum(){
    if (Position <= LargestDigit){GlyphBitmask(PositionVals[Position], positions[Position]);}
    else{GlyphBitmask(None, positions[Position]);}
}
void timer(long currTime){
   if (isRunning == true ){
        counter = currTime - LastTime;   
   }
   else{LastTime = currTime - counter ;}
   if (Refresh != false){
       DisplayCounter = (counter / 100 );
       NumOfPositions();
       translateNumber(DisplayCounter);
 }
}

class Button {
  public:
      bool LastState;
      bool State;
      explicit Button(int tlacitko){
          button = tlacitko;
      }
   private:
      int button;
     void ShowIncr(int butt){
      if (butt == 0 && Refresh == true){
        isRunning = !isRunning;
        }
      if(butt == 1 && isRunning == true){
        Refresh = !Refresh;
       }
      if (butt == 2 && isRunning == false && Refresh == true ){
      counter = DisplayCounter = 0;
      LargestDigit = 1;
       }
      }
    
public: 
void process( bool State){
   if (State != LastState){
      if (State == LOW){
        ShowIncr(button); 
   }
     LastState = State;
  }
 }
};

Button ButtonObj[3] = {Button(0), Button(1), Button(2) };

void setup() {
    pinMode(latch_pin, OUTPUT); 
    pinMode(clock_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);
    for (int j = 0; j < ButtonCount; ++j){ 
      pinMode(buttons[j], INPUT); 
      } 
    counter = 0;
    Position = 0;
    LargestDigit = 1;
    isRunning = false;
    Refresh = true;
    LastTime = 0;
    CurrTime = 0;
    DisplayCounter = 0;
    Show = 0; 
    ShowDelta = 0;
}
    
void loop() {
  for (int i = 0; i < ButtonCount; ++i){
    ButtonObj[i].State = digitalRead(buttons[i]);
    ButtonObj[i].process( ButtonObj[i].State);
  }
  CurrTime = millis();
  timer(CurrTime);
  displayNum();
  Position = (Position + 1) % 4;
 
  
}

   
