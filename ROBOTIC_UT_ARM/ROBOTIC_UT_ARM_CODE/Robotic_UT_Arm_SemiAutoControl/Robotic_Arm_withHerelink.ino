// Including necessary Library
#include <Arduino.h>
#include <Servo.h>
#include <sbus.h>
#include "TeensyThreads.h"
#include <math.h>

// Defining SBUS Object
bfs::SbusRx sbus_rx(&Serial1);
bfs::SbusData ReceiverValue;
// Defining Servo Object
Servo S1, S2, S3, S4;

#define PI 3.14159265359

// Receive signal From Remote
int p1;              // for extend and retract(Wheel:CH3)
int p2, prev_p2 = 0; // to select the modes(up,straight,downn)(B:CH1)

// LPF for encoder
float e1_pred = 0, e2_pred = 0, e3_pred = 0, a = 0.25;
float r1_pred = 0, r2_pred = 0;
int r1, r2;

// Defining Parameters for CG Balancing
volatile int x = 0, y = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0;

const float CG = -58.40, bat_w = 4000.0;
const float x1 = 3.0, x2 = 24.80, x3 = 18.5, x4 = 9.5;
const float l1 = 5, l2 = 38, l3 = 27;
const float w1 = 190, w2 = 183, w3 = 164, w4 = 160;

const int bat_init_pos = 695, bat_end_pos = 758;
const int s4_min = 25, s4_max = 180;

int pot1_pre = 0;
float Battery_pos;

void read_receiver() // Reads herelink data
{
  // For binding with Herelink remote (SBUS control), uncomment this part and comment out the Flysky part.

  if (sbus_rx.Read())
  {
    ReceiverValue = sbus_rx.data();
    for (int8_t i = 0; i < ReceiverValue.NUM_CH; i++)
    {
      ReceiverValue.ch[i] = 1000 + ((ReceiverValue.ch[i] * 0.636) - 153);
    }
  }

  // To bind with FlySky remote (PWM control), uncomment this part and comment the Herelink read part
  //  r1 =constrain(pulseIn(36, HIGH), 1000, 2000);//22
  //  r2 =constrain(pulseIn(33, HIGH), 1000, 2000);
  //  ReceiverValue.ch[0] =r1; //pulseIn(22, HIGH); // modes
  //  ReceiverValue.ch[2] = r2;//pulseIn(20, HIGH); // expansion and retraction
}

// Positioning of battery to balance CG according to Arm position
void CG_Balance()
{
  // Serial.println();
  while (true)
  {

    // to get real time angle of each arm using encoder
    int e1 = analogRead(A0);
    int e2 = analogRead(A1);
    int e3 = analogRead(A2);

    // Low Pass Filter to reduce the fluctuation of encoder data
    e1_pred = a * e1 + (1 - a) * e1_pred;
    e2_pred = a * e2 + (1 - a) * e2_pred;
    e3_pred = a * e3 + (1 - a) * e3_pred;

    int real_angle1 = map(e1_pred, 204, 260, 0, 90); // to get real time angle of each arm using encoder
    int real_angle2 = map(e2_pred, 72, 545, 0, 180);
    int real_angle3 = map(e3_pred, 164, 400, 0, 90);

    float t2 = (90 - real_angle1) * (PI / 180.0);
    float t3 = (270 - real_angle1 + real_angle2) * (PI / 180.0);
    float t4 = (90 - real_angle1 + real_angle2 - real_angle3) * (PI / 180.0);

    float a1 = x1;
    float a2 = l1 + x2 * cos(t2);
    float a3 = l1 + l2 * cos(t2) + x3 * cos(t3);
    float a4 = l1 + l2 * cos(t2) + l3 * cos(t3) + x4 * cos(t4);

    float wa = w1 + w2 + w3 + w4;
    float aCG = (a1 * w1 + a2 * w2 + a3 * w3 + a4 * w4) / wa;
    Battery_pos = (CG * (bat_w + wa) - wa * aCG) / bat_w;

    s4 = map(Battery_pos * (-10), bat_init_pos, bat_end_pos, s4_min, s4_max);
    S4.write(s4);
    Serial.print(Battery_pos);
    Serial.print(" | Deg: ");
    Serial.println(s4);
    threads.yield();
  }
}

