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
byte arctable;   //reference to a separate function to calibrate nonlinear movements
int minVal;      //minimum value of gauge
int maxVal;      //maximum value of gauge
float last;      //current target
} StepperdataX27;

unsigned long lastUpdateX27=0;

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


void StepperX27_Zeroize(bool mode)
{
  unsigned long now=0;
  for (byte x=0;x<stepperzahlX27;x++)
  { 
    //bring Steppers back down to 0
    //2 seconds to allow steppers to move to new position
    for (byte x=0;x<stepperzahlX27;x++)
    {
      stepperX27[x].setPosition(1);
    }
    if (mode)
    {
      now=millis();
      while (now>(millis()-2000))
      {
        for (byte x=0;x<stepperzahlX27;x++)
          {stepperX27[x].update();}
        delay(1);
      }
      delay(1000);
      for (byte x=0;x<stepperzahlX27;x++)
      {
        stepperX27[x].setPosition(stepperdataX27[x].arc-1);
      }
      now=millis();
      while (now>(millis()-2000))
      {
        for (byte x=0;x<stepperzahlX27;x++)
          {stepperX27[x].update();}
        delay(1);
      }
      delay(1000);
      for (byte x=0;x<stepperzahlX27;x++)
      {
        stepperX27[x].setPosition(1);
      }
      now=millis();
      while (now>(millis()-2000))
      {
        for (byte x=0;x<stepperzahlX27;x++)
          {stepperX27[x].update();}
        delay(1);
      }
    }
  }
}


void SetupStepperX27(void)
{
  for (byte x=0;x<stepperzahlX27;x++)
  {
    stepperX27[x]=SwitecX25(stepperdataX27[x].arc, stepperdataX27[x].pIN[0], stepperdataX27[x].pIN[1], stepperdataX27[x].pIN[2], stepperdataX27[x].pIN[3]); 
  } 
}


///iniate a single step movement 
void UpdateStepperX27(byte pos)
{
  float newVal=atof(datenfeld[pos].wert);
  if (newVal!=stepperdataX27[datenfeld[pos].target].last)
  {
    unsigned short  NewStepperPos=map(newVal, 0, 65535,0, stepperdataX27[datenfeld[pos].target].arc);
    stepperX27[datenfeld[pos].target].setPosition(NewStepperPos);
    stepperdataX27[datenfeld[pos].target].last=newVal;
    lastUpdateX27=millis();
  }
  if (millis()-lastUpdateX27<5000)  //sleep if no new data since 5 seconds
   {stepperX27[datenfeld[pos].target].update();}
  
}
