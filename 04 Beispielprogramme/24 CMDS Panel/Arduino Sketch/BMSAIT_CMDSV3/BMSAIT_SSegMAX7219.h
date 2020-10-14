// settings and functions to display numbers on 7-segment-displays

#include <LedControl.h>
LedControl Max7219_display=LedControl(0,0,0, 1); //empty call of the LEDControl class

#define MAX_CLK 2   //PIN "Clock" for the SPI connection of the 7-Segment Tube 
#define MAX_CS  3   //PIN "Cable Select" for the SPI connection of the 7-Segment Tube
#define MAX_DIN 4   //PIN "Data In" for the SPI connection of the 7-Segment Tube 

  
void SetupMax7219()
{
  Max7219_display=LedControl(MAX_DIN, MAX_CLK, MAX_CS, 1);  //actual call of the LEDControl class with the correct PINs 
  Max7219_display.shutdown(0,false);
  Max7219_display.setIntensity(0,5);
  Max7219_display.setDigit(0,0,1,false);
  Max7219_display.setDigit(0,1,2,false);
  Max7219_display.setDigit(0,2,3,false);
  Max7219_display.setDigit(0,3,4,false);
  Max7219_display.setDigit(0,4,5,false);
  Max7219_display.setDigit(0,5,6,false);
  Max7219_display.setDigit(0,6,7,false);
  Max7219_display.setDigit(0,7,8,false);
  delay(2000);
  Max7219_display.clearDisplay(0);
}

void UpdateMAX7219(int p)
{
  bool dp=false;
  memcpy(Wert, datenfeld[p].wert, DATENLAENGE);
  
  //modifier for CMDS
    CMDSUpdate(p); //look for modifiers
  //modifier for CMDS
  
  byte stelle = 7-datenfeld[p].start;
  for (byte x=0 ; x<datenfeld[p].stellen ; x++)
  {
    if ((x==datenfeld[p].dp-1)&&(Wert[x]!=' ')) {dp=true;}    //Dezimalpunkt setzen
    Max7219_display.setChar(0,stelle,Wert[x],dp);
    dp=false;
    stelle--;
  }       
}
