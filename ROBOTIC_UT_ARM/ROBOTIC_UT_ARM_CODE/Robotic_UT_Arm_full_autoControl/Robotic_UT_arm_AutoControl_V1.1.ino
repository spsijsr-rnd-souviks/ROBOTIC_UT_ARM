#include <Servo.h>
#include <ezButton.h>
#include <math.h>

ezButton button(13); // another pin to GND

// #define Button 13
// #define Button2 4

Servo S1;
Servo S2;
Servo S3;
// Servo S4;
String ans;
int x, y, offst = 45, theta, lo = 40, ip;
int s1, s2, s3;
int so1, so2, so3;
uint16_t start, now, interval;

void setup()
{
  Serial.begin(9600);
  int x = 5, y = 10;
  S1.attach(3);
  int s1 = 0; // x+y;
  s1 = map(s1, 0, 225, 0, 180);
  S1.write(s1);
  S2.attach(5);
  S2.write(0); //(2*x);
  S3.attach(6);
  S3.write(0); //(90+x);
  button.setDebounceTime(50);
}

void loop()
{
  so1 = S1.read();
  so1 = map(so1, 0, 180, 0, 225);
  so2 = S2.read();
  so3 = S3.read();
  Serial.print(so1);
  Serial.print(" ");
  Serial.print(so2);
  Serial.print(" ");
  Serial.println(so3);
  Serial.println("Chose operation: 1. roof, 2. floor, 3. corner, 4. wall");
  // need to code
  ip = read_val();
  switch (ip)
  {
  case 1:
    // roof
    Serial.println("roof"); //(y<0)
    // theta = -10;
    // operation(0,1,1,theta);
    roof_floor(0, 10, 179);
    break;
  case 2:
    // floor y<0
    // theta = -10;
    // operation(1.9,1,1,theta); //1.9 instead of 2, as 1.9*90=171~180-10, there will be 10 deg of lavarage in S3(270) for full scale
    roof_floor(95, 10, 120);
    break;
  case 3:
    // subs of corner
  slct_corner_mode:
    Serial.println("1. Upper corner(45), 2. lower corner(-45)");
    ip = read_val();
    switch (ip)
    {
    case 1:
      theta = -45;               // need to cheack
      operation(1, 0, 1, theta); // upper
      break;
    case 2:
      theta = 45;                // need to cheack
      operation(1, 0, 1, theta); // lower
      break;
    case 3:
      break;
    default:
      goto slct_corner_mode;
    }
    break;
  case 4:
    // subs of wall
  slct_wall_mode:
    Serial.println("1. up 1m, 2. straight, 3. down 1m, 4. exit");
    ip = read_val();
    switch (ip)
    {
    case 1:                      // up
      theta = -9;                // atan(100/(lo-10));
      operation(1, 1, 1, theta); // up
      break;
    case 2: // straight
      theta = 0;
      operation(1, 0, 0, theta); // straight
      break;
    case 3:                      // down
      theta = 10;                // atanf(100/(lo-10));
      operation(1, 1, 1, theta); // down
      break;
    case 4:
      break;
    default:
      Serial.println("enter valid input");
      goto slct_wall_mode;
    }
    break;
  }
  delay(50);
}

void operation(int a, int b, int c, int y) // angle not less than 16
{
  x = 5;
  initial(c * y + x, 2 * x, 90 * a + x - b * y);

  for (x = 5; x <= 90; x++)
  {
    s1 = c * y + x; // 90+c*y+x;
    // s1= map(s1,0,225,0,180);
    s2 = 2 * x;
    s3 = 90 * a + x - b * y;

    S1.write(map(s1, 0, 225, 0, 180));
    S2.write(s2);
    S3.write(s3);
    delay(100);
    Serial.print("s1: ");
    Serial.print(s1);
    Serial.print(" s2: ");
    Serial.print(s2);
    Serial.print(" S3: ");
    Serial.println(s3);

    if (x == 90)
    {
      Serial.println("Max length, no surface, retract");
      ans = read_str();
      goto out_operation;
    }

    button.loop();
    // if(button.isPressed())
    if (button.getState() == 0)
    {
    up_operation:
      Serial.println("again");
      delay(50); // for debouncing
      start = millis();

      // while(button.isPressed())
      while (button.getStateRaw() == 0)
      {
        Serial.println("stall");
        now = millis();
        interval = now - start;

        if (interval >= 3000) // if button is pressed continously for 3 sec, our reading is done
        {
          Serial.println("3 sec done, press any button to retract the arm");
          delay(10);
          ans = read_str(); // to hold at that position, if we need to take continous reading along that length
          goto out_operation;
        }
      }

      x++;            // If button is not pressed, increase the arm angle/length
      s1 = c * y + x; // 90+y+x;
      // s1= map(s1,0,225,0,180);
      s2 = 2 * x;
      s3 = 90 * a + x - b * y; // x-y;
      Serial.print("s1: ");
      Serial.print(s1);
      Serial.print(" s2: ");
      Serial.print(s2);
      Serial.print(" S3: ");
      Serial.println(s3);

      if (x == 90) // maximul arm length condition
      {
        Serial.println("Max length while button pressed, retract");
        ans = read_str();
        delay(10);
        goto out_operation;
      }

      S1.write(map(s1, 0, 225, 0, 180));
      S2.write(s2);
      S3.write(s3);
      delay(50);
      goto up_operation;
    }
  }
out_operation:
  Serial.println("out");
  retract_to_active_pos();

  S1.write(s1);
  S2.write(s2);
  S3.write(s3);
}

