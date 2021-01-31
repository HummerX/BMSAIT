// settings and functions to display numbers on 7-segment-displays
//target= device ID (if multiple displays are connected
//ref2= not used
//ref3= number of figures to display
//ref4= offset position 
//ref5= decimal point

#include <TM1637Display.h>

// Call 7-Segment-Display with TM1637 controller
#define TM1637_CLK 2
#define TM1637_DIO 3
#define TM1637_BRIGHTNESS 0x0f

TM1637Display TM1637_display[] = {
    TM1637Display(TM1637_CLK, TM1637_DIO)
    // ,TM1637Display(/*CLK PIN*/,/* DIO PIN */)
};


  
void SetupTM1637()
{
  TM1637_display[0].setBrightness(TM1637_BRIGHTNESS);
  TM1637_display[0].clear();
  //TM1637_display[1].setBrightness(TM1637_BRIGHTNESS);
  //TM1637_display[1].clear();
}

//display a 4-digit number from the flight data on the display
void UpdateTM1637(int p)
{
  uint16_t number = atoi(datenfeld[p].wert);
  uint8_t dots;
  if (datenfeld[p].ref5==0) dots=0b10000000;
  if (datenfeld[p].ref5==1) dots=0b01000000;
  if (datenfeld[p].ref5==2) dots=0b00100000;
  if (datenfeld[p].ref5==3) dots=0b00010000;
  TM1637_display[datenfeld[p].target].showNumberDecEx(number, dots,true);
}
