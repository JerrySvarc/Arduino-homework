#include "funshield.h"

const int ledky[4] = {led4_pin,led3_pin,led2_pin,led1_pin};
const int buttons[2] = {button1_pin,button2_pin};

const int hranice = 16;
const int WaitTime = 1000;
const int incrTime = 300;
const int  ButtonCount = 2;
const int IncVal[2] = {1,-1};
int CurrTime;
int counter; 

void show(int idx, bool on){
  digitalWrite(ledky[idx], on?ON:OFF);
}

 bool test(int val, int num){
  return val & (1<<num);
}
 void zobraz(int val){
  for(int i =0; i<4; ++i){
    show(i,test(val,i));
 }
}
class Button {
  public:
      bool LastState;
      int LastTime;
      bool State;
      explicit Button(int tlacitko){
          button = tlacitko;
          }
   private:
      int button;
      int DeltaTime;
      int PressTime;
      int LastPressTime;
      int WaitHranice = WaitTime;
      int PressDelta;
      bool getstate(int state){
          if (state == LOW){
             return true;
          }
          else{
              return false;
          }
        }
     void ShowIncr(int button){
         counter += IncVal[button];
         counter = (counter + hranice)% hranice;
         zobraz(counter);
        }
public: 
void process(int CurrTime, bool State){
   DeltaTime = CurrTime - LastTime;
   PressDelta = CurrTime - LastPressTime;
   if (State != LastState){
      if (getstate(State) == true){
        LastPressTime = CurrTime;
        ShowIncr(button);
      }
      else{
       WaitHranice = WaitTime;
      }
     LastState = State;
     LastTime = CurrTime;
      }
   else if (getstate(State)== true && PressDelta>= WaitHranice){
      LastPressTime = CurrTime;
      WaitHranice = incrTime;
      ShowIncr(button);
   }
}
};

Button ButtonObj[2] = {Button(0), Button(1)};
void setup() 
{
   for (int i =0; i < 4; ++i){
      pinMode(ledky[i],OUTPUT);
  }
  for (int j = 0; j < 2; ++j){
     pinMode(buttons[j], INPUT);
     
    }
  counter = 0;
  zobraz(counter);
  for (int i = 0; i < ButtonCount; ++i){
    ButtonObj[i].LastState = digitalRead(buttons[i]);
    ButtonObj[i].LastTime = millis();
  }
  }
  
void loop() {
  for (int i = 0; i < ButtonCount; ++i){
    ButtonObj[i].State = digitalRead(buttons[i]);
  }
  CurrTime = millis();
  for (int i = 0; i < ButtonCount; ++i){
    ButtonObj[i].process(CurrTime, ButtonObj[i].State);
  }
   
  }

   
