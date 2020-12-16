// settings and functions to drive multiple stepper motors (x27-168 recommended) using a VID6606 controller chip
// The VID6606 allows to drive the X25.168 at its range of 315 degrees with 12 steps per degree (3780 steps total).
//target= reference link to the line of the stepperdataVID table of this module
//ref2= not used
//ref3= not used
//ref4= not used
//ref5= not used

#include <SwitecX12.h>

typedef struct
{
byte pIN[2];    //PINs the motor is connected to (PIN1:on/off ; PIN2: direction)
uint16_t arc;   //max steps for the motor
byte arctable;  //reference to a separate function to calibrate nonlinear movements
int minVal;     //minimum Value of gauge
int maxVal;     //maximum Value of gauge
float last;     //current target
}StepperdataVID;

unsigned long lastUpdateVID=0;

StepperdataVID stepperdataVID[] =
{
  //  {PIN Step PIN Dir}     arc    arctable  MinVal  MaxVal   last
    { {    8,      9   },   315*12 ,   2,       0,     1200,    0   },  // example FTIT
    { {    6,      7   },   315*12 ,   1,       0,      110,    0   },  // example RPM
    { {    4,      5   },   225*12 ,   0,       0,        1,    0   },  // example NozzlePosition
    { {    2,      3   },   315*12 ,   0,       0,      100,    0   }   // example OIL Pressure
};

const int STEPPERZAHLVID = sizeof(stepperdataVID)/sizeof(stepperdataVID[0]);

SwitecX12 stepperVID[STEPPERZAHLVID]=                //make sure that the number of calls in this table matches the number of steppers in the list above
{
   SwitecX12(69,0,0) //dummy call
  ,SwitecX12(69,0,0) //dummy call 
  ,SwitecX12(69,0,0) //dummy call
  ,SwitecX12(69,0,0) //dummy call
}; 

int prevVID=1;



void StepperVID_Zeroize(void)
{
  unsigned long now=0;
  for (byte x=0;x<STEPPERZAHLVID;x++)
  {
    // bring steppers to 0 position
    stepperVID[x].zero();
  }
  delay(1000);
  
  //Set target position of all steppers to max
  //2 seconds to allow steppers to move to new position
  for (byte x=0;x<STEPPERZAHLVID;x++)
  { stepperVID[x].setPosition(stepperdataVID[x].arc-1);}
  now=millis();
  while (now>(millis()-2500))
  {
    for (byte x=0;x<STEPPERZAHLVID;x++)
      {
        stepperVID[x].update();
        delayMicroseconds(100);
      } 
  }

  //bring Steppers back down to 0
  //2 seconds to allow steppers to move to new position
  for (byte x=0;x<STEPPERZAHLVID;x++)
      {
        stepperVID[x].setPosition(1);
      }
  now=millis();
  while (now>(millis()-2500))
  {
    for (byte x=0;x<STEPPERZAHLVID;x++)
    {
      stepperVID[x].update();
      delayMicroseconds(100);
    }
  }  
}


void SetupStepperVID(void)
{
  for (byte x=0;x<STEPPERZAHLVID;x++)
  {
    stepperVID[x]=SwitecX12(stepperdataVID[x].arc, stepperdataVID[x].pIN[0], stepperdataVID[x].pIN[1]); 
  }
}

///allow adjustments to match an input value to the specific behavior of different gauges
int ConversionVID(float rawVal, byte pos, byte table=0)
{
  int ergebnis=0;
  if (table==0)
  {
    //linear movement
    if (stepperdataVID[datenfeld[pos].target].maxVal<=1)
      {ergebnis=map(rawVal*100,stepperdataVID[datenfeld[pos].target].minVal,stepperdataVID[datenfeld[pos].target].maxVal*100,1,stepperdataVID[datenfeld[pos].target].arc-1);}
    else if (stepperdataVID[datenfeld[pos].target].maxVal<=10)
      {ergebnis=map(rawVal*10,stepperdataVID[datenfeld[pos].target].minVal,stepperdataVID[datenfeld[pos].target].maxVal*10,1,stepperdataVID[datenfeld[pos].target].arc-1);}
    else
      {ergebnis=map(rawVal,stepperdataVID[datenfeld[pos].target].minVal,stepperdataVID[datenfeld[pos].target].maxVal,1,stepperdataVID[datenfeld[pos].target].arc-1);}
  }
  else if (table==1)  //RPM Gauge
  {
      //nonlinear movement RPM 
    //    0%    0° arc =   0%  =    0 Steps 
    //   70%  120° arc =  38%  = 1440 Steps
    //   80%  180° arc =  57%  = 2160 Steps
    //   90%  255° arc =  81%  = 3060 Steps
    //  100%  270° arc =  86%  = 3250 Steps 
    //  110%  315° arc = 100%  = 3780 Steps   
    
    if (rawVal<=70.0)
    {
      ergebnis=1+map(int(rawVal*10),0,700,0,1439);
    } 
    else if (rawVal<=80.0)
    {
      ergebnis=1440+map(int(rawVal*10),700,800,0,720);
    }
    else if (rawVal<=90.0)
    {
      ergebnis=2160+map(int(rawVal*10),800,900,0,900);
    }
    else if (rawVal<=100.0)
    {
      ergebnis=3060+map(int(rawVal*10),900,1000,0,190);
    }
    else if (rawVal>100.0)
    {
      ergebnis=3250+map(int(rawVal*10),1000,1100,0,530);
    }
  }
  else if (table==2)  //FTIT Gauge
  {
    //nonlinear movement FTIT
    //  200°    0° arc =   0%  =    0 Steps 
    //  700°   95° arc =  30%  = 1140 Steps
    // 1000°  275° arc =  87%  = 3300 Steps 
    // 1200°  315° arc =  100% = 3780 Steps 

    if (rawVal<=2.0)
    {
      ergebnis=1;
    } 
    else if (rawVal<=7.0)
    {
      ergebnis=1+map(int(100*rawVal),200,700,0,1139);
    }
    else if (rawVal<=10.0)
    {
      ergebnis=1140+map(int(100*rawVal),700,1000,0,2160);
    }
    else if (rawVal<=12.0)
    {
      ergebnis=3300+map(int(100*rawVal),1000,1200,0,480);
    }
    else
    {
      ergebnis=3780;
    }
  }
  return ergebnis;
}

///iniate a single step movement 
void UpdateStepperVID(byte pos)
{
  float newVal=atof(datenfeld[pos].wert);
  if (newVal!=stepperdataVID[datenfeld[pos].target].last)
  {
    
    int NewStepperPos=ConversionVID(newVal, pos, stepperdataVID[datenfeld[pos].target].arctable);
    stepperVID[datenfeld[pos].target].setPosition(NewStepperPos);
    stepperdataVID[datenfeld[pos].target].last=newVal;
    lastUpdateVID=millis();
  }
  if (millis()-lastUpdateVID<5000)  //sleep if no new data since 5 seconds
   {stepperVID[datenfeld[pos].target].update();}
  
}
