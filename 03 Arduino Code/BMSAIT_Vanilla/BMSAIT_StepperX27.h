//V2.1 20.3.22
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
byte pIN[4];     //PINs the motor is connected to
uint16_t arc;    //max steps for the motor
float last;      //current target
} StepperdataX27;

unsigned long lastUpdateX27=0;
unsigned long lastMessageX27=0;

StepperdataX27 stepperdataX27[] =
{
  //  {PIN1 PIN2 PIN3 PIN4}    arc         last
    { {  2,   3,   4,   5   }, 315*3 ,      0   }  // example: FTIT
};

const int stepperzahlX27 = sizeof(stepperdataX27)/sizeof(stepperdataX27[0]);

SwitecX25 stepperX27[stepperzahlX27]=
{
  SwitecX25(945,0,0,0,0)
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
  for (byte x=0;x<stepperzahlX27;x++)
  { 
    //bring Steppers back down to 0
    //2 seconds to allow steppers to move to zero position
    stepperX27[x].setPosition(1);
 
    now=millis();
    while (now+3000>millis())
    {
      stepperX27[x].update();
      delayMicroseconds(500);
    }
    
    if (m)
    {
      stepperX27[x].setPosition(stepperdataX27[x].arc-1);

      now=millis();
      while (now+2000>millis())
      {
        stepperX27[x].update();
        delayMicroseconds(500);
      }
      
      delay(500);
      stepperX27[x].setPosition(1);
      now=millis();
      while (now+2000>millis())
      {
        stepperX27[x].update();
        delayMicroseconds(500);
      }
    }
  }
}

void StepperX27_FastUpdate()
{
  for (byte x=0;x<stepperzahlX27;x++)
  {stepperX27[x].update();}
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
    
    stepperX27[datenfeld[pos].target].setPosition(newStepperPos);
    stepperdataX27[datenfeld[pos].target].last=newVal;
    lastUpdateX27=millis();
  }
  if (millis()-lastUpdateX27<5000)  //sleep if no new data since 5 seconds
   {stepperX27[datenfeld[pos].target].update();}
}
