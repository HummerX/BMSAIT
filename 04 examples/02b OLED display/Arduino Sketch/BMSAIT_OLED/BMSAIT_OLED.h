#include <U8g2lib.h>

//constructor of U8G2 Lib


// Declare screen Object
// make sure to find the correct constructor here
#if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA) || defined(ESP)
  //arduino board with enough memory will use the unbuffered mode
  U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C displayOLED(U8G2_R0,2,3);
#else
  //arduino board with low memory will have to use the buffered mode
  U8G2_SSD1306_128X32_UNIVISION_2_SW_I2C displayOLED(U8G2_R0,2,3);
#endif

// set the number of chars to display(4..8).
#define DIGITS 5       


//the number of digits will influence the font size
#if DIGITS < 6
  #define OLEDFONT u8g2_font_inb24_mr
  #define OLEDFONT_H 24
  #define OLEDFONT_W 20
#elif DIGITS < 8
  #define OLEDFONT u8g2_font_inb21_mr 
  #define OLEDFONT_H 21
  #define OLEDFONT_W 16
#else
  #define OLEDFONT u8g2_font_inb19_mr
  #define OLEDFONT_H 18
  #define OLEDFONT_W 14
#endif


//Layout settings
                                        //Rotation: U8G2_R0  U8G2_R1  U8G2_R2  U8G2_R3
                                        //           (0°)    (90°CW)  (180°)   (270°CW) 
#define OFFSETX 10  //increase this to move Text      right     down     left      up
                   //decrease this to move Text      left      up       right    down  
#define OFFSETY 0  //increase this to move Text      down     right     up       left
                   //decrease this to move Text       up      left      down     right 



// Global Variable required
char Oled[DIGITS+1];

void OLEDPrep()
{
  displayOLED.setFont(OLEDFONT);
  displayOLED.setFontRefHeightExtendedText();
  displayOLED.setDrawColor(1);
  displayOLED.setFontPosTop();
  displayOLED.setFontDirection(0);
}

void ClearDisplayOLED()
{
  displayOLED.firstPage();
  do {  } while ( displayOLED.nextPage() );
}
  
void SetupOLED() 
{
  displayOLED.begin();
  OLEDPrep();
  ClearDisplayOLED();
    displayOLED.firstPage();  
  do 
  {  
    displayOLED.drawStr(0, ((displayOLED.getDisplayHeight()-OLEDFONT_H)/2)+OFFSETY, "BMSAIT");
  } while( displayOLED.nextPage() );
  delay(2000);
  
}

void UpdateOLED(byte pos) 
{
  if ((millis()-lastInput)>10000) //if no data was recieved within 10 seconds, shut down display
  {
    if (!debugmode) //display remains on in debugmode
    {
      ClearDisplayOLED();
      delay(1);
      return;
    }
  }
  
  if (datenfeld[0].wert[0]!='T') //display value only if player is in 3D
  {
   for (byte c=0;c<DIGITS;c++)
     {Oled[c]='0';}    
  }
  else
  {
    for (byte c=0;c<DIGITS;c++)
     {Oled[c]=datenfeld[pos].wert[c];} 
  }


  displayOLED.firstPage();  
  do 
  {  
    displayOLED.drawStr(OFFSETX, ((displayOLED.getDisplayHeight()-OLEDFONT_H)/2)+OFFSETY, Oled);
  } while( displayOLED.nextPage() );

}