void adjustServoPosition() // Function to Expand and Retract the arm in Horizontal direction
{
  s1 = constrain(x, 0, 90) * 10;
  S1.writeMicroseconds(map(s1, 0, 2700, 500, 2500));
  s2 = 2 * x - 15;
  S3.write(x - 20); //-5,-10, -20

  int so2 = S2.read();
  while (so2 != s2)
  {
    so2 += (so2 > s2) ? -1 : 1;
    S2.write(so2);
    threads.delay(5);
  }
}
void adjustServoPositionUP() // Function to Expand and Retract the arm in Upward direction
{
  s1 = (x - (y / 2)) * 10; // constrain(x, 0, 90) - constrain(y, 0, 90)/2;
  S1.writeMicroseconds(map(s1, 0, 2700, 500, 2500));
  s2 = 2 * x - 15;
  S3.write(x - 50); // 45,50

  int so2 = S2.read();
  while (so2 != s2)
  {
    so2 += (so2 > s2) ? -1 : 1;
    S2.write(so2);
    threads.delay(1);
  }
}
void adjustServoPositionDown() // Function to Expand and Retract the arm in downnward direction
{
  s1 = 10 * (constrain(x, 0, 135) + constrain(y, 0, 90) / 2);
  S1.writeMicroseconds(map(s1, 0, 2700, 500, 2500));
  s2 = (2 * x) - 15;
  S3.write(x - 65 + 90);

  int so2 = S2.read();
  while (so2 != s2)
  {
    so2 += (so2 > s2) ? -1 : 1;
    S2.write(so2);
    threads.delay(1);
  }
}

void changeOP() // Function to retract the arm Forcefully to Initial position, when Mode is Changed.
{
  const int s1_in = 0;
  const int s2_in = 0;
  const int s3_in = 0;
  int so1 = map(S1.read(), 0, 180, 0, 270);
  int so2 = S2.read();
  int so3 = S3.read();
  for (; so1 != s1_in || so2 != s2_in || so3 != s3_in;)
  {
    if (so1 > s1_in)
    {
      so1--;
    }
    else if (so1 < s1_in)
    {
      so1++;
    }
    S1.writeMicroseconds(map(so1, 0, 270, 500, 2500));

    if (so2 > s2_in)
    {
      so2--;
    }
    else if (so1 < s2_in)
    {
      so2++;
    }
    S2.writeMicroseconds(map(so2, 0, 180, 500, 2500));

    if (so3 > s3_in)
    {
      so3--;
    }
    else if (so3 < s3_in)
    {
      so3++;
    }
    S3.write(so3);
    threads.delay(30);
  }
  read_receiver();
  p2 = ReceiverValue.ch[0];
  prev_p2 = p2;
  x = 0;
  y = 0;
}

// Increase and Decrease the servo angle as direted by REMOTE for operation.
void straight()
{

  read_receiver();
  p1 = ReceiverValue.ch[2];
  while (p1 > 1600 && x < 90 && y < 90) // Extend
  {
    x++;
    y++;
    adjustServoPosition();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(25);
  }

  while (p1 < 1300 && x > 0 && y > 0) // Retract
  {
    x--;
    y--;
    adjustServoPosition();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(25);
  }
}
void up()
{

  read_receiver();
  p1 = ReceiverValue.ch[2];

  while (p1 > 1600 && x < 90 && y < 90)
  { // Extend
    x++;
    y++;
    adjustServoPositionUP();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(25);
  }

  while (p1 < 1300 && x > 0 && y > 0) // Retract
  {
    x--;
    y--;
    adjustServoPositionUP();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(25);
  }
}
void down()
{

  read_receiver();
  p1 = ReceiverValue.ch[2];

  while (p1 > 1600 && x < 135 && y < 120) // 90 // Extend
  {
    x++;
    y++;
    adjustServoPositionDown();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(25);
  }

  while (p1 < 1300 && x > 0 && y > 0) // Retract
  {
    x--;
    y--;
    adjustServoPositionDown();
    read_receiver();
    p1 = ReceiverValue.ch[2];
    threads.delay(25);
  }
}

// Function to Change the mode and perform operation accortdingly.
void operation()
{
  read_receiver();
  p2 = ReceiverValue.ch[0];

  while (true)
  {
    read_receiver();
    p2 = ReceiverValue.ch[0];
    while (p2 > 800 & p2 < 1200)
    {
      read_receiver();
      p2 = ReceiverValue.ch[0];
      if (!(prev_p2<p2 + 100 & prev_p2> p2 - 100))
      {
        changeOP();
      }
      up();
    }
    while (p2 > 1300 & p2 < 1600)
    {
      read_receiver();
      p2 = ReceiverValue.ch[0];
      if (!(prev_p2<p2 + 100 & prev_p2> p2 - 100))
      {
        changeOP();
      }
      straight();
    }
    while (p2 > 1700 & p2 < 2100)
    {
      read_receiver();
      p2 = ReceiverValue.ch[0];
      if (!(prev_p2<p2 + 100 & prev_p2> p2 - 100))
      {
        changeOP();
        S3.write(30);
      }
      down();
    }
  }

  threads.yield();
}

void setup()
{

  Serial.begin(9600);
  pinMode(18, INPUT);
  pinMode(33, INPUT);

  // Initialisation of Servos
  S1.attach(2, 500, 2500);
  S2.attach(3, 500, 2500);
  S3.attach(4, 500, 2500);
  S4.attach(5, 500, 2500);

  S1.write(0);
  S2.write(0);
  S3.write(0);
  S4.write(0);

  // Initialisation of SBUS communication
  sbus_rx.Begin();

  // Start CG balanceing Function
  threads.addThread(CG_Balance);
}

void loop()
{

  operation();
}