void roof_floor(int d, int x1, int mx) ////put value for servo 1, roof : 10+45, floor : 170+45
{
  x = x1 + d;

  initial(x, 2 * x1, x);

  for (int i = 0; i <= 160; i++)
  {
    s1 = x; // 90+y+x;
    // s1= map(s1,0,225,0,180);
    s2 = 2 * x1 + i; // 2*x;
    s3 = x + i;      // 10+i;//x-(y);

    S1.write(map(s1, 0, 225, 0, 180));
    S2.write(s2);
    S3.write(s3);
    delay(50);

    if (s2 == mx) //(s2==179)//(x==90)
    {
      Serial.println("Max length, no surface, retract");
      ans = read_str();
      goto out_roof_floor;
    }

    button.loop();
    // if(button.isPressed())
    if (button.getState() == 0)
    {
    up_roof_floor:
      Serial.println("again");
      delay(50); // for debouncing
      start = millis();

      // while(button.isPressed())
      while (button.getStateRaw() == 0)
      {
        Serial.println("stall");
        now = millis();
        interval = now - start;

        if (interval >= 3000) // if button is pressed continously for 3 sec, our reading is done
        {
          Serial.println("3 sec done, press any button to retract the arm");
          delay(10);
          ans = read_str(); // to hold at that position, if we need to take continous reading along that length
          goto out_roof_floor;
        }
      }

      s2++;
      s3++;
      Serial.print("s1: ");
      Serial.print(s1);
      Serial.print(" s2: ");
      Serial.print(s2);
      Serial.print(" S3: ");
      Serial.println(s3);

      if (s2 == 179) //(x==90) //maximul arm length condition
      {
        Serial.println("Max length while button pressed, retract");
        ans = read_str();
        delay(10);
        goto out_roof_floor;
      }

      // S1.write(s1);
      S2.write(s2);
      S3.write(s3);
      delay(50);
      goto up_roof_floor;
    }
  }
out_roof_floor:
  Serial.print("x ");
  Serial.print(x);
  Serial.print(" y ");
  Serial.print(y);
  Serial.println(" out");
  retract_to_active_pos();
  S1.write(map(s1, 0, 225, 0, 180));
  S2.write(s2);
  S3.write(s3);
  // Serial.print("fe");
}

void initial(int si1, int si2, int si3)
{
  s1 = S1.read();
  s2 = S2.read();
  s3 = S3.read();
  for (; s1 != si1 || s2 != si2 || s3 != si3;)
  {
    if (s1 < si1)
      s1++;
    else if (s1 > si1)
      s1--;
    // s1= map(s1,0,225,0,180);
    S1.write(map(s1, 0, 225, 0, 180));
    // s1 = map(s1, 0, 180,0,255);

    if (s2 < si2)
      s2++;
    else if (s2 > si2)
      s2--;
    S2.write(s2);

    if (s3 < si3)
      s3++;
    else if (s3 > si3)
      s3--;
    S3.write(s3);
    delay(75);
  }
}

void retract_to_active_pos() // need to rectify
{
  s1 = S1.read();
  Serial.print(s1);
  s1 = map(s1, 0, 180, 0, 225);
  Serial.print(" mapped to ");
  Serial.println(s1);
  s2 = S2.read();
  s3 = S3.read();

  for (; s1 != 0 || s2 != 0 || s3 != 0;)
  {
    if (s1 > 0)
    {
      s1--;
    }
    else if (s1 < 0)
    {
      s1++;
    }
    // s1= map(s1,0,225,0,180);
    S1.write(map(s1, 0, 225, 0, 180));
    // s1 = map(s1, 0, 180,0,255);
    // Serial.println(s1);

    if (s2 > 0)
    {
      s2--;
    }
    else if (s2 < 0)
    {
      s2++;
    }
    S2.write(s2);
    // Serial.println(s2);

    if (s3 > 0)
    {
      s3--;
    }
    else if (s3 < 0)
    {
      s3++;
    }
    S3.write(s3);
    // Serial.println(s3);
    delay(75);
    Serial.print("s1: ");
    Serial.print(s1);
    Serial.print(" s2: ");
    Serial.print(s2);
    Serial.print(" S3: ");
    Serial.println(s3);
  }
  Serial.print("ot");
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

float read_val()
{
  boolean inputReceived = false;
  float x = 0.0;
  // Loop until valid input is received
  while (!inputReceived)
  {
    if (Serial.available() > 0)
    {
      // Read the input from Serial Monitor
      String input = Serial.readStringUntil('\n');
      x = input.toFloat(); // Convert input string to float

      // Check if valid float input
      if (!isnan(x))
      {
        inputReceived = true; // Input is valid
      }
      else
      {
        Serial.println("Invalid input. Please enter a valid float number, please re-enter:");
      }
    }
  }
  return x;
}