// settings and functions to display numbers on 7-segment-displays

#include <TM1637Display.h>

// Call 7-Segment-Display with TM1637 controller
#define TM1637_CLK A5
#define TM1637_DIO A4

TM1637Display TM1637_display=TM1637Display(TM1637_CLK, TM1637_DIO);
//uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
//uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };

  
void SetupTM1637()
{
  
  TM1637_display.setBrightness(0x0f);
  TM1637_display.clear();
}

//display a 4-digit number from the flight data on the display
void UpdateTM1637(byte p)
{
  uint16_t number = atoi(datenfeld[p].wert);
  uint8_t dots;
  if (datenfeld[p].dp==0) dots=0b10000000;
  if (datenfeld[p].dp==1) dots=0b01000000;
  if (datenfeld[p].dp==2) dots=0b00100000;
  if (datenfeld[p].dp==3) dots=0b00010000;
  TM1637_display.showNumberDecEx(number, dots,true);

}
