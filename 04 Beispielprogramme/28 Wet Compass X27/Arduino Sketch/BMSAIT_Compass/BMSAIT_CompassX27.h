//V1.1 4.4.22
// settings and functios for a compass using a X-class stepper motor(i.e. x27-268)
//target= not used
//ref2= not used
//ref3= not used
//ref4= not used
//ref5= not used

#include "SwitecX27cont.h"   //modified Version of the X25 library 

#define COMPASSSTEPS1REV 1080   //Steps for one revolution of the stepper (usually 3 steps per degree)
#define COMPASSMOTORPIN1 2      //Pin 1 of the stepper motor
#define COMPASSMOTORPIN2 3      //Pin 2 of the stepper motor
#define COMPASSMOTORPIN3 4      //Pin 3 of the stepper motor
#define COMPASSMOTORPIN4 5      //Pin 4 of the stepper motor
#define COMPASSSENSORPIN A0     //Pin of a sensor to home the position of the stepper motor
#define COMPASSINVERT    false  //Indicates if the motor is moving backward (increase in steps moves motor counterclockwise)
#define COMPASSSENSORBLOCK false //Indicates if the homing sensor will send a LOW (false) or a HIGH (true) signal if activated 
#define COMPASSOFFSET    30    //Use this to finetune the compass center position (-540..+540 steps offset)

#if COMPASSINVERT 
  #define COMPASSZERO COMPASSSTEPS1REV*1.5
#else
  #define COMPASSZERO COMPASSSTEPS1REV/2
#endif

SwitecX27 compass(2*COMPASSSTEPS1REV,COMPASSMOTORPIN1,COMPASSMOTORPIN2,COMPASSMOTORPIN3,COMPASSMOTORPIN4);
uint16_t compass_oldVal =0;
unsigned long lastUpdateCompass=0;
unsigned long lastMessageCompass=0;


void CompassX27_Zeroize(bool full)
{

  uint16_t compass_spr=0;
  bool zero=false;
  
  if (COMPASSINVERT)
  {
    compass.center(2*COMPASSSTEPS1REV);
    compass.setPosition(0);
  }
  else
  {
    compass.center(0);
    compass.setPosition(2*COMPASSSTEPS1REV);  
  }
  
  while ((compass.currentStep!=compass.targetStep) && (zero==false))
  {
    compass.update();
    delay(1);
    if(COMPASSSENSORBLOCK)
      {if(analogRead(COMPASSSENSORPIN)>400){zero=true;}}
    else
      {if(analogRead(COMPASSSENSORPIN)<10){zero=true;}}
  }
  
  if (zero)
    {if (debugmode) SendMessage("Compass zeroize success",1);}
  else
    {if (debugmode) SendMessage("Compass zeroize error",1);}

  //Zeroize compass position in the stepper library
  compass.center(COMPASSZERO);
  
  if (debugmode) 
  {
  //determine actual steps per revolution  
    compass_spr=compass.currentStep;
    //move compass out of center (approx 90°)
    compass.setPosition(COMPASSZERO + COMPASSSTEPS1REV / 4);
    while ((compass.currentStep!=compass.targetStep))
    {
      compass.update();
      delay(1);
    }
    
    compass.setPosition(2*COMPASSSTEPS1REV);
    zero=false;
    while ((compass.currentStep!=compass.targetStep)&& (zero==false))
    {
      compass.update();
      if(COMPASSSENSORBLOCK)
        {if(analogRead(COMPASSSENSORPIN)>400){zero=true;}}
      else
        {if(analogRead(COMPASSSENSORPIN)<10){zero=true;}}
      delay(1);
      if (zero){compass_spr=compass.currentStep-compass_spr;}
    }
    
    if (zero)
    {
      SERIALCOM.print(VAR_BEGIN);
      SERIALCOM.print('t');
      SERIALCOM.print("Steps per revolution: ");
      SERIALCOM.print(compass_spr);
      SERIALCOM.println(VAR_ENDE);
    }
  } 
   
  if (COMPASSOFFSET!=0)
  {
    compass.setPosition(COMPASSZERO+COMPASSOFFSET);
    while ((compass.currentStep!=compass.targetStep))
    {
      compass.update();
      delay(1);
    }
    compass.center(COMPASSZERO);
  }
}

void SetupCompassX27()
{
  CompassX27_Zeroize(false);
}

void CompassX27_FastUpdate()
{
  compass.update();
}

void UpdateCompassX27(byte pos)
{
  //read target
  uint16_t compass_newVal=atoi(datenfeld[pos].wert);
 
  //check if a new heading is present
  if (compass_newVal!=compass_oldVal)
  {
    uint16_t target=0;
    if (COMPASSINVERT)
      {target=COMPASSZERO - map(compass_newVal,0,65535,0,COMPASSSTEPS1REV);}
    else
      {target=COMPASSZERO + map(compass_newVal,0,65535,0,COMPASSSTEPS1REV);}
    compass_oldVal=compass_newVal;
    
    if (debugmode)
    {
      if (millis()>lastMessageCompass+1000)
      {
        DebugReadback(pos);
        SERIALCOM.print(VAR_BEGIN);
        SERIALCOM.print('t');
        SERIALCOM.print("Pos: ");
        SERIALCOM.print(compass.currentStep);
        SERIALCOM.println(VAR_ENDE);
        lastMessageCompass=millis();
      }
    }

    //backward movement crossing 360° 
    if ((int)(target-compass.currentStep)>COMPASSSTEPS1REV/2)                
    {
      compass.center(compass.currentStep+COMPASSSTEPS1REV);
      if (debugmode)
      {
        if (COMPASSINVERT)
          {SendMessage("Compass transition forward",1);}
        else
          {SendMessage("Compass transition backward",1);}
      }      
    }
    //forward movement crossing 360°      
    else if ((int)(compass.currentStep-target)>COMPASSSTEPS1REV/2)         
    {
      compass.center(compass.currentStep-COMPASSSTEPS1REV);
      if (debugmode)
      {
        if (COMPASSINVERT)
          {SendMessage("Compass transition backward",1);}
        else
          {SendMessage("Compass transition forward",1);}
      }
    }         
    
    compass.setPosition(target);
  }
  compass.update();
}
