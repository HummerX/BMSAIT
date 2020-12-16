// settings and functions to display numbers on 7-segment-displays

#include <TM1637Display.h>

// Call 7-Segment-Display with TM1637 controller
#define TM1637_CLK A5
#define TM1637_DIO A4

TM1637Display TM1637_display=TM1637Display(TM1637_CLK, TM1637_DIO);


  
void SetupTM1637()
{
  TM1637_display=TM1637Display(TM1637_CLK, TM1637_DIO);
  TM1637_display.setBrightness(0x0f);
  TM1637_display.showNumberDecEx(69,0,true);
  delay(2000);
  TM1637_display.clear();
}

/*
//display a 4-digit number from the flight data on the display
void UpdateTM1637(int p)
{
  uint16_t number = atoi(datenfeld[p].wert);
  uint8_t dots;
  if (datenfeld[p].dp==0) dots=0b10000000;
  if (datenfeld[p].dp==1) dots=0b01000000;
  if (datenfeld[p].dp==2) dots=0b00100000;
  if (datenfeld[p].dp==3) dots=0b00010000;
  TM1637_display.showNumberDecEx(number, dots,true);
}
*/
//display a 4-digit number from the flight data on the display
void UpdateTM1637(int p)
{
  byte anzeige[4] = {0x00,0x00,0x00,0x00};
    memcpy(Wert, datenfeld[p].wert, sizeof(datenfeld[p].wert));
    
    //modifier for BUPRadio
    BUPRadioUpdate(p); //look for modifiers
        
    for (int lauf=0;lauf<datenfeld[p].ref3;lauf++)
    {
    if (Wert[lauf]!=' ') anzeige[lauf+datenfeld[p].ref4]=TM1637_display.encodeDigit(Wert[lauf]);
    }
    //modifier for BUPRadio
  
  uint8_t dots;
  if (datenfeld[p].ref5==0) dots=0b10000000;
  if (datenfeld[p].ref5==1) dots=0b01000000;
  if (datenfeld[p].ref5==2) dots=0b00100000;
  if (datenfeld[p].ref5==3) dots=0b00010000;
  TM1637_display.setSegments(anzeige);
  
}
