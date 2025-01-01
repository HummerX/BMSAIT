//V2.2 1.1.25
// settings and functions to drive a stepper motor (i.e. x27-168)
// standard X27.168 range is 315 degrees at 1/3 degree steps
//target= reference link to the line of the stepperdata table of this module
//ref2= not used
//ref3= not used
//ref4= not used
//ref5= not used

#include <SwitecX25.h>

typedef struct
{
byte pIN[4];        //PINs the motor is connected to
uint16_t arc;       //max steps for the motor
bool invert;         //mark if the motor has its zero position on the clockwise end of the movement arc
unsigned int last;  //current target
} StepperdataX27;

unsigned long lastUpdateX27=0;
unsigned long lastMessageX27=0;

StepperdataX27 stepperdataX27[] =
{
  //  {PIN1 PIN2 PIN3 PIN4}    arc    invert   last
    { {  2,   3,   4,   5   }, 315*3 , false,    0   }  // example: RPM
// ,{ {  x,   x,   x,   x   }, 315*3 , false,    0   }  //uncomment this to add a second motor. You need to change the x by the correct PINs
};

const int stepperzahlX27 = sizeof(stepperdataX27)/sizeof(stepperdataX27[0]);

SwitecX25 stepperX27[stepperzahlX27]=
{
  SwitecX25(945,0,0,0,0)
//,SwitecX25(945,0,0,0,0)  //uncomment this to add a second motor. no changes to the field are needed
}; 


void SetupStepperX27(void)
{
  for (byte x=0;x<stepperzahlX27;x++)
  {
    stepperX27[x]=SwitecX25(stepperdataX27[x].arc, stepperdataX27[x].pIN[0], stepperdataX27[x].pIN[1], stepperdataX27[x].pIN[2], stepperdataX27[x].pIN[3]); 
  }
}

void StepperX27_Zeroize(bool m)
{
  unsigned long now=0;
  bool busy=true;
  for (byte motor=0;motor<stepperzahlX27;motor++)
  { 
    //bring Steppers back down to 0
    if (stepperdataX27[motor].invert) 
      stepperX27[motor].setPosition(stepperdataX27[motor].arc-1);
    else
      stepperX27[motor].setPosition(1);
  }
  now=millis();
  busy=true;
  while ((now+3000>millis())&& busy)
  {
    busy=false;
    for (byte motor=0;motor<stepperzahlX27;motor++)
    {
      stepperX27[motor].update();
      if (stepperX27[motor].dir!=0){busy=true;}
    } 
    delayMicroseconds(100);
  }
  
  if (m)
  {
    for (byte motor=0;motor<stepperzahlX27;motor++)
    {      
      if (stepperdataX27[motor].invert) 
        {stepperX27[motor].setPosition(1);}
      else
        {stepperX27[motor].setPosition(stepperdataX27[motor].arc-1);}

      busy=true;
      while ((now+3000>millis())&& busy)
      {
        busy=false;
        for (byte motor=0;motor<stepperzahlX27;motor++)
        {
          stepperX27[motor].update();
          if (stepperX27[motor].dir!=0){busy=true;}
        } 
        delayMicroseconds(100);
      }
      
      delay(500);
      if (stepperdataX27[motor].invert) 
        {stepperX27[motor].setPosition(stepperdataX27[motor].arc-1);}
      else
        {stepperX27[motor].setPosition(1);}
      busy=true;
      while ((now+3000>millis())&& busy)
      {
        busy=false;
        for (byte motor=0;motor<stepperzahlX27;motor++)
        {
          stepperX27[motor].update();
          if (stepperX27[motor].dir!=0){busy=true;}
        } 
        delayMicroseconds(100);
      }
    }
  }
}

void StepperX27_FastUpdate()
{
  for (byte motor=0;motor<stepperzahlX27;motor++)
  {stepperX27[motor].update();}
}

///iniate a single step movement 
void UpdateStepperX27(byte pos)
{
  uint16_t newVal=atoi(datenfeld[pos].wert);

  if (newVal<0)newVal=0;
  if (newVal>65535)newVal=65535;
  if (newVal!=stepperdataX27[datenfeld[pos].target].last)
  {
    uint16_t  newStepperPos=map(newVal, 0, 65535,0, stepperdataX27[datenfeld[pos].target].arc);
      if (debugmode)
    {
      if (millis()>lastMessageX27+1000)
      {
        DebugReadback(pos);
        SERIALCOM.print(VAR_BEGIN);
        SERIALCOM.print('t');
        SERIALCOM.print(newStepperPos) ;
        SERIALCOM.println(VAR_ENDE);
        lastMessageX27=millis();
      }
    }
    if (stepperdataX27[datenfeld[pos].target].invert) 
      stepperX27[datenfeld[pos].target].setPosition(stepperdataX27[datenfeld[pos].target].arc - newStepperPos);
    else
      stepperX27[datenfeld[pos].target].setPosition(newStepperPos);
      
    stepperdataX27[datenfeld[pos].target].last=newVal;
    lastUpdateX27=millis();
  }
  if (millis()-lastUpdateX27<5000)  //sleep if no new data since 5 seconds
   {stepperX27[datenfeld[pos].target].update();}
}
