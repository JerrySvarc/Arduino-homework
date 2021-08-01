#include "funshield.h"

unsigned long last_time;
constexpr int dtime = 300;
const int ledky[4] = {led1_pin,led2_pin,led3_pin,led4_pin};
const int had[7] = {0x01,0x02,0x04,0x08,0x04,0x02,0x01};
int counter;

void setup() 
{
  // put your setup code here, to run once:
  for(int i = 0; i < 4; ++i){
      pinMode(ledky[i],OUTPUT);
    }
  last_time = millis();
  counter = 0;
  zobrazHada(counter);
}

void zobraz(int idx, bool on){
  digitalWrite(ledky[idx], on?ON:OFF);
}

bool test(int val, int num){
  return had[val] & had[num];
}

void zobrazHada(int val){
  for(int i =0; i<4; ++i){
    zobraz(i,test(val,i));
 }
}
void loop() 
{
  if (counter> 5) counter = 0;
  auto current_time = millis();
  auto delta = current_time - last_time;
  if(delta >= dtime){
      ++counter;
      zobrazHada(counter);
      last_time = current_time;
    }
 }

 
