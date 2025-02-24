#include <Arduino.h>
#include <Servo.h>
#include <ezButton.h>
#include <sbus.h>
#include "TeensyThreads.h"

bfs::SbusRx sbus_rx(&Serial1);
bfs::SbusData ReceiverValue;

Servo S1, S2, S3, S4;
ezButton button(13);

#define PI 3.14159265359

int p1; //for extend and retract(Wheel:CH3)
int p2, prev_p2 = 0; //to select the modes(up,straight,down)(B:CH1)

volatile int x = 0,y=0,  s2 = 0, s3 = 0, s4 = 0;
volatile float CG = 20.0, bat_w = 4000.0;
const float x1 = 20, x2 = 20, x3 = 20, x4 = 20;
const float l1 = 40, l2 = 40, l3 = 40;
const float w1 = 100, w2 = 100, w3 = 100, w4 = 100;
float s1 = 0;

float Battery_pos;

void read_receiver() //to read herelink data
{
  if (sbus_rx.Read()) {
      ReceiverValue = sbus_rx.data();
      for (int8_t i = 0; i < ReceiverValue.NUM_CH; i++) {
          ReceiverValue.ch[i] = 1000 + ((ReceiverValue.ch[i] * 0.636) - 153);
      }
  }
}

void CG_Balance() 
{
  while (true) 
  {
    int sot1 = S1.read();
    int sot2 = S2.read();
    int sot3 = S3.read();

    float t2 = (90 - sot1) * (PI / 180.0);
    float t3 = (270 - sot1 + sot2) * (PI / 180.0);
    float t4 = (90 - sot1 + sot2 - sot3) * (PI / 180.0);

    float a1 = x1;
    float a2 = l1 + x2 * cos(t2);
    float a3 = l1 + l2 * cos(t2) + x3 * cos(t3);
    float a4 = l1 + l2 * cos(t2) + l3 * cos(t3) + x4 * cos(t4);
    
    float wa = w1 + w2 + w3 + w4;
    float aCG = (a1 * w1 + a2 * w2 + a3 * w3 + a4 * w4) / wa;
    Battery_pos = (CG * (bat_w + wa) - wa * aCG) / bat_w;

    s4 = map(Battery_pos*10,150,190, 0, 180);
    S4.write(s4);
    threads.yield();
  }
}

void adjustServoPosition() //To go straight
{
  s1 = constrain(x, 0, 90)*10;
  S1.writeMicroseconds(map(s1, 0, 2700, 500, 2500));
  s2 = 2 * x-15;
  S3.write(x+3);
  
  int so2 = S2.read();
  while (so2 != s2) 
  {
    so2 += (so2 > s2) ? -1 : 1;
    S2.write(so2);
    threads.delay(1);
  }
}
void adjustServoPositionUP() // To go up
{
  s1 = (x-(y/2))*10;//constrain(x, 0, 90) - constrain(y, 0, 90)/2;
  S1.writeMicroseconds(map(s1, 0, 2700, 500, 2500));
  s2 = 2 * x-15;
  S3.write(x-45);
  
  int so2 = S2.read();
  while (so2 != s2) 
  {
    so2 += (so2 > s2) ? -1 : 1;
    S2.write(so2);
    threads.delay(1);
  }
}
void adjustServoPositionDOWN() //To go down
{
  s1 = 10*(constrain(x, 0, 135) + constrain(y, 0, 90)/2);
  S1.writeMicroseconds(map(s1, 0, 2700, 500, 2500));
  s2 = 2 * x-15;
  S3.write(x-40+90);
  
  int so2 = S2.read();
  while (so2 != s2)
  {
    so2 += (so2 > s2) ? -1 : 1;
    S2.write(so2);
    threads.delay(1);
  }
}

void changeOP() //initial position
{
  int s1_in = 0;
  int s2_in = 0;
  int s3_in = 0;
  int so1 =map(S1.read(),0,180,0,270);
  int so2 = S2.read();
  int so3 = S3.read();
  for(;so1!=s1_in || so2!=s2_in || so3!=s3_in;)
  {
    if(so1>s1_in){so1--;}
    else if(so1<s1_in){so1++;}
    S1.writeMicroseconds(map(so1,0,270,500,2500));

    if(so2>s2_in){so2--;}
    else if(so1<s2_in){so2++;}
    S2.writeMicroseconds(map(so2,0,180,500,2500));

    if(so3>s3_in){so3--;}
    else if(so3<s3_in){so3++;}
    S3.write(so3);
    threads.delay(30);
  }
  read_receiver();
  p2 = ReceiverValue.ch[0];
  prev_p2=p2;
  x=0;
  y=0;
}

void straight()
{
  Serial.println(p2);
  read_receiver();
  p1 = ReceiverValue.ch[2];
  while (p1 > 1600 && x < 90 && y<90) 
  { // Extend
    x++;
    y++;
    adjustServoPosition();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(20);
  }
  
  while (p1 < 1300 && x > 0 && y>0) 
  { // Retract
    x--;
    y--;
    adjustServoPosition();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(20);
  }
}
void up()
{
  Serial.println(p2);
  read_receiver();
  p1 = ReceiverValue.ch[2];
        
  while (p1 > 1600 && x < 90 && y<90) 
  { // Extend
    x++;
    y++;
    adjustServoPositionUP();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(20);
  }
  
  while (p1 < 1300 && x > 0 && y>0) 
  { // Retract
    x--;
    y--;
    adjustServoPositionUP();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(20);
  }
}
void dow()
{
  Serial.println(p2);
  read_receiver();
  p1 = ReceiverValue.ch[2];
        
  while (p1 > 1600 && x < 90 && y<90) 
  { // Extend
    x++;
    y++;
    adjustServoPositionDOWN();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(20);
  }
  
  while (p1 < 1300 && x > 0 && y>0) 
  { // Retract
    x--;
    y--;
    adjustServoPositionDOWN();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(20);
  }

}

void operation() 
{
  read_receiver();
  p2 = ReceiverValue.ch[0];

  while(true)
  {
    read_receiver();
    p2 = ReceiverValue.ch[0];
    while (p2>800 & p2<1200) 
    {
      read_receiver();
      p2 = ReceiverValue.ch[0];
      if(!(prev_p2< p2 +100 & prev_p2> p2-100)) {changeOP();}
      up();
    }
    while (p2>1300 & p2<1600) 
    {
      read_receiver();
      p2 = ReceiverValue.ch[0];
      if(!(prev_p2< p2 +100 & prev_p2> p2-100)) {changeOP();}
      straight();
    }
    while (p2>1700 & p2<2000) 
    {
      read_receiver();
      p2 = ReceiverValue.ch[0];
      if(!(prev_p2< p2 +100 & prev_p2> p2-100)) {changeOP();}
      dow();
    }
  }
  
  threads.yield();
}

void setup() 
{
  Serial.begin(9600);
  S1.attach(2, 500, 2500);
  S2.attach(3, 500, 2500);
  S3.attach(4, 500, 2500);
  S4.attach(5, 500, 2500);
  
  S1.write(0);
  S2.write(0);
  S3.write(0);
  S4.write(0);
  
  sbus_rx.Begin();
  threads.addThread(CG_Balance);
  threads.addThread(operation);
}

void loop() 
{
  // read_receiver();
  // Serial.println(ReceiverValue.ch[2]);
  //Serial.println(cos(PI));
  //Serial.println(Battery_pos);
}
