#include <Servo.h>
#include <ezButton.h>
#include <sbus.h>

bfs::SbusRx sbus_rx(&Serial1);
bfs::SbusData ReceiverValue;
int ch1Value;
int ch2Value;
int ch3Value;
int ch4Value;
int ch5Value;
int ch6Value;
int ch7Value;

ezButton button(13);

Servo S1;
Servo S2;
Servo S3;
Servo S4;

int pw, s1 = 0, s2 = 0, s3 = 0, s4 = 0, so1, so2, so3, s_, x = 0, y = 0, p1, p2, p3, a, b, prev_p3 = 1000;
int t1, t2, t3, t4;
float a1, a2, a3, a4, wa, w1, w2, w3, w4, bat_w, x1, x2, x3, x4, l1, l2, l3, CG;

void setup()
{
  Serial.begin(9600);
  S1.attach(2, 500, 2500); // 270
  S1.write(0);
  S2.attach(3, 500, 2500);
  S2.write(0);
  S3.attach(4, 500, 2500);
  S3.write(0);

  sbus_rx.Begin();
}
void loop()
{
  read_receiver();
  p1 = ReceiverValue.ch[0];
  p2 = ReceiverValue.ch[1];
  p3 = ReceiverValue.ch[2];

  s1 = x;
  if (s1 < 0)
  {
    s1 = 0;
  }
  S1.write(map(s1, 0, 270, 0, 180));

up:

  while (p1 > 1700) // extend
  {
    s1 = x;
    if (s1 < 0)
    {
      s1 = 0;
    }
    S1.write(map(s1, 0, 270, 0, 180));

    s2 = 2 * x;

    read_receiver();
    p3 = ReceiverValue.ch[2];
    s_ = map(p3, 1000, 2000, -10, 10);
    so3 = x + 90 + s_;
    S3.write(so3);

    so2 = S2.read();
    for (; so2 != s2;)
    {

      if (so2 == 180)
      {
        // Serial.println("max length");
        goto up;
      }

      if (so2 > s2)
      {
        so2--;
      }
      else if (so1 < s2)
      {
        so2++;
      }
      S2.write(so2);
      delay(2);
    }
    S2.write(so2);
    if (x == 90)
    {
      // Serial.println("max length");
      goto ext;
    }
    x++;
    read_receiver();
    p1 = ReceiverValue.ch[0];
    delay(15);
  }

  while (p2 > 1700) // retract
  {
    s1 = x;
    if (s1 < 0)
    {
      s1 = 0;
    }
    S1.write(map(s1, 0, 270, 0, 180));

    s2 = 2 * x;

    read_receiver();
    p3 = ReceiverValue.ch[2];
    s_ = map(p3, 1000, 2000, -10, 10);
    so3 = x + 90 + s_;
    S3.write(so3);

    so2 = S2.read();
    for (; so2 != s2;)
    {
      if (so2 == 0)
      {
        // Serial.println("min length");
        goto up;
      }
      if (so2 > s2)
      {
        so2--;
      }
      else if (so1 < s2)
      {
        so2++;
      }
      S2.write(so2);
      delay(2);
    }
    S2.write(so2);
    if (x == 0)
    {
      // Serial.println("min length");
      goto ext;
    }
    x--;
    read_receiver();
    p2 = ReceiverValue.ch[1];
    delay(15);
  }

ext:

  read_receiver();
  p3 = ReceiverValue.ch[2];
  s_ = map(p3, 1000, 2000, -10, 10);
  so3 = x + 90 + s_;
  S3.write(so3);

  Serial.print("p1: ");
  Serial.print(p1);
  Serial.print(" p2: ");
  Serial.print(p2);
  Serial.print(" p3: ");
  Serial.println(p3);
}

String read_str()
{
  while (Serial.available() == 0)
  {
  }
  String ans = Serial.readString();
  ans.trim(); // to remove any spaces
  return ans;
}
int mapWithStep(int value, int in_min, int in_max, int out_min, int out_max, int step)
{
  int mapped = map(value, in_min, in_max, out_min, out_max); // Perform linear mapping
  return (mapped / step) * step;                             // Adjust to the nearest step
}

void CG_Balance()
{
  so1 = S1.read();
  so2 = S2.read();
  so3 = S3.read();
  t2 = 90 - so1;
  t3 = 270 - so1 + so2;
  t4 = 90 - so1 + so2 - so3;

  a1 = x1;
  a2 = l1 + x2 * cos(t2);
  a3 = l1 + l2 * cos(t2) + x3 * cos(t3);
  a4 = l1 + l2 * cos(t2) + l3 * cos(t3) + x4 * cos(t4);

  wa = (w1 + w2 + w3 + w4);
  float aCG = (a1 * w1 + a2 * w2 + a3 * w3 + a4 * w4) / wa;

  float Battery_pos = (CG * (bat_w + wa) - wa * aCG) / bat_w;

  float min = 0;
  float max = 5;

  s4 = map(Battery_pos, min, max, 0, 180);
  S4.write(s4);
}
void read_receiver()
{
  if (sbus_rx.Read())
  {
    /* Grab the received data */
    ReceiverValue = sbus_rx.data();
    /* Display the received data */
    for (int8_t i = 0; i < ReceiverValue.NUM_CH; i++)
    {
      ReceiverValue.ch[i] = 1000 + ((ReceiverValue.ch[i] * 0.636) - 153);
      //        Serial.print(ReceiverValue.ch[i]);
      //        Serial.print("\t");
    }
    /* Display lost frames and failsafe data */
    Serial.print(ReceiverValue.lost_frame);
    Serial.print("\t");
    Serial.println(ReceiverValue.failsafe);
  }
}