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
uint16_t last;     //current target
}StepperdataVID;

unsigned long lastUpdateVID=0;

StepperdataVID stepperdataVID[] =
{
  //  {PIN Step PIN Dir}     arc     last
    { {    6,      7   },   315*12 ,  0   },  // example RPM
};

const int STEPPERZAHLVID = sizeof(stepperdataVID)/sizeof(stepperdataVID[0]);

SwitecX12 stepperVID[STEPPERZAHLVID]=                //make sure that the number of calls in this table matches the number of steppers in the list above
{
   SwitecX12(69,0,0) //dummy call
}; 

int prevVID=1;



void StepperVID_Zeroize(bool full)
{
  unsigned long now=0;
  for (byte x=0;x<STEPPERZAHLVID;x++)
  {
    // bring steppers to 0 position
    stepperVID[x].zero();
  }
  

  if (full)  //check full range of motors
  {
    delay(1000);
    //Set target position of all steppers to max
    //3 seconds to allow steppers to move to new position
    for (byte x=0;x<STEPPERZAHLVID;x++)
    { stepperVID[x].setPosition(stepperdataVID[x].arc-1);}
    now=millis();
    while (now>(millis()-3000))
    {
      for (byte x=0;x<STEPPERZAHLVID;x++)
        {
          stepperVID[x].update();
          delayMicroseconds(100);
        } 
    }
  
    //bring Steppers back down to 0
    //3 seconds to allow steppers to move to new position
    for (byte x=0;x<STEPPERZAHLVID;x++)
        {
          stepperVID[x].setPosition(1);
        }
    now=millis();
    while (now>(millis()-3000))
    {
      for (byte x=0;x<STEPPERZAHLVID;x++)
      {
        stepperVID[x].update();
        delayMicroseconds(100);
      }
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

///iniate a single step movement 
void UpdateStepperVID(byte pos)
{
  uint16_t newVal=atoi(datenfeld[pos].wert);
  if (newVal!=stepperdataVID[datenfeld[pos].target].last)
  {
    unsigned short NewStepperPos=map(newVal,0,65535,0,stepperdataVID[datenfeld[pos].target].arc);
    stepperVID[datenfeld[pos].target].setPosition(NewStepperPos);
    stepperdataVID[datenfeld[pos].target].last=newVal;
    lastUpdateVID=millis();
  }
  if (millis()-lastUpdateVID<5000)  //sleep if no new data since 5 seconds
   {stepperVID[datenfeld[pos].target].update();}
  
}
