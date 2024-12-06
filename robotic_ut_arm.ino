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
  S1.attach(3);
  S2.attach(5);
  S3.attach(6);
  button.setDebounceTime(50);
}

void loop()
{
  Serial.println("Chose operation: 1. roof, 2. corner, 3. wall, 4. floor");
  // need to code
  ip = read_val();
  switch (ip)
  {
  case 1:
    // roof
    Serial.println("roof");
    theta = -50;
    roof_operation(theta);
    break;
  case 2:
    // floor
    theta = 50;
    floor_operation(theta);
    break;
  case 3:
    // subs of corner
  slct_corner_mode:
    Serial.println("1. Upper corner(45), 2. lower corner(-45)");
    ip = read_val();
    switch (ip)
    {
    case 1:
      theta = 45; // need to cheack
      corner_operation(theta);
      break;
    case 2:
      theta = -45; // need to cheack
      corner_operation(theta);
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
    case 1:
      theta = atan(100 / (lo - 10));
      wall_operation(theta);
      break;
    case 2:
      theta = 0;
      wall_operation(theta);
      break;
    case 3:
      theta = atanf(-100 / (lo - 10));
      wall_operation(theta);
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

void wall_operation(int angle)
{
  // theta = atan(100/(lo-10));
  y = angle;
  so1 = S1.read();
  so2 = S2.read();
  so3 = S3.read();
  for (so1, so3; so1 <= 90 + y, so3 <= 90 - y; so1++, so3++)
  {
    S1.write(so1);
    S3.write(so3);
    delay(50);
  }

  for (x = 5; x <= 90; x++)
  {
    s1 = 90 + y + x;
    s2 = 2 * x;
    s3 = 90 - y + x;

    S1.write(s1);
    S2.write(s2);
    S3.write(s3);
    delay(50);

    if (x == 90)
    {
      Serial.println("Max length, no surface, retract");
      ans = read_str();
      goto out_wall;
    }

    button.loop();
    // if(button.isPressed())
    if (button.getState() == 0)
    {
    up_wall:
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
          goto out_wall;
        }
      }

      x++; // If button is not pressed, increase the arm angle/length
      s1 = 90 + y + x;
      s2 = 2 * x;
      s3 = 90 - y + x;

      if (x == 90) // maximum arm length condition
      {
        Serial.println("Max length while button pressed, retract");
        ans = read_str();
        delay(10);
        goto out_wall;
      }

      S1.write(s1);
      S2.write(s2);
      S3.write(s3);
      delay(10);
      goto up_wall;
    }
  }
out_wall:
  delay(1);
  retract_to_active_pos();
}

void roof_operation(int angle) // angle not less than 16
{
  y = angle;
  so1 = S1.read();
  so2 = S2.read();
  so3 = S3.read();
  for (so1, so3; so1 <= 90 + y, so3 <= 10; so1++, so3++)
  {
    S1.write(so1);
    S3.write(so3);
    delay(50);
  }

  for (x = 15; x <= 90; x++)
  {
    s1 = 90 + y + x;
    s2 = 2 * x;
    s3 = x - y;

    S1.write(s1);
    S2.write(s2);
    S3.write(s3);
    delay(50);

    if (x == 90)
    {
      Serial.println("Max length, no surface, retract");
      ans = read_str();
      goto out_roof;
    }

    button.loop();
    // if(button.isPressed())
    if (button.getState() == 0)
    {
    up_roof:
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
          goto out_roof;
        }
      }

      x++; // If button is not pressed, increase the arm angle/length
      s1 = 90 + y + x;
      s2 = 2 * x;
      s3 = x - y;

      if (x == 90) // maximul arm length condition
      {
        Serial.println("Max length while button pressed, retract");
        ans = read_str();
        delay(10);
        goto out_roof;
      }

      S1.write(s1);
      S2.write(s2);
      S3.write(s3);
      delay(10);
      goto up_roof;
    }
  }
out_roof:
  Serial.println("out");
  retract_to_active_pos();
}
void floor_operation(int angle)
{
  y = angle;
  so1 = S1.read();
  so2 = S2.read();
  so3 = S3.read();
  for (so1, so3; so1 <= 90 + y, so3 <= 180 - y; so1++, so3++)
  {
    S1.write(so1);
    S3.write(so3);
    delay(50);
  }

  for (x = 5; x <= 90; x++)
  {
    s1 = 90 + y + x;
    s2 = 2 * x;
    s3 = 180 - y + x;

    S1.write(s1);
    S2.write(s2);
    S3.write(s3);
    delay(50);

    if (x == 90)
    {
      Serial.println("Max length, no surface, retract");
      ans = read_str();
      goto out_floor;
    }

    button.loop();
    // if(button.isPressed())
    if (button.getState() == 0)
    {
    up_floor:
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
          goto out_floor;
        }
      }

      x++; // If button is not pressed, increase the arm angle/length
      s1 = 90 + y + x;
      s2 = 2 * x;
      s3 = 180 - y + x;

      if (x == 90) // maximul arm length condition
      {
        Serial.println("Max length while button pressed, retract");
        ans = read_str();
        delay(10);
        goto out_floor;
      }

      S1.write(s1);
      S2.write(s2);
      S3.write(s3);
      delay(10);
      goto up_floor;
    }
  }
out_floor:
  Serial.println("out");
  retract_to_active_pos();
}

void corner_operation(int angle)
{
  y = angle;
  so1 = S1.read();
  so2 = S2.read();
  so3 = S3.read();
  for (so1, so3; so1 <= 90 + y, so3 <= 90; so1++, so3++)
  {
    S1.write(so1);
    S3.write(so3);
    delay(50);
  }

  for (x = 5; x <= 90; x++)
  {
    s1 = 90 + y + x;
    s2 = 2 * x;
    s3 = 90 + x;

    S1.write(s1);
    S2.write(s2);
    S3.write(s3);
    delay(50);

    if (x == 90)
    {
      Serial.println("Max length, no surface, retract");
      ans = read_str();
      goto out_corner;
    }

    button.loop();
    // if(button.isPressed())
    if (button.getState() == 0)
    {
    up_corner:
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
          goto out_corner;
        }
      }

      x++; // If button is not pressed, increase the arm angle/length
      s1 = y + x;
      s2 = 2 * x;
      s3 = 90 + x;

      if (x == 90) // maximul arm length condition
      {
        Serial.println("Max length while button pressed, retract");
        ans = read_str();
        delay(10);
        goto out_corner;
      }

      S1.write(s1);
      S2.write(s2);
      S3.write(s3);
      delay(10);
      goto up_corner;
    }
  }
out_corner:
  Serial.println("out");
  retract_to_active_pos();
}

void retract_to_active_pos() // need to rectify
{
  for (x; x >= 5; x--)
  {
    s1 = y + x; // y
    s2 = 2 * x;
    s3 = 90 + x;
    S1.write(s1);
    S2.write(s2);
    S3.write(s3);
    Serial.println(x);
    delay(50);
  }
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