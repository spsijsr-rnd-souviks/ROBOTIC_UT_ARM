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

volatile int x = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0;
volatile float CG = 20.0, bat_w = 4000.0;
const float x1 = 20, x2 = 20, x3 = 20, x4 = 20;
const float l1 = 40, l2 = 40, l3 = 40;
const float w1 = 100, w2 = 100, w3 = 100, w4 = 100;

float Battery_pos;

void read_receiver()
{
  if (sbus_rx.Read())
  {
    ReceiverValue = sbus_rx.data();
    for (int8_t i = 0; i < ReceiverValue.NUM_CH; i++)
    {
      ReceiverValue.ch[i] = 1000 + ((ReceiverValue.ch[i] * 0.636) - 153);
    }
  }
}

void CG_Balance()
{
  while (true)
  {
    int so1 = S1.read();
    int so2 = S2.read();
    int so3 = S3.read();

    float t2 = (90 - so1) * (PI / 180.0);
    float t3 = (270 - so1 + so2) * (PI / 180.0);
    float t4 = (90 - so1 + so2 - so3) * (PI / 180.0);

    float a1 = x1;
    float a2 = l1 + x2 * cos(t2);
    float a3 = l1 + l2 * cos(t2) + x3 * cos(t3);
    float a4 = l1 + l2 * cos(t2) + l3 * cos(t3) + x4 * cos(t4);

    float wa = w1 + w2 + w3 + w4;
    float aCG = (a1 * w1 + a2 * w2 + a3 * w3 + a4 * w4) / wa;
    Battery_pos = (CG * (bat_w + wa) - wa * aCG) / bat_w;

    s4 = map(Battery_pos, 15.0, 19.0, 0, 180);
    S4.write(s4);
    threads.yield();
  }
}

void adjustServoPosition()
{
  s1 = constrain(x, 0, 90);
  S1.write(map(s1, 0, 270, 0, 180));
  s2 = 2 * x;
  S3.write(x);

  int so2 = S2.read();
  while (so2 != s2)
  {
    so2 += (so2 > s2) ? -1 : 1;
    S2.write(so2);
    threads.delay(1);
  }
}

void operation()
{
  while (true)
  {
    read_receiver();
    int p1 = ReceiverValue.ch[2];

    while (p1 > 1600 && x < 90)
    { // Extend
      x++;
      adjustServoPosition();
      read_receiver();
      p1 = ReceiverValue.ch[2];
      threads.delay(20);
    }

    while (p1 < 1300 && x > 0)
    { // Retract
      x--;
      adjustServoPosition();
      read_receiver();
      p1 = ReceiverValue.ch[2];
      threads.delay(20);
    }

    threads.yield();
  }
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
  // Serial.println(cos(PI));
  Serial.println(Battery_pos);
}
