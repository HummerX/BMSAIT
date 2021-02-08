// settings and functions to display numbers on 7-segment-displays
// datenfeld.target defines the Max7219 device
// datenfeld.ref2 defines the row of a LED
// datenfeld.ref3 defines the column of a LED

#include <LedControl.h>
LedControl LEDM[2]={LedControl(0,0,0, 1),LedControl(0,0,0, 1)}; //empty call of the LEDControl class

#define LEDM_CLK 4   //PIN "Clock" for the SPI connection of the MAX7219 LED Control
#define LEDM_CS 3    //PIN "Cable Select" for the SPI connection of the MAX7219 LED Control
#define LEDM_DIN 2  //PIN "Data In" for the SPI connection of the MAX7219 LED Control
#define LEDM_BRIGHTNESS 5  //sets the intensity of the LEDs (0..15)
  
  
void SetupLEDMatrix()
{
  delay(1000);
  LEDM[0]=LedControl(LEDM_DIN, LEDM_CLK, LEDM_CS, 1);  //actual call of the LEDControl class with the correct PINs 
  LEDM[0].shutdown(0,false);
  LEDM[0].setIntensity(0,LEDM_BRIGHTNESS);
  LEDM[0].clearDisplay(0);
  
  //LEDM[1]=LedControl(0/*DIN*/,0 /*CLK*/,0 /*LEDM_CS*/, 1);  //actual call of second LEDControl class with the correct PINs 
  //LEDM[1].shutdown(0,false);
  //LEDM[1].setIntensity(0,LEDM_BRIGHTNESS);
  //LEDM[1].clearDisplay(0); 
   
  delay(1000);
}

void UpdateLEDMatrix(byte p)
{
  if ((datenfeld[p].wert[0]=='T'))        // if the first character is T(rue), the LED will be turned on
  {          
    LEDM[datenfeld[p].target].setLed(0,datenfeld[p].ref2,datenfeld[p].ref3,true);
  }
  else                                  // otherwise the LED will be turned off
  {
    LEDM[datenfeld[p].target].setLed(0,datenfeld[p].ref2,datenfeld[p].ref3,false);
  }
}
