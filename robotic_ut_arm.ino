#include <Servo.h>
#include <ezButton.h>

ezButton button(13); // another pin to GND

// #define Button 13
// #define Button2 4

Servo S1;
Servo S2;
Servo S3;
//Servo S4;
String ans;
int x,y;
int s1, s2, s3;
int so1, so2, so3;
uint16_t start, now, interval;

void setup() {
  Serial.begin(9600);
  S1.attach(3);
  S2.attach(5);
  S3.attach(6);
  button.setDebounceTime(50);
}

void loop() {
  Serial.println("Start");

  initial_position();//it will read servo's current position, then positioned it in programmed initial position

  //read the current state of the servo
  so1=S1.read();
  so2=S2.read();
  so3=S3.read();

  //y = 45;//it will decide the mode(0, 45, 90)
  // for(int i = 0; i<=(y+10); i++)
  // {
  //   S3.write(i);
  // }

  //x= 10;//not less than 10 or experimented value
  // int s1 = y+x;
  // int s2 = 2*x;
  // int s3 = 90+x;

  // S1.write(s1);
  // S2.write(s2);
  // S3.write(s3);
  //retract_to_active_pos();

lab:  
  Serial.println("Select MODE:1(-45), 2(0), 3(90)");
  float mode = read_val();

  if(mode==1)
  {
    y=0;
    S1_positioning(so1); //Read the current position of S1, then position it according to the given mood
  }

  else if (mode==2) 
  {
    y=45;
    S1_positioning(so1);
  }

  else if (mode==3) 
  {
    y=90;
    S1_positioning(so1);
  }

  else 
  {
    Serial.println("wrong input");
    goto lab;
  }

  // String ans = read_str();
  // delay(10);

  //Action mode: from the current position, arm will extend untill full length or button(control feed back) is pressed continously for 3 sec
  for (x = 10; x<=90; x++)
  {
    s1 = y+x;
    s2 = 2*x;
    s3 = 90+x;

    S1.write(s1);
    S2.write(s2);
    S3.write(s3);
    delay(50);

    if(x==90)
    {
      Serial.println("Max length, no surface, retract");
      ans = read_str();
      goto out;
    }

    button.loop();
    //if(button.isPressed())
    if(button.getState()==0)
    {
up:
      Serial.println("again");
      delay(50);//for debouncing
      start = millis();
    
      //while(button.isPressed())
      while (button.getStateRaw()==0) 
      {
        Serial.println("stall");
        now=millis();
        interval = now - start;

        if(interval >= 3000) //if button is pressed continously for 3 sec, our reading is done
        {
          Serial.println("3 sec done, press any button to retract the arm");
          delay(10);
          ans=read_str(); //to hold at that position, if we need to take continous reading along that length
          goto out;
          
        }
      }

      x++; //If button is not pressed, increase the arm angle/length
      s1 = y+x;
      s2 = 2*x;
      s3 = 90+x;

      if(x==90) //maximul arm length condition
      {
        Serial.println("Max length while button pressed, retract");
        ans = read_str();
        delay(10);
        goto out;
      }

      S1.write(s1);
      S2.write(s2);
      S3.write(s3);
      delay(10);
      goto up;
      
    }
  }
  out:

  //retract
  Serial.println("out");
  retract_to_active_pos(); //Arm will retract to it's action position

label1:
  Serial.println("1. Go to initial position. /n 2. Go to Mode Select");

  float m = read_val();
  if(m==1)
  {
   //do nothig, go at the start of the void loop
  }
  else if(m==2)
  {
    goto lab; //go to mood select line
  }
  else 
  {
    Serial.println("Enter Valid input");
    goto label1; 
  }
  delay(100);
}

void S1_positioning(int so)
{

  if (so>y)
  {
    for(so; so>=(y+10);so--)
    {
      S1.write(so);
      delay(50);
    }
  }
  else if(so<y)
  {
    for(so;so<=(y+10); so++)
    {
      S1.write(so);
      delay(50);
    }
  }
  else 
  {
    for(so; so<=10;so++)
    {
      S1.write(so);
    }
  }

}

void retract_to_active_pos()
{
  for(x; x>=10; x--) 
  {
    s1 = y+x;//y
    s2 = 2*x;
    s3 = 90+x;
    S1.write(s1);
    S2.write(s2);
    S3.write(s3);
    Serial.println(x);
    delay(50);
  }
}

void initial_position()
{
  so1=S1.read();
  so2=S2.read();
  so3=S3.read();

  x=10;
  
  //For S1
  if(so1>45)
  {
    for(so1;so1>=(45+x);so1++)
    S1.write(so1);
    delay(50);
  }
  else if(so1<45)
  {
    for(so1; so1<=(45+x); so1--)
    {
      S1.write(so1);
      delay(50);
    }
  }
  else 
  {
    for(so1; so1>=x; so1++)
    {
      S1.write(so1);
      delay(50);
    }
  }

  //For S2
    if(so2>2*x)
  {
    for(so2;so2>=2*x;so2++)
    S2.write(so2);
    delay(50);
  }
  else if(so2<2*x)
  {
    for(so2; so2<=2*x; so2--)
    {
      S2.write(so2);
      delay(50);
    }
  }
  else 
  {
  //   for(so2; so2>=90; so2++)
  //   {
  //     S2.write(so2);
  //     delay(50);
  //   } 
  }

  //For S3
  if(so3>x+90)
  {
    for(so3;so3>=(90+x);so3++)
    S3.write(so3);
    delay(50);
  }
  else if(so3<x+90)
  {
    for(so3; so3<=(90+x); so3--)
    {
      S3.write(so3);
      delay(50);
    }
  }
  else 
  {
    // for(so3; so3>=x; so3++)
    // {
    //   S3.write(so3);
    //   delay(50);
    // }
  }
}

void rest_pos() //not used
{
  //for(x; x>=10; x--)
  for(s1, s2, s3; s1>0,s2>=10,s3>=10; s1--, s2--, s3--) 
  {
    // s1 = 90+x; //y+x;
    // s2 = 2*x;
    // s3 = 90+x;
    S1.write(s1);
    S2.write(s2);
    S3.write(s3);
    delay(50);
  }
}

void rest_to_active() //not used
{
  for(s1, s2, s3; s1<=y+10, s2<=(2*10), s3<=(90+10); s1++, s2++, s3++)
  {
    S1.write(s1);
    S2.write(s2);
    S3.write(s3);
    delay(50);
  }

}

String read_str()
{ 
  while(Serial.available()==0){
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
  while (!inputReceived) {
    if (Serial.available() > 0) {
      // Read the input from Serial Monitor
      String input = Serial.readStringUntil('\n');
      x = input.toFloat(); // Convert input string to float
      
      // Check if valid float input
      if (!isnan(x)) {
        inputReceived = true; // Input is valid
      } else {
        Serial.println("Invalid input. Please enter a valid float number, please re-enter:");
      }
    }
  }
  return x;
}
