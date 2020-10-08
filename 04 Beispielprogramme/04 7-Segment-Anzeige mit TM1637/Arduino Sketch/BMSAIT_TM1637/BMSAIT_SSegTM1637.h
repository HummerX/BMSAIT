// settings and functions to display numbers on 7-segment-displays

#include <TM1637Display.h>

// Call 7-Segment-Display with TM1637 controller
#define TM1637_CLK 2
#define TM1637_DIO 3

//empty call
TM1637Display TM1637_display=TM1637Display(0, 0);

  
void SetupTM1637()
{
  //initialize display
  TM1637Display TM1637_display=TM1637Display(TM1637_CLK, TM1637_DIO);

  //set brightness
  TM1637_display.setBrightness(0x0f);

  //clear display
  TM1637_display.clear();
}

///display a number on the TM1637 display
void UpdateTM1637(int p)
{
    //read first n digits from the stored value and save this into a buffer
    byte buf[datenfeld[p].stellen+1];
    for (byte x;x<datenfeld[p].stellen;x++) 
    {buf[x]=datenfeld[p].wert[x];}
    buf[datenfeld[p].stellen]='\0';
    //convert the buffer to a decimal number
    uint16_t seg= atoi(buf);

    //display the number
    TM1637_display.showNumberDecEx(seg, 0b01000000,true,datenfeld[p].stellen,datenfeld[p].start);

  /*
  uint16_t number = atoi(datenfeld[p].wert);
  uint8_t dots;
  if (datenfeld[p].dp==0) dots=0b10000000;
  if (datenfeld[p].dp==1) dots=0b01000000;
  if (datenfeld[p].dp==2) dots=0b00100000;
  if (datenfeld[p].dp==3) dots=0b00010000;
  TM1637_display.showNumberDecEx(number, dots,true);
  */
}
