// settings and functions to drive a stepper motor (i.e. x27-168)
// standard X27.168 range is 315 degrees at 1/3 degree steps

#include <SwitecX25.h>

typedef struct
{
byte pIN[4];     //PINs the motor is connected to
uint16_t arc;    //max steps for the motor
byte arctable;   //reference to a separate function to calibrate nonlinear movements
int minVal;      //minimum Value of gauge
int maxVal;      //maximum Value of gauge
float last;      //current target
}StepperdataX27;

long lastUpdateX27=0;

StepperdataX27 stepperdataX27[] =
{
  //  {PIN1 PIN2 PIN3 PIN4}    arc   arctable  MinVal  MaxVal   last
    { {  2,   3,   4,   5   }, 315*3 ,   2,       0,     1200,    0   }  // example FTIT: Stepper on PIN=4,5,6 and 7, max steps=945, arctable=0, last value=0}
};

const int STEPPERZAHLX27 = sizeof(stepperdataX27)/sizeof(stepperdataX27[0]);

SwitecX25 stepperX27[STEPPERZAHLX27]=
{
  SwitecX25(945,0,0,0,0)
}; 


void SetupStepperX27(void)
{
  for (byte x=0;x<STEPPERZAHLX27;x++)
  {
    stepperX27[x]=SwitecX25(stepperdataX27[x].arc, stepperdataX27[x].pIN[0], stepperdataX27[x].pIN[1], stepperdataX27[x].pIN[2], stepperdataX27[x].pIN[3]); 
    // bring steppers to 0 position
    stepperX27[x].zero();
  } 
}

void StepperX27_Zeroize()
{
  long now=0;
  
  //Set target position of all steppers to max
  //2 seconds to allow steppers to move to new position
  for (byte x=0;x<STEPPERZAHLX27;x++)
  { stepperX27[x].setPosition(stepperdataX27[x].arc-1);}
  now=millis();
  while (now+2000>millis())
  {
    for (byte x=0;x<STEPPERZAHLX27;x++)
      {stepperX27[x].update();}
    delay(1);
  }

  //bring Steppers back down to 0
  //2 seconds to allow steppers to move to new position
  for (byte x=0;x<STEPPERZAHLX27;x++)
      {
        stepperX27[x].setPosition(1);
      }
  now=millis();
  while (now+2000>millis())
  {
    for (byte x=0;x<STEPPERZAHLX27;x++)
      {stepperX27[x].update();}
    delay(1);
  }
}

///allow adjustments to match an input value to the spefic behavior of different gauges
int ConversionX27(float rawVal, byte pos, byte table=0)
{

  int ergebnis=0;
  if (table==0)
  {
    //linear movement
    if (stepperdataX27[datenfeld[pos].ziel].maxVal<=1)
      {ergebnis=map(rawVal*100,stepperdataX27[datenfeld[pos].ziel].minVal,stepperdataX27[datenfeld[pos].ziel].maxVal*100,1,stepperdataX27[datenfeld[pos].ziel].arc-1);}
    else if (stepperdataX27[datenfeld[pos].ziel].maxVal<=10)
      {ergebnis=map(rawVal*10,stepperdataX27[datenfeld[pos].ziel].minVal,stepperdataX27[datenfeld[pos].ziel].maxVal*10,1,stepperdataX27[datenfeld[pos].ziel].arc-1);}
    else
      {ergebnis=map(rawVal,stepperdataX27[datenfeld[pos].ziel].minVal,stepperdataX27[datenfeld[pos].ziel].maxVal,1,stepperdataX27[datenfeld[pos].ziel].arc-1);}
  }
  else if (table==1)  //RPM Gauge
  {
    //nonlinear movement RPM
    //    0%     0 Steps (0° arc)
    //   65%   315 Steps (105° arc)
    //  100%   810 Steps (270° arc)
    //  110%   945 Steps (315° arc)
    
    if (rawVal<=65.0)
    {
      ergebnis=1+map(int(rawVal*10),0,650,0,314);
    } 
    else if (rawVal<=100.0)
    {
      ergebnis=315+map(int(rawVal*10),650,1000,0,495);
    }
    else if (rawVal>100.0)
    {
      ergebnis=810+map(int(rawVal*10),1000,1100,0,134);
    }
  }
  else if (table==2)  //FTIT Gauge
  {
    //nonlinear movement FTIT
    //  200°     0 Steps (0° arc)
    //  700°   285 Steps (95° arc)
    // 1000°   825 Steps (275° arc)
    // 1200°   945 Steps (315° arc)
    
    if (rawVal<=2.0)
    {
      ergebnis=1;
    } 
    else if (rawVal<=7.0)
    {
      ergebnis=map(int(100*rawVal),200,700,0,285);
    }
    else if (rawVal<=10.0)
    {
      ergebnis=285+map(int(100*rawVal),700,1000,0,540);
    }
    else if (rawVal<=12.0)
    {
      ergebnis=825+map(int(100*rawVal),1000,1200,0,119);
    }
    else
    {
      ergebnis=944;
    }
  }
  return ergebnis;
}

///iniate a single step movement 
void UpdateStepperX27(byte pos)
{
  float newVal=atof(datenfeld[pos].wert);
  if (newVal!=stepperdataX27[datenfeld[pos].ziel].last)
  {
    
    int NewStepperPos=ConversionX27(newVal, pos, stepperdataX27[datenfeld[pos].ziel].arctable);
    stepperX27[datenfeld[pos].ziel].setPosition(NewStepperPos);
    stepperdataX27[datenfeld[pos].ziel].last=newVal;
    lastUpdateX27=millis();
  }
  if (millis()-lastUpdateX27<5000)  //sleep if no new data since 5 seconds
   {stepperX27[datenfeld[pos].ziel].update();}
  
}
