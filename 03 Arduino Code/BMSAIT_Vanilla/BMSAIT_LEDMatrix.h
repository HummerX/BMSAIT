//V1.2   18.12.2022
// settings and functions to display numbers on 7-segment-displays
// datenfeld.target defines the Max7219 device (if you daisy-chained multiple devices)
// datenfeld.ref2 defines the row of a LED
// datenfeld.ref3 defines the column of a LED
// datenfeld.ref4 fast blinking is on
// datenfeld.ref5 slow blinking is on

#include <LedControl.h>
const byte Anz_Max7219_Controller =1;
unsigned long LEDMatrixTimer;

LedControl LEDController=LedControl(0,0,0, 1); //empty call of the LEDControl class

#define LEDM_CLK 4   //PIN "Clock" for the SPI connection of the MAX7219 LED Control
#define LEDM_CS  3    //PIN "Cable Select" for the SPI connection of the MAX7219 LED Control
#define LEDM_DIN 2  //PIN "Data In" for the SPI connection of the MAX7219 LED Control
#define LEDM_BRIGHTNESS 8  //sets the intensity of the LEDs (0..15)
#define BLINKSPEED 500  //pause (in ms) between on/off for fast blinking. Slow blinking will be 50%
  
  
void SetupLEDMatrix()
{
  delay(1000);
  LEDController=LedControl(LEDM_DIN, LEDM_CLK, LEDM_CS, Anz_Max7219_Controller);  //actual call of the LEDControl class with the correct PINs 
  
  for (byte l=0;l<Anz_Max7219_Controller;l++)
  {
    LEDController.shutdown(l,false);
    LEDController.setIntensity(l,LEDM_BRIGHTNESS);
    LEDController.clearDisplay(l);
  }
  for (byte r=0;r<8;r++)
  {
    for (byte c=0;c<8;c++)
    {
      LEDController.setLed(0,c,r,true);
    }
  }
  delay(1000);
  for (byte r=0;r<8;r++)
  {
    for (byte c=0;c<8;c++)
    {
      LEDController.setLed(0,c,r,false);
    }
  }
  delay(500);
}



void LEDMatrix_UpdateBlink()
{
  LEDMatrixTimer=millis();
  for(byte v = 0; v < variableCount; v++)
  {
    if (datenfeld[v].typ==12)
    {
      if (datenfeld[v].ref4==1)
      {
        datenfeld[v].ref4=0;
        if (datenfeld[v].ref5==0)
          {datenfeld[v].ref5=1;}
        else
          {datenfeld[v].ref5=0;}
      }
      else
        {datenfeld[v].ref4=1;}
    }
  }
}


void UpdateLEDMatrix(byte p)
{
  if ((datenfeld[p].wert[0]=='T')||(datenfeld[p].wert[0]=='1'))        // if the first character is T(rue) or 1,  the LED will be turned on
  {          
    LEDController.setLed(datenfeld[p].target,datenfeld[p].ref3,datenfeld[p].ref2,true);
  }
  else if (datenfeld[p].wert[0]=='3')                                  //fast blinking Light
  {
     if (datenfeld[p].ref4==1)
       {LEDController.setLed(datenfeld[p].target,datenfeld[p].ref3,datenfeld[p].ref2,true);}
     else
       {LEDController.setLed(datenfeld[p].target,datenfeld[p].ref3,datenfeld[p].ref2,false);}
  }
  else if (datenfeld[p].wert[0]=='2')                                  //slow blinking Light
  {
     if (datenfeld[p].ref5==1)
       {LEDController.setLed(datenfeld[p].target,datenfeld[p].ref3,datenfeld[p].ref2,true);}
     else
       {LEDController.setLed(datenfeld[p].target,datenfeld[p].ref3,datenfeld[p].ref2,false);}
  }
  else                                  // otherwise the LED will be turned off
  {
    LEDController.setLed(datenfeld[p].target,datenfeld[p].ref3,datenfeld[p].ref2,false);
  }
  
  if (millis()>LEDMatrixTimer+BLINKSPEED)
  { LEDMatrix_UpdateBlink(); }
}
