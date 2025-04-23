// Version: 1.0    6.1.24
// Display of the wet compass on a 128x32 OLED display

#include <U8g2lib.h>
#include "BMSAIT_OLED_Compass_img.h"

/// Declare screen Object
    // --> make sure to find the correct constructor for your hardware <--
    //  https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#constructor-name
    
  #if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA) || defined(ESP)
    //arduino board with enough memory will use the unbuffered mode
    U8G2_SH1106_128X32_VISIONOX_F_HW_I2C displayOLED(U8G2_R2, /* reset=*/ U8X8_PIN_NONE); 
  #else
    //arduino board with low memory will have to use the buffered mode
    U8G2_SH1106_128X32_VISIONOX_2_HW_I2C displayOLED(U8G2_R2, /* reset=*/ U8X8_PIN_NONE); 
  #endif


///Layout settings
                            //                        Rotation: U8G2_R0  U8G2_R1  U8G2_R2  U8G2_R3
                            //                                  (0°)    (90°CW)   (180°)   (270°CW) 
  #define OFFSETX 335       //increase this to move graphic     left     down      right      up
                            //decrease this to move graphic     right     up       left      down  
  #define OFFSETY 0         //increase this to move graphic      up      right     down      left
                            //decrease this to move graphic     down     left       up       right 
  

///functions


void ClearDisplayOLEDCompass()
{
#if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA) || defined(ESP)
  displayOLED.clearBuffer();
  displayOLED.sendBuffer();
#else
    displayOLED.firstPage();
    do {} 
    while ( displayOLED.nextPage() );
#endif
}
  
void SetupOLEDCompass() 
{
  displayOLED.begin();
   
  #if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA) || defined(ESP)
    displayOLED.clearBuffer();
    displayOLED.setFont(u8g2_font_logisoso32_tf);
    displayOLED.drawStr(5, 32, "BMSAIT");
    displayOLED.sendBuffer();
  #else
    displayOLED.firstPage();
    do 
    {
      displayOLED.setFont(u8g2_font_logisoso32_tf);
      displayOLED.drawStr(5, 32, "BMSAIT");   
    } while ( displayOLED.nextPage() );
  #endif
 
  delay(2000);
}

void UpdateOLEDCompass(byte pos) 
{
  if ((millis()-lastInput)>10000) 
  {
    if (!debugmode)         //if not in testmode and no data was recieved within 10 seconds,the display will shut down
    {
      ClearDisplayOLEDCompass();    
      return;
    }
  }  
  unsigned short CH=atoi(datenfeld[pos].wert);
  if (CH>359) {CH=0;}
  short xpos=OFFSETX+3*CH-compass_width;
#if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA) || defined(ESP)
  displayOLED.clearBuffer();
  displayOLED.drawXBMP(xpos, OFFSETY, compass_width, compass_height, compass_img);   
  displayOLED.sendBuffer();
#else
  displayOLED.firstPage();
  do 
  {
    displayOLED.drawXBMP(xpos, OFFSETY, compass_width, compass_height, compass_img);   
  } while ( displayOLED.nextPage() );
#endif
}
