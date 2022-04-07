//V2.1 22.3.22
// settings and functions to drive multiple stepper motors (x27-168 recommended) using a VID6606 controller chip
// The VID6606 allows to drive the X25.168 at its range of 315 degrees with 12 steps per degree (3780 steps total).

//target= reference link to the line of the stepperdataVID table of this module
//ref2= force updates (i.E. hydraulics)
//ref3= not used
//ref4= not used
//ref5= not used

#include <SwitecX12.h>

typedef struct
{
byte pIN[2];       //PINs the motor is connected to (PIN1:on/off ; PIN2: direction)
uint16_t arc;      //max steps for the motor
boolean inverted;  //change motor direction (zero position is on the clockwise end)
uint16_t last;     //current target
}StepperdataVID;


unsigned long lastUpdateVID=0;
unsigned long forceUpdate=0;

StepperdataVID stepperdataVID[] =
{
  //  {PIN Step PIN Dir}     arc    inverted   last
    { {    6,      7   },   315*12 ,  false,     0   },  // example RPM
};

const int STEPPERZAHLVID = sizeof(stepperdataVID)/sizeof(stepperdataVID[0]);

SwitecX12 stepperVID[STEPPERZAHLVID]=                //make sure that the number of calls in this table matches the number of steppers in the list above
{
   SwitecX12(69,0,0) //dummy call
}; 

int prevVID=1;


void SetupStepperVID(void)
{
  for (byte x=0;x<STEPPERZAHLVID;x++)
  {
    stepperVID[x]=SwitecX12(stepperdataVID[x].arc, stepperdataVID[x].pIN[0], stepperdataVID[x].pIN[1]); 
  }
}

void StepperVID_Zeroize(bool full)
{
  unsigned long now=0;
  bool busy=true;
  for (byte motor=0;motor<STEPPERZAHLVID;motor++)
  {
    // bring steppers to 0 position
    if (stepperdataVID[motor].inverted)
      {stepperVID[motor].setPosition(stepperdataVID[motor].arc-1);}
    else
      {stepperVID[motor].setPosition(1);}
  }
  now=millis();
  
  while ((now+3000>millis())&& busy)
  {
    busy=false;
    for (byte motor=0;motor<STEPPERZAHLVID;motor++)
    {
      stepperVID[motor].update();
      if (stepperVID[motor].dir!=0){busy=true;}
    } 
    delayMicroseconds(100);
  }
  
  if (full)  //check full range of motors
  {
    delay(1000);
    //Set target position of all steppers to max
    //3 seconds to allow steppers to move to new position
    for (byte motor=0;motor<STEPPERZAHLVID;motor++)
    { 
      //Bring steppers to max position
      if (stepperdataVID[motor].inverted)
        {stepperVID[motor].setPosition(1);}
      else
        {stepperVID[motor].setPosition(stepperdataVID[motor].arc-1);}
    }
    now=millis();
    busy=true;
    while ((now+3000>millis())&& busy)
    {
      busy=false;
      for (byte motor=0;motor<STEPPERZAHLVID;motor++)
        {
          stepperVID[motor].update();
          if (stepperVID[motor].dir!=0){busy=true;}
        } 
        delayMicroseconds(100);
    }
  
    // bring steppers to 0 position
    for (byte motor=0;motor<STEPPERZAHLVID;motor++)
    { 
      if (stepperdataVID[motor].inverted)
        {stepperVID[motor].setPosition(stepperdataVID[motor].arc-1);}
      else
        {stepperVID[motor].setPosition(1);}
    }
    now=millis();
    busy=true;
    while ((now+3000>millis())&& busy)
    {
      busy=false;
      for (byte motor=0;motor<STEPPERZAHLVID;motor++)
      {
        stepperVID[motor].update();
        if (stepperVID[motor].dir!=0){busy=true;}
      } 
      delayMicroseconds(100);
    }
  } 
}

void StepperVID_FastUpdate()
{
  for (byte motor=0;motor<STEPPERZAHLVID;motor++)
  {stepperVID[motor].update();}
}

///iniate a single step movement 
void UpdateStepperVID(byte pos)
{
  uint16_t newVal=atoi(datenfeld[pos].wert);
  uint16_t NewStepperPos=0; 
  if (newVal!=stepperdataVID[datenfeld[pos].target].last)
  {
    NewStepperPos=map(newVal,0,65535,0,stepperdataVID[datenfeld[pos].target].arc);
    if (stepperdataVID[datenfeld[pos].target].inverted) 
      {stepperVID[datenfeld[pos].target].setPosition(stepperdataVID[datenfeld[pos].target].arc-NewStepperPos);}
    else
      {stepperVID[datenfeld[pos].target].setPosition(NewStepperPos);}
    stepperdataVID[datenfeld[pos].target].last=newVal;
    lastUpdateVID=millis();
  }
  
  if (millis()-lastUpdateVID<5000)  //sleep if no new data since 5 seconds
   {stepperVID[datenfeld[pos].target].update();}


  if (millis()>forceUpdate+5000)  //force update every 5 seconds for required motors (i.e. hydraulics)
   {
    if (datenfeld[pos].ref2==1)
      {stepperdataVID[datenfeld[pos].target].last=0;}
    forceUpdate=millis();
   }
}
