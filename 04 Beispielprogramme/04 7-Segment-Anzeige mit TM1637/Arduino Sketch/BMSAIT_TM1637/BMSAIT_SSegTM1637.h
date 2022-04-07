
// This LEDDisplayDriver.h library is used to drive a TM1637 module with 6 digits
// Manual for library: http://lygte-info.dk/project/DisplayDriver%20UK.html
// By HKJ from lygte-info.dk

// settings and functions to display numbers on 7-segment-displays
//target= device ID (if multiple displays are connected
//ref2= not used
//ref3= number of figures to display
//ref4= offset position 
//ref5= decimal point

#include <LEDDisplayDriver.h>

#ifndef _TM1637_
#error "_TM1637_ must be defined in LEDDisplayDriver.h for this sketch to work"
#endif

// Call 7-Segment-Display with TM1637 controller
#define TM1637_DIGITS 4 //number of digits on your TM1637 display (4 or 6)
#define TM1637_SDA_PIN A4 //DIO
#define TM1637_SCL_PIN A5 //CLK
#define TM1637_BRIGHTNESS 10



#if TM1637_DIGITS==6
  LEDDisplayDriver TM1637_display[] = 
  {
      LEDDisplayDriver(TM1637_SDA_PIN, TM1637_SCL_PIN,true, TM1637_DIGITS)
      ,LEDDisplayDriver(A1/*SDA_PIN*/,A2/* SCL_PIN*/,true, 6/*num of digits*/)
  };
#else
  LEDDisplayDriver TM1637_display[] = 
  {
      LEDDisplayDriver(TM1637_SDA_PIN, TM1637_SCL_PIN)
      ,LEDDisplayDriver(A1/*SDA_PIN*/,A2/* SCL_PIN*/)
  };
#endif


void SetupTM1637()
{
  TM1637_display[0].setBrightness(TM1637_BRIGHTNESS);
  TM1637_display[0].clear();
  TM1637_display[1].setBrightness(TM1637_BRIGHTNESS);
  TM1637_display[1].clear();
}


void UpdateTM1637(int p)
{
  #if TM1637_DIGITS==6
    byte disp[6] ={0,0,0,0,0,0};
  #else
    byte disp[4] ={0,0,0,0};
  #endif
  memcpy(disp,datenfeld[p].wert,datenfeld[p].ref3);
  
  byte pos=0;
  //write digits
  for (byte x=0;x<datenfeld[p].ref3;x++)
  {
    #if TM1637_DIGITS == 6
      //correct digit order
      pos=TM1637_TransformPos(5-x-datenfeld[p].ref4);
    #else
      //no correction required
      pos=x;
    #endif
        
    TM1637_display[datenfeld[p].target].showChar(pos,disp[x]);
  }
  

 //paint decimal point
  if(datenfeld[p].ref5<=datenfeld[p].ref3)  
  {
    if (disp[datenfeld[p].ref5]==' ')
      { TM1637_display[datenfeld[p].target].removeDp();}
    else
    {  
      #if TM1637_DIGITS == 6
        //correct digit order
        TM1637_display[datenfeld[p].target].setDp(TM1637_TransformPos(5-datenfeld[p].ref5-datenfeld[p].ref4+1));
      #else
        //no correction required
        TM1637_display[datenfeld[p].target].setDp(5-datenfeld[p].ref5-datenfeld[p].ref4+1);
      #endif
    }
  }
  
}
