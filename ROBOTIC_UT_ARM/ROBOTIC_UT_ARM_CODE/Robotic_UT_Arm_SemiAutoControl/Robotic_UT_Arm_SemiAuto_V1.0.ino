#include<Servo.h>
#include <ezButton.h>
ezButton button(13);
Servo S1;
Servo S2;
Servo S3;

Servo S4;


int pw,s1=0,s2=0,s3=0,so1, so2, so3, x=0,y=0,p1,p2,p3,a,b,prev_p3=1000;

void setup()
{
  Serial.begin(9600);
  pinMode(11, INPUT);
  pinMode(10, INPUT); // Set pin 10 as an input
  pinMode(9, INPUT);
  S1.attach(3, 500, 2500); //270
  S1.write(0);
  S2.attach(5, 500, 2500);
  S2.write(0);
  S3.attach(6, 500, 2500);
  S3.write(0);

  pw = 500;
  
  S1.writeMicroseconds(pw);
  S2.writeMicroseconds(pw);
  S2.writeMicroseconds(pw);
}
void loop()
{
  p1 = pulseIn(9, HIGH); // swc, extend & retract, CH: 5
  p2 = pulseIn(10, HIGH);// vba, angle to extend, CH: 6
  p3 = pulseIn(11,HIGH);//Thottle, CH: 3

  b = map(p2, 980, 2000, -45,45);
  s1=x+b;
  if(s1<0){s1=0;}
  S1.write(map(s1, 0,270,0,180));
 // delay(1);

up:
    

   while(p1<1100)//extend
   { 
    p2 = pulseIn(10, HIGH);
    b = map(p2, 980, 2000, -45,45);
    s1=x+b;
    if(s1<0){s1=0;}
    S1.write(map(s1, 0,270,0,180));

    s2=2*x;

    // p3 = pulseIn(11,HIGH);
    // int pw3 = map(p3, 1000, 2000, 0,180);
    // S3.write(pw3);

    prev_p3=p3;
    p3 = pulseIn(11,HIGH);
    if(p3<(prev_p3-10) || prev_p3>(p3+10))
  {
    sev3();
  }
  prev_p3=p3;
    S3.write(so3);
    
    so2= S2.read();
    for(;so2!=s2;)
    {

      if(so2 == 180)
      {
        //Serial.println("max length");
        goto up;
      }

      if(so2>s2){so2--;}
      else if(so1<s2){so2++;}
      S2.write(so2);
      //delay(10);
    }
    S2.write(so2);
    if(x==90)
    {
     // Serial.println("max length");
      goto ext;
    }
    x++;
    p1 = pulseIn(9, HIGH);
   // Serial.println(p1);
   }

    while(p1>1800)//retract
   {
    p2 = pulseIn(10, HIGH);
    b = map(p2, 980, 2000, -45,45);
    s1=x+b;
    if(s1<0){s1=0;}
    S1.write(map(s1, 0,270,0,180));
    s2=2*x;

    // p3 = pulseIn(11,HIGH);
    // int pw3 = map(p3, 1000, 2000, 0,180);
    // S3.write(pw3);
    prev_p3=p3;
  p3 = pulseIn(11,HIGH);
  if(p3<(prev_p3-10) || prev_p3>(p3+10))
  {
    sev3();
    //S3.write(so3);
  }
  prev_p3=p3;

    //sev3();
    S3.write(so3);

    so2= S2.read();
    for(;so2!=s2;)
    {

      if(so2 == 0)
      {
       // Serial.println("min length");
        goto up;
      }

      if(so2>s2){so2--;}
      else if(so1<s2){so2++;}
      S2.write(so2);
      //delay(10);
    }
    S2.write(so2);
    if(x==0)
    {
      //Serial.println("min length");
      goto ext;
    }
    x--;
    p1 = pulseIn(9, HIGH);
    // Serial.println(p1);
   }

ext:

  prev_p3=p3;
   p3 = pulseIn(11,HIGH);
  if(p3<(prev_p3-10) || p3>(prev_p3+10))
  {
    sev3();
    //S3.write(so3);
  }
  prev_p3=p3;

  S3.write(so3);

  Serial.print("p1: ");Serial.print(p1);
  Serial.print(" p2: ");Serial.print(p2);
  Serial.print(" p3: ");Serial.println(p3);
}

void sev3()
{
  Serial.println(p3);
  
  // p2 = pulseIn(10, HIGH);
  // b = map(p2, 980, 2000, -45,45);
  // s1=x+b;
  // if(s1<0){s1=0;}

  int p3 = pulseIn(11, HIGH);
  int ip = map(p3,900,2100, 0, 180);
  so3 = S3.read();
  for(;so3!=ip;)
  {
    // p2 = pulseIn(10, HIGH);
    // b = map(p2, 980, 2000, -45,45);
    // s1=x+b;
    // if(s1<0){s1=0;}
    // S1.write(s1);

   // Serial.println(p3);
    if (so3>ip) 
    {
      so3--;
    }
    else if (so3<ip) {
    so3++;
    }
    S3.write(so3);

    //delay(1);

    p3 = pulseIn(11, HIGH);
    ip = map(p3,900,2100, 0, 180);
    so3 = S3.read();
  }
  S3.write(so3);
}


String read_str()
{ 
  while(Serial.available()==0){
  }
  String ans = Serial.readString();
  ans.trim(); // to remove any spaces
  return ans;
}
int mapWithStep(int value, int in_min, int in_max, int out_min, int out_max, int step) {
  int mapped = map(value, in_min, in_max, out_min, out_max); // Perform linear mapping
  return (mapped / step) * step; // Adjust to the nearest step
}

// void CG_Balance()
// {
//   so1 = S1.read();
//   so2 = S2.read();
//   so3 = S3.read();
//   t2 = 90 - so1;
//   t3 = 270 - so1 + so2;
//   t4 = 90 -so1 + so2 - so3;

//   a1 = x1;
//   a2 = l1+ x2*cos(t2);
//   a3 = l1 + l2*cos(t2) + x3*cos(t3);
//   a4 = l1 + l2*cos(t2) + l3*cos(t3) + x4*cos(t4);


//   wa = (w1 + w2 + w3 + w4);
//   float aCG = (a1*w1 + a2*w2 + a3*w3 + a4*w4)/wa;

//   float Battery_pos = (CG*(bat_w + wa)-wa*aCG)/bat_w;

//   float min = ;
//   float max = ;

//   s4 = map(Battery_pos, min, max, 0, 180);
//   S4.write(s4);
// }