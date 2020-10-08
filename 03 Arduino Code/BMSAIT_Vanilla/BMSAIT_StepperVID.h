
// settings and functions to drive multiple stepper motors (x27-168 recommended) using a VID6606 controller chip
// The VID6606 allows to drive the X25.168 at its range of 315 degrees with 12 steps per degree (3780 steps total).

#include <SwitecX12.h>

typedef struct
{
byte pIN[2];    //PINs the motor is connected to
uint16_t arc;   //max steps for the motor
byte arctable;  //reference to a separate function to calibrate nonlinear movements
int minVal;     //minimum Value of gauge
int maxVal;     //maximum Value of gauge
float last;     //current target
}StepperdataVID;

long lastUpdateVID=0;

StepperdataVID stepperdataVID[] =
{
  //  {PIN Step PIN Dir}     arc    arctable  MinVal  MaxVal   last
    { {    8,      9   },   315*12 ,   2,       0,     1200,    0   },  // example FTIT: Stepper on PINs 8(high/low) 9(direction), max steps=945*3, arctable=2, last value=0}
    { {    6,      7   },   315*12 ,   1,       0,      110,    0   },  // example RPM: Stepper on PINs 6(high/low) 7(direction), max steps=945*3, arctable=1, last value=0}
    { {    4,      5   },   225*12 ,   0,       0,        1,    0   },  // example NozzlePosition: Stepper on PINs 4(high/low) 5(direction), max steps=945*3, arctable=0, last value=0}
    { {    2,      3   },   315*12 ,   0,       0,      100,    0   }   // example OIL Pressure: Stepper on PINs 2(high/low) 3(direction), max steps=945*3, arctable=0, last value=0}
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
 long now=0;
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
  while (now+2500>millis())
  {
    for (byte x=0;x<STEPPERZAHLVID;x++)
      {stepperVID[x].update();
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
  while (now+2500>millis())
  {
    for (byte x=0;x<STEPPERZAHLVID;x++)
      {stepperVID[x].update();
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
    if (stepperdataVID[datenfeld[pos].ziel].maxVal<=1)
      {ergebnis=map(rawVal*100,stepperdataVID[datenfeld[pos].ziel].minVal,stepperdataVID[datenfeld[pos].ziel].maxVal*100,1,stepperdataVID[datenfeld[pos].ziel].arc-1);}
    else if (stepperdataVID[datenfeld[pos].ziel].maxVal<=10)
      {ergebnis=map(rawVal*10,stepperdataVID[datenfeld[pos].ziel].minVal,stepperdataVID[datenfeld[pos].ziel].maxVal*10,1,stepperdataVID[datenfeld[pos].ziel].arc-1);}
    else
      {ergebnis=map(rawVal,stepperdataVID[datenfeld[pos].ziel].minVal,stepperdataVID[datenfeld[pos].ziel].maxVal,1,stepperdataVID[datenfeld[pos].ziel].arc-1);}
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
  if (newVal!=stepperdataVID[datenfeld[pos].ziel].last)
  {
    
    int NewStepperPos=ConversionVID(newVal, pos, stepperdataVID[datenfeld[pos].ziel].arctable);
    stepperVID[datenfeld[pos].ziel].setPosition(NewStepperPos);
    stepperdataVID[datenfeld[pos].ziel].last=newVal;
    lastUpdateVID=millis();
  }
  if (millis()-lastUpdateVID<5000)  //sleep if no new data since 5 seconds
   {stepperVID[datenfeld[pos].ziel].update();}
  
}
