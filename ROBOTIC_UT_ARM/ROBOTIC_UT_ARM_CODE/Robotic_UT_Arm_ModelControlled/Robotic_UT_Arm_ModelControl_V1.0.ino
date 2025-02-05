#include <Servo.h>
#include <ezButton.h>
Servo S1;
Servo S2;
Servo S3;
int pw, po1, po2, po3,s1,s2,s3,so1,so2,so3,som1,som2,som3;
ezButton button(13);
void setup() {
  Serial.begin(9600);
  button.setDebounceTime(50);
  S1.attach(3, 500, 2500); //270
  S2.attach(5, 500, 2500);
  S3.attach(6, 500, 2500);
  pw = 500;
  
  S1.writeMicroseconds(pw);
  S2.writeMicroseconds(pw);
  S2.writeMicroseconds(pw);

}

void loop() {
  

  int ip1 = analogRead(A0);
  int pw1 = map(ip1, 0, 1023, 500, 2000);
  s1=map(ip1,0,1023,0,180);
  S1.write(s1);
  //S1.writeMicroseconds(pw1);
  

  int ip2 = analogRead(A1);
  int pw2 = map(ip2, 0, 1023, 500, 2350);
  s2 = map(ip2, 0,1023,0,180);
  S2.write(s2);
  //S2.writeMicroseconds(pw2);

  int ip3 = analogRead(A2);
  int pw3 = map(ip3, 0, 1023, 700, 2500);
  s3=map(ip3,0,1023,0,180);
  S3.write(s3);
  //S3.writeMicroseconds(pw3);

  // s2=map(ip2, 0, 1023, 0, 180);
  // so2= S2.read();
  // while(s2!=so2)
  // {

  //   ip2 = analogRead(A1);
  //   pw2 = map(ip2, 0, 1023, 500, 2350);
  //   s2=map(ip2, 0, 1023, 0, 180);

  //   if(so2>s2)
  //   {
  //     so2--;
  //     S2.write(so2);
  //   }
  //   else if (so2<s2) 
  //   {
  //     so2++;
  //     S2.write(so2);
  //   }
  //   delay(100);
  // }
  // S2.write(s2);

  //S2.write(map(ip2, 0, 1023, 0, 180));
  //S2.writeMicroseconds(pw2);

  

  uint16_t start = millis();
  button.loop();
  while (button.getStateRaw()==0) 
      {
        po1 = ip1;
        Serial.print("last value: ");Serial.print(po1);
        Serial.print(" || stall time: ");
        uint16_t now=millis();
        uint16_t interval = now - start;
        Serial.println(interval);
      }
  // ip1 = analogRead(A0);
  // pw1 = map(ip1, 0, 1023, 500, 2000);
  // s1=map(ip1,0,1023,0,180);
  

  // ip2 = analogRead(A1);
  // pw2 = map(ip2, 0, 1023, 500, 2350);
  // s2=map(ip2,0,1023,0,180);

  // ip3 = analogRead(A2);
  // pw3 = map(ip3, 0, 1023, 700, 2500);
  // s3=map(ip3,0,1023,0,180);

  // som1 = S1.readMicroseconds();
  // som2 = S2.readMicroseconds();
  // som3 = S3.readMicroseconds();
  // while(som1!=pw1 || som2 != pw2 || som3 != pw3)
  // {
  //   if(som1> pw1)
  //   {
  //     som1--;
  //     S1.writeMicroseconds(som1);
  //   }
  //   else if(som1< pw1)
  //   {
  //     som1++;
  //     S1.writeMicroseconds(som1);
  //   }
    
  //   if(som2> pw2)
  //   {
  //     som2--;
  //     S2.writeMicroseconds(som2);
  //   }
  //   else if(som2< pw2)
  //   {
  //     som2++;
  //     S2.writeMicroseconds(som2);
  //   }
  //   if(som3> pw3)
  //   {
  //     som3--;
  //     S3.writeMicroseconds(som3);
  //   }
  //   else if(som3< pw3)
  //   {
  //     som3++;
  //     S3.writeMicroseconds(som3);
  //   }
  //   delay(50);
  //   ip1 = analogRead(A0);
  //   pw1 = map(ip1, 0, 1023, 500, 2000);

  //   ip2 = analogRead(A1);
  //   pw2 = map(ip2, 0, 1023, 500, 2350);

  //   ip3 = analogRead(A2);
  //   pw3 = map(ip3, 0, 1023, 700, 2500);
  // }

  // S1.writeMicroseconds(pw1);
  // S2.writeMicroseconds(pw2);
  // S3.writeMicroseconds(pw3);

  Serial.println();
  Serial.print("ip1: ");Serial.print(ip1);
  Serial.print(", pw1: "); Serial.print(pw1);
  Serial.print(", Angel 1: "); Serial.print(map(S1.read(), 0, 180, 0, 270));
  Serial.print(" || ip2: ");Serial.print(ip2);
  Serial.print(", pw2: "); Serial.print(pw2);
  Serial.print(", Angel 2: "); Serial.print(map(S2.read(), 0, 180, 0, 180));
  Serial.print(" || ip3: ");Serial.print(ip3);
  Serial.print(", pw3: "); Serial.print(pw3);
  Serial.print(", Angel 3: "); Serial.println(map(S3.read(), 0, 180, 0, 180));
  delay(100);
  
}
//void Encoder_servo1()

