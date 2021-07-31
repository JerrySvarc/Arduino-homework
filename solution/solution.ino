#include "funshield.h"
#include "input.h"

long LastTime;
long CurrTime;
long Position;
int Delta;
int Length;
bool Running;
bool End;
int Counter;
const char *str;
const char *pointer = str;
constexpr byte EMPTY_GLYPH = 0b11111111;
int positionsCount = 4;
constexpr unsigned int scrollingInterval = 300;



// map of letter glyphs
constexpr byte LETTER_GLYPH[] {
  0b10001000,   // A
  0b10000011,   // b
  0b11000110,   // C
  0b10100001,   // d
  0b10000110,   // E
  0b10001110,   // F
  0b10000010,   // G
  0b10001001,   // H
  0b11111001,   // I
  0b11100001,   // J
  0b10000101,   // K
  0b11000111,   // L
  0b11001000,   // M
  0b10101011,   // n
  0b10100011,   // o
  0b10001100,   // P
  0b10011000,   // q
  0b10101111,   // r
  0b10010010,   // S
  0b10000111,   // t
  0b11000001,   // U
  0b11100011,   // v
  0b10000001,   // W
  0b10110110,   // ksi
  0b10010001,   // Y
  0b10100100,   // Z
};

/** 
 * Show chararcter on given position. If character is not letter, empty glyph is displayed instead.
 * @param ch character to be displayed
 * @param pos position (0 = leftmost)
 */

int StringLength(const char *s) {
   int c = 0;
   while(s[c] != '\0')
      c++;
   return c;
}
void displayChar(char ch, byte pos)
{
  byte glyph = EMPTY_GLYPH;
  if (isAlpha(ch)) {
    glyph = LETTER_GLYPH[ ch - (isUpperCase(ch) ? 'A' : 'a') ];
  }
  
  digitalWrite(latch_pin, LOW);
  shiftOut(data_pin, clock_pin, MSBFIRST, glyph);
  shiftOut(data_pin, clock_pin, MSBFIRST, 1 << pos);
  digitalWrite(latch_pin, HIGH);
}

SerialInputHandler input;


void Shift(){
  if (Counter == 0 && End == false && Length >= 4){
       ++pointer;
  }
}

void DisplayTwo(int pos){
  if (Counter == pos){displayChar(pointer[0], pos);}
      else if (pos == (Counter+1)) {displayChar(pointer[1] , pos);}
      else{displayChar( EMPTY_GLYPH , pos);}
}

void DisplayThree(int pos){
  if (Counter == pos){displayChar(pointer[0], pos);}
  else if (pos == (Counter+1)) {displayChar(pointer[1] , pos);}
  else if (pos == (Counter+2)) {displayChar(pointer[2] , pos);}
  else{displayChar( EMPTY_GLYPH , pos);}
}

void DisplayShorter(int pos){
  switch (Length){
    case 0:
      displayChar( EMPTY_GLYPH , pos);
      break;
    case 1:
      if (Counter == pos){displayChar( pointer[0] , pos);}
      else {displayChar( EMPTY_GLYPH , pos);}
      break;
    case 2:
      DisplayTwo(pos);
      break;
    case 3:
      DisplayThree(pos);
      break;
  }
}
void IsAtEnd(){
  if (pointer+3 >= (str + Length) && End == false && Length >= 4)  {
    End = true; 
    Counter = 3;
  }
  else if (pointer+3 >= (str + Length) && End == false && Length < 4 )  {
    End = true; 
    Counter = 4;
  }
}
void DisplayBeginning(int pos){
  if(Length > 3){
    if (Position < Counter){displayChar( EMPTY_GLYPH , pos);}
    else {displayChar( pointer[(pos - Counter)] , pos);}
  }
}

void DisplayEnd(int pos){
  if (Length < 4){
    DisplayShorter(pos);
  }
  else{
    if (pos > Counter){displayChar( EMPTY_GLYPH , pos);}
    else {displayChar( pointer[(3 - Counter)+ pos] , pos);}
  }
}

void Display7Seg(int pos){
  if (End == false) {
    if (Length >= 4){DisplayBeginning(pos);}
  }
  else{
   DisplayEnd(pos);
   if(Counter == 0 && Length >= 4) {Running = false;}
   else if (Length < 4 && (Counter == -Length)) {Running = false;}
  }
}

void Display(int pos,int delta){
  if (delta >= scrollingInterval && Running == true ){ 
    Shift();
    if ((Length >= 4 || Length == 0) && Counter > 0){--Counter;}
    else if (Length < 4 && Counter > -Length) {--Counter;} 
    LastTime = CurrTime;
    }
  Display7Seg(pos);
  IsAtEnd();
}

void setup() {
    pinMode(latch_pin, OUTPUT); 
    pinMode(clock_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);
    LastTime = millis();
    Position = 0; 
    Length = 0; 
    Counter = 0;
    input.initialize();
    Running = false;
    End = false;
    
}
    
void loop() {
    
  input.updateInLoop();
  CurrTime = millis();
  Delta = CurrTime - LastTime;
  
  if (Running == false){
    str = input.getMessage();
    Running = true;
    Counter = 4;
    pointer = str;
    Length = StringLength(str);
    End = false;
  }
  else if (Running == true) {
    Display(Position, Delta);
  }
  Position = (Position + 1) % 4;
 
  }
  


   
