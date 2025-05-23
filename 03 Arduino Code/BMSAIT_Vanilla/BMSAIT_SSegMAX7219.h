// settings and functions to display numbers on 7-segment-displays
// V1.3.7 26.09.2021

//target= device ID (if multiple displays are connected
//ref2= not used
//ref3= number of figures to display
//ref4= offset position 
//ref5= decimal point

#include <LedControl.h>
LedControl Max7219_display[]={
    LedControl(0,0,0, 1)     //empty call of the LEDControl class
    //,LedControl(0,0,0, 1)  //uncomment this if you use a second Max7219 on one arduino. copy this line if you use more than 2.
};
const byte max7219anz = sizeof(Max7219_display)/sizeof(Max7219_display[0]); 

#define MAX_CLK 8   //PIN "Clock" for the SPI connection of the 7-Segment Tube 
#define MAX_CS 9    //PIN "Cable Select" for the SPI connection of the 7-Segment Tube
#define MAX_DIN 10  //PIN "Data In" for the SPI connection of the 7-Segment Tube 
#define MAX_BRIGHTNESS 5 //0 (out) -  16 (max brightness)
  
void SetupMax7219()
{
  Max7219_display[0]=LedControl(MAX_DIN, MAX_CLK, MAX_CS, 1);  //actual call of the LEDControl class with the correct PINs 
  //Max7219_display[1]=LedControl(/*DIN*/,/*CLK*/, /*CS*/, 1);  //uncomment this if you use a second Max7219 on one arduino. copy this line if you use more than 2. Make sure to enter the correct PINs the additional devices are connected to
  for (byte x=0;x<max7219anz;x++)
  {
    Max7219_display[x].shutdown(0,false);
    Max7219_display[x].setIntensity(0,MAX_BRIGHTNESS);
    Max7219_display[x].setDigit(0,0,1,false);
    Max7219_display[x].setDigit(0,1,2,false);
    Max7219_display[x].setDigit(0,2,3,false);
    Max7219_display[x].setDigit(0,3,4,false);
    Max7219_display[x].setDigit(0,4,5,false);
    Max7219_display[x].setDigit(0,5,6,false);
    Max7219_display[x].setDigit(0,6,7,false);
    Max7219_display[x].setDigit(0,7,8,false);
  }
  delay(2000);
  for (byte x=0;x<max7219anz;x++)
    {Max7219_display[x].clearDisplay(0);}
}


void UpdateMAX7219(byte p)
{
  bool dp=false;
  char Wert[DATENLAENGE]="";

  memcpy(Wert, datenfeld[p].wert, DATENLAENGE);
 
  byte stelle = 7-datenfeld[p].ref4;
  for (byte x=0 ; x<datenfeld[p].ref3 ; x++)
  {
    if (dp){dp=false;}
    if ((x==datenfeld[p].ref5-1)&&(Wert[x]!=' ')){dp=true;}    //set decimal point 
    Max7219_display[datenfeld[p].target].setChar(0,stelle,Wert[x],dp);
    stelle--;
  }          
}
