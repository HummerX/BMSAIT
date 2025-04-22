//V1.0 22.07.24
// settings and functions to drive an air core motor with a CS4192 controller chip
//target= ID if multiple air core motors are used on one Arduino; this reflects the table position of the motor data on the following table aircoreData (start with 0)
//ref2= not used
//ref3= not used
//ref4= not used
//ref5= not used

#include <SPI.h>
#include "AircoreCS.h"

void AirCore_Zeroize(bool);

typedef struct
{
byte pIN;     // Cable select pin
uint16_t arc;    //max arc of the motor (max value: 1023 for a 360° movement)
float last;      //current target
} AirCoreData;

unsigned long lastUpdateAC=0;
unsigned long lastMessageAC=0;

AirCoreData aircoreData[] =   //fill this table with the specific data of each motor connected to your Arduino. For the <PIN>, enter the cable select (CS) pin you used to connect the CS4182 controller. <Arc> should be 1013. leave 0 for <last>.
{
  // PIN   arc  last
    { 10, 1023,  0   }  // example: RPM
    //,{ 9, 1023,  0   }  // example: FTIT
};
const int aircoreNum = sizeof(aircoreData)/sizeof(aircoreData[0]);

AircoreCS aircoreMotor[aircoreNum]=
{
  AircoreCS(13)
  //,AirCoreCS(13) //uncomment if you want to dive multiple motors on one Arduino. You need one "blank" entry (Pin 13) for each motor connected to the Arduino
}; 

void SetupAirCore(void)
{
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setBitOrder(MSBFIRST); 
  
  for (byte x=0;x<aircoreNum;x++)
  {
    aircoreMotor[x]=AircoreCS(aircoreData[x].pIN); 
    aircoreMotor[x].pulse();
  } 
  AirCore_Zeroize(true);  //optional setting to show that the motor is alive on startup. Put in a "//" to deactivate this line if you don't want that.
}

void AirCore_Zeroize(bool m)
{
  unsigned long now=0;
  for (byte x=0;x<aircoreNum;x++)
  { 
    //move motor to zero/max  positions
    //wait 2 seconds to allow steppers to perform movement
    aircoreMotor[x].setPosition(1);

    //just zeroize
    now=millis();
    while (now+2000>millis())
    {
      aircoreMotor[x].update();
      delayMicroseconds(500);
    }
    
    if (m) //full calibration
    {
      aircoreMotor[x].setPosition(aircoreData[x].arc-1);

      now=millis();
      while (now+2000>millis())
      {
        aircoreMotor[x].update();
        delayMicroseconds(500);
      }
      
      delay(500);
      aircoreMotor[x].setPosition(1);
      now=millis();
      while (now+2000>millis())
      {
        aircoreMotor[x].update();
        delayMicroseconds(500);
      }
    }
  }
}

void AirCore_FastUpdate()
{
  for (byte x=0;x<aircoreNum;x++)
  {aircoreMotor[x].update();}
}

///iniate a single step movement 
void UpdateAirCore(byte pos)
{
  uint16_t newVal=atoi(datenfeld[pos].wert);
  if (newVal<0)newVal=0;
  if (newVal>65535)newVal=65535;
  if (newVal!=aircoreData[datenfeld[pos].target].last)
  {
    uint16_t  newStepperPos=map(newVal, 0, 65535,0, aircoreData[datenfeld[pos].target].arc);
    if (debugmode)
    {
      if (millis()>lastMessageAC+1000)
      {
        DebugReadback(pos);
        SERIALCOM.print(VAR_BEGIN);
        SERIALCOM.print('t');
        SERIALCOM.print(newStepperPos) ;
        SERIALCOM.println(VAR_ENDE);
        lastMessageAC=millis();
      }
    }
    
    aircoreMotor[datenfeld[pos].target].setPosition(newStepperPos);
    aircoreData[datenfeld[pos].target].last=newVal;
    lastUpdateAC=millis();
  }
  if (millis()-lastUpdateAC<5000)  //sleep if no new data since 5 seconds
   {aircoreMotor[datenfeld[pos].target].update();}
}
