//V0.9 12.3.22
//BMSAIT module to drive a 28BYJ48 stepper motor (28BCJ-48 with ULN2003 driver board)

#include <AccelStepper.h>
#define FULLSTEP 4
#define HALFSTEP 8


typedef struct
{
byte pIN[4];     //PINs the motor is connected to
uint16_t arc;    //max steps for the motor
float last;      //current target
byte calSwitch;  //switchID in module switches that will toggle the manual calibration
} StepperdataBYJ;

StepperdataBYJ stepperdataBYJ[] =
{
  //  {PIN1 PIN2 PIN3 PIN4}    arc      last   calSwitch
    { {  2,   3,   4,   5},    4096 ,   0,       99   }  // example: Compass
};
const int stepperzahlBYJ = sizeof(stepperdataBYJ)/sizeof(stepperdataBYJ[0]);

AccelStepper stepperBYJ[stepperzahlBYJ];


void SetupStepperBYJ()
{
for (byte stepperID=0;stepperID<stepperzahlBYJ;stepperID++)
  {
    stepperBYJ[stepperID]=AccelStepper(HALFSTEP, stepperdataBYJ[stepperID].pIN[0], stepperdataBYJ[stepperID].pIN[2], stepperdataBYJ[stepperID].pIN[1], stepperdataBYJ[stepperID].pIN[3]);
    stepperBYJ[stepperID].setMaxSpeed(2000.0);
    stepperBYJ[stepperID].setAcceleration(400);
    stepperBYJ[stepperID].setSpeed(200);
    stepperBYJ[stepperID].moveTo(0);
    #ifdef Switches
    for (byte switchID=0;switchID<anzSchalter;switchID++)
    {
     if (schalter[switchID].intCommand=51+stepperID){stepperdataBYJ[stepperID].calSwitch=switchID;}
    }
    #endif
  } 
}

void StepperBYJ_Zeroize(bool full)
{
  if (full)
  {
    for (byte stepperID=0;stepperID<stepperzahlBYJ;stepperID++)
    {
      long centerPoint=0;
      stepperBYJ[stepperID].move(stepperdataBYJ[stepperID].arc);
      long termination=millis()+4000;
      
      while((digitalRead(schalter[stepperdataBYJ[stepperID].calSwitch].pIN)==1)&&(millis()<termination));
      {
        stepperBYJ[stepperID].run();
      }
      if (millis()<termination)  //we did not go into the timeout. This means that the center pos was found
      { 
        centerPoint=stepperBYJ[stepperID].currentPosition();
        stepperBYJ[stepperID].stop();
      
        while(stepperBYJ[stepperID].isRunning())
        {stepperBYJ[stepperID].run();}
        
        stepperBYJ[stepperID].moveTo(centerPoint);
        stepperBYJ[stepperID].runToPosition();
        stepperBYJ[stepperID].setCurrentPosition(0);
      }
    }
  }
  else
  {
    for (byte stepperID=0;stepperID<stepperzahlBYJ;stepperID++)
    {
      stepperBYJ[stepperID].moveTo(0);
      stepperBYJ[stepperID].runToPosition();
    }
  }
}

void ManualCalibrationStepperBYJ(byte StepperID)
{
  if ((stepperdataBYJ[StepperID].calSwitch!=99) && (digitalRead(schalter[stepperdataBYJ[StepperID].calSwitch].pIN)==0))
  {
    int endPoint = stepperdataBYJ[StepperID].arc;
    while (digitalRead(schalter[stepperdataBYJ[StepperID].calSwitch].pIN)==0)
    { 
      if (stepperBYJ[StepperID].distanceToGo() == 0)
      {
        stepperBYJ[StepperID].setCurrentPosition(0);
        endPoint = -endPoint;
        stepperBYJ[StepperID].moveTo(endPoint);
        delay(1);
      }
      stepperBYJ[StepperID].run();
    }
    stepperBYJ[StepperID].stop();
    stepperBYJ[StepperID].setCurrentPosition(0);
  }
}


void StepperBYJ_FastUpdate()
{
  for (byte x=0;x<stepperzahlBYJ;x++)
    {stepperBYJ[x].run();}
}
  
void UpdateStepperBYJ(byte pos)
{
  //calibration with a toggle switch
  #ifdef Switches
    ManualCalibrationStepperBYJ(datenfeld[pos].target);
  #endif

  stepperBYJ[datenfeld[pos].target].run();
  uint16_t newVal=atoi(datenfeld[pos].wert); 
  
  if ((newVal<0) || (newVal>65535)) return;      //important: data variable needs to contain a position value (0..65535)
  if (stepperdataBYJ[datenfeld[pos].target].last!=newVal)  //check if target position changed 
  {
    stepperdataBYJ[datenfeld[pos].target].last=newVal;
    if (debugmode){DebugReadback(pos);}
    uint16_t tgt=map(newVal,0,65535,0,stepperdataBYJ[datenfeld[pos].target].arc);
    
    if (tgt-stepperBYJ[datenfeld[pos].target].currentPosition()>stepperdataBYJ[datenfeld[pos].target].arc/2)              //backward movement crossing null position 
      { stepperBYJ[datenfeld[pos].target].setCurrentPosition(stepperBYJ[datenfeld[pos].target].currentPosition()+stepperdataBYJ[datenfeld[pos].target].arc);}     
    else if (stepperBYJ[datenfeld[pos].target].currentPosition()-tgt>stepperdataBYJ[datenfeld[pos].target].arc/2)         //forward movement crossing null position
      {stepperBYJ[datenfeld[pos].target].setCurrentPosition(stepperBYJ[datenfeld[pos].target].currentPosition()-stepperdataBYJ[datenfeld[pos].target].arc);}         

    stepperBYJ[datenfeld[pos].target].moveTo(tgt);
  }
  stepperBYJ[datenfeld[pos].target].run();
}
