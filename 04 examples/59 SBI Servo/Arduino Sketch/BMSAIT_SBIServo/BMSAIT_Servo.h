// settings and functions to drive servo motors (directly connected to the arduino)
//target= reference link to the line of the servodata table of this module
//ref2= Pos of 3D variable (1651)
//ref3= Main Power variable (1242)
//ref4= not used
//ref5= not used

#include <Servo.h>

#define SERVODELAY 200        //time (ms) the servo will pause after movement
#define SBIDELAY 500          //time (ms) the off-flag will show up when the SBI moves from closed to open or vice versa

#define SBI_PIN             3 //Arduino PIN the servo is conncted to
#define SBI_OFF_ANGLE      90 //servo movement angle for the "off" symbol
#define SBI_CLOSED_ANGLE  135 //servo movement angle for the "closed" symbol
#define SBI_OPEN_ANGLE     55 //servo movement angle for the "open" symbol


Servo SBIservo;

byte SBIPosNow=3;
byte SBIPosLast=0;
long sBImovement=0;
long SBIMessage=0;

void SetupServo()
{  
  SBIservo.attach(SBI_PIN);
  SBIservo.write(SBI_CLOSED_ANGLE);
  delay(1000);
  SBIservo.write(SBI_OPEN_ANGLE);
  delay(1000);
  SBIservo.write(SBI_OFF_ANGLE);
  delay(200);
  SBIservo.detach();
}

void Servo_Zeroize(bool full)
{

  if (!SBIservo.attached())
      {SBIservo.attach(SBI_PIN);}  //reactivate servo
  SBIservo.write(SBI_CLOSED_ANGLE);
  if (full)
  {
    delay(1000);
    SBIservo.write(SBI_OPEN_ANGLE);
    delay(1000);
    SBIservo.write(SBI_OFF_ANGLE);
    delay(200);
    SBIservo.detach();
  }
}


void Servo_FastUpdate()
{}


void UpdateServo(byte id)
{
  if (sBImovement+10000<millis()) 
  {
    if(SBIservo.attached())
    {
      SBIservo.detach(); 
      if (debugmode) SendMessage("Detach",1);
    }
  }  
  
  unsigned short sBICurrValue=atoi(datenfeld[id].wert);
  SBIPosLast = SBIPosNow;
    
  if ((datenfeld[datenfeld[id].ref2].wert[0]=='F') || (datenfeld[datenfeld[id].ref3].wert[0]=='F'))  
  {
    SBIPosNow=1;      //player is either not in 3D or the a/c power is still off
  }      
  else if (sBICurrValue>600)    // SB more than 1% open
  {
    if (SBIPosLast==0)
    {
      sBImovement=millis();
      SBIPosNow=1;
    }
    else if (millis()<sBImovement+SBIDELAY)
      {SBIPosNow=1;}    //temporary switch to off 
    else
      {SBIPosNow=2;}    
  }      
  else if (sBICurrValue<600)    // SB less than 1% open
  {
    if (SBIPosLast==2)
    {
      sBImovement=millis();
      SBIPosNow=1;
    }
    else if (millis()<sBImovement+SBIDELAY)
      {SBIPosNow=1;}  //temporary switch to off 
    else
      {SBIPosNow=0;}
  }      
  else
  {
    SBIPosNow=1;
  }
  
  if (debugmode)
  {
    if (millis()>SBIMessage+1000)
    {
      SBIMessage=millis();
      DebugReadback(neuer_wert.varNr);
      SERIALCOM.print(VAR_BEGIN);
      SERIALCOM.print('t');
      SERIALCOM.print(sBICurrValue) ;
      SERIALCOM.println(VAR_ENDE);
    }
  }
  
  if (SBIPosNow!=SBIPosLast) 
  { 
    unsigned short winkel=90;
    if (!SBIservo.attached())
    {
      SBIservo.attach(SBI_PIN); //reactivate servo 
      if (debugmode) SendMessage("Attach",1);
    }
    
    if (SBIPosNow==0) 
      {
        winkel=SBI_CLOSED_ANGLE;
      }          
    else if (SBIPosNow==1)
      {
        winkel=SBI_OFF_ANGLE;
      }
    else if (SBIPosNow==2)
      {
        winkel=SBI_OPEN_ANGLE;
      }
    else
      {winkel=90;}
      
    SBIservo.write(winkel);
    delay(200);
  }
}
