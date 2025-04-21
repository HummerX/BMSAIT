// Version: 1.0 18.04.2025

// settings and functions to drive servo motors (directly connected to the arduino)

//prerequisites:
//- module switches needs to be activated in the UserConfig
//- Servo library must be installed! https://docs.arduino.cc/libraries/servo/!

//required ettings in the data table of UserConfig module
//type: 61
//target= points to a line in the magswitchData table in this module
//ref2= not used
//ref3= not used
//ref4= not used
//ref5= not used

#include <Servo.h> 

typedef struct
{
   byte dataPos;   // position of the mag switch status variable in the data table of the module "user config" (first line=0)
   byte switchPos; // position of the mag switch in the switches data table of the module "switches" (fist line=0)
   byte servoPin;  // the Pin the servo is attached to (this needs to be a pwm capable Pin of your Arduino!)
   byte servoMin;  // leftmost angle of the servo motor (0..servoMax)
   byte servoMax;  // rightmost angle of the servo motor (servoMin..max angle of your hardware (i.e. 180))
   unsigned long magSwitchNextStep;
   byte magSwitchStatus;
}  MagSwitchData;

//User Settings
#define MAGSWITCHDELAY 500    //the amount of time the Arduino will wait before a magSwitch contact will be released

MagSwitchData magSwitchData[] =
{  //dataPos switchPos   servoPin  servoMin  servoMax magSwitchNextStep magSwitchStatus
   {    0,      0,          3,        10,      170,            0,              0}  
  ,{    0,      1,          3,        10,      170,            0,              0}  
};
const int magSwitchCount = sizeof(magSwitchData)/sizeof(magSwitchData[0]);
//User Settings


//don't change anything beyond this point
Servo magSwitchServo[magSwitchCount];

void MagSwitch_FastUpdate()
{
  for (byte id=0;id<magSwitchCount;id++)
  {
    //non-blocking servo movement sequence
    switch (magSwitchData[id].magSwitchStatus)
    {
      case 0: 
      case 1:
        //do nothing
        break;
      case 2:
        if (debugmode) {SendMessage("MagSwitchReset",1);}
        magSwitchServo[id].attach(magSwitchData[id].servoPin);
        magSwitchServo[id].write(magSwitchData[id].servoMin); //move to leftmost position
        magSwitchData[id].magSwitchStatus=3;   
        magSwitchData[id].magSwitchNextStep=millis()+ 2* MAGSWITCHDELAY;    // delay until next step is 1 second
        break;
      case 3:
        if (millis() > magSwitchData[id].magSwitchNextStep)    
          {magSwitchData[id].magSwitchStatus=4;}            // delay has passed - move to next step
        break;
      case 4:
        magSwitchServo[id].write(magSwitchData[id].servoMax); //move to rightmost position 
        magSwitchData[id].magSwitchStatus=5;
        magSwitchData[id].magSwitchNextStep=millis()+ 2* MAGSWITCHDELAY;   // delay until next step is 1 second
        break;
      case 5:
        if (millis()>magSwitchData[id].magSwitchNextStep)
          {magSwitchData[id].magSwitchStatus=6;}           // delay has passed - move to next step
        break;
      case 6:
        magSwitchServo[id].write(magSwitchData[id].servoMin+(magSwitchData[id].servoMax-magSwitchData[id].servoMin)/2);  //move to median
        magSwitchData[id].magSwitchStatus=7;
        magSwitchData[id].magSwitchNextStep=millis()+ MAGSWITCHDELAY;   // delay until next step are 0.2 seconds
        break;
      case 7:
        if (millis()>magSwitchData[id].magSwitchNextStep)
          {magSwitchData[id].magSwitchStatus=8;}           // delay has passed - move to next step
        break;
      case 8:
        magSwitchServo[id].detach();                        //movement complete, deactivate motor
        magSwitchData[id].magSwitchStatus=0;
        magSwitchData[id].magSwitchNextStep=0;
        break;
    }
  }
}

void UpdateMagSwitches()
{
  for (byte id=0;id<magSwitchCount;id++)
  {
    if (magSwitchData[id].magSwitchStatus==0)
    {
      //check if the motor movement needs to be activated. This happens when the defined switch is active and the magnetic switch bit of BMS is off 
      if (switches[magSwitchData[id].switchPos].lastPINState==0) //check if switch is active
      { //switch is active
        if(datenfeld[magSwitchData[id].dataPos].wert[0]=='F')  //check if mag switch is inactive in BMS
        {    
          magSwitchData[id].magSwitchStatus=1;
          magSwitchData[id].magSwitchNextStep=millis()+MAGSWITCHDELAY; //initiate delay - give BMS some time to register that a MagSwitch got moved
        }
      }
    }
    if ((magSwitchData[id].magSwitchStatus==1)&&(millis()>magSwitchData[id].magSwitchNextStep))  //check if BMS still calls for a switch release
    {
      if (switches[magSwitchData[id].switchPos].lastPINState==0) 
      {  //switch is still active
        if(datenfeld[magSwitchData[id].dataPos].wert[0]=='F')  
          {magSwitchData[id].magSwitchStatus=2;}   //MagSwitch in BMS ist still inactive
        else
          {magSwitchData[id].magSwitchStatus=0;}   //MagSwitch is now active - cancel procedure
      } 
      else
        {magSwitchData[id].magSwitchStatus=0;}     //MagSwitch has been released - cancel procedure
    }
  }
}

void SetupMagSwitch()
{  

 //prüfen, ob Motor doppelt vorkommt -> Initialisierung nur einmal!
  byte MagPins[magSwitchCount];
  for (byte id=0;id<magSwitchCount;id++)
    {MagPins[id]=0;}
    
  byte pos=0;
  
  if (magSwitchCount==1)
    {MagPins[0]=magSwitchData[0].servoPin;}
  else
  {
    for (byte lauf=0;lauf<magSwitchCount;lauf++) 
    {
      boolean found=false;
      for (byte lauf2=1;lauf2<magSwitchCount;lauf2++)
      {
        if (MagPins[lauf2]== magSwitchData[lauf].servoPin)
         {found=true;}
      }
      if (!found)
      {
        MagPins[pos]=lauf;
        pos++;   
      }
    }
  } 
  
  for (byte id : MagPins)  //PRÜFEN: IST ID richtig oder MagPins[ID]?
  {
    if (id!=0)
    {
     magSwitchServo[id].attach(magSwitchData[id].servoPin); 
     magSwitchServo[id].write(magSwitchData[id].servoMin); //move to min
    }
  }
  delay(1000);
  for (byte id : MagPins)
  {
   if (id!=0)
    {magSwitchServo[id].write(magSwitchData[id].servoMax);} //move to max
  }
  delay(1000);
  for (byte id : MagPins)
  {
   if (id!=0)
    {magSwitchServo[id].write(magSwitchData[id].servoMin+(magSwitchData[id].servoMax-magSwitchData[id].servoMin)/2);}  //move to median
  }
  delay(200);
  for (byte id : MagPins)
  {
   if (id!=0)
    {magSwitchServo[id].detach();}
  }
}
