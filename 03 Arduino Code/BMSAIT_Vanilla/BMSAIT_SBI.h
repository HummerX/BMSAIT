 // Display of the SBI on a 128x64 OLED display
 // This code is an adaption of the DEDuino coding by Uri_ba (https://pit.uriba.org/tag/deduino/)
 // V1.3.7 26.09.2021
 
#include <U8g2lib.h>
#include "BMSAIT_SBI_img.h"


/// Declare screen Object
  // make sure to find the correct constructor here
  #if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA) || defined(ESP)
    //arduino board with enough memory will use the unbuffered mode
    U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI displaySBI(U8G2_R0, 2/*clock (D0) */, 3/*data (D1) */, 6/*cs*/,5/*dc*/, 4/*reset*/);
  #else
    //arduino board with low memory will have to use the buffered mode
    U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI displaySBI(U8G2_R0, 2/*clock (D0) */, 3/*data (D1) */, 6/*cs*/,5/*dc*/, 4/*reset*/);
  #endif

  #define SB_SCREEN_W 128 //set the width of your display
  #define SB_SCREEN_H 64  //set the height of your display


///Layout settings
                                                          //Rotation: U8G2_R0  U8G2_R1  U8G2_R2  U8G2_R3
                                                          //           (0°)    (90°CW)  (180°)   (270°CW) 
  #define OFFSETX 0         //increase this to move CLOSED graphic     right     down     left      up
                            //decrease this to move CLOSED graphic     left      up       right    down  
  #define OFFSETY 0         //increase this to move CLOSED graphic     down     right     up       left
                            //decrease this to move CLOSED graphic      up      left      down     right 
  
  #define SBIDELAY 500    //time (ms) the off-flag will show up when the SBI moves from closed to open or vice versa
  //#define ANIMATION      //if set, the display will show an animation of the scale movement; if not, the off flag will show during movement
  #define OFFSET_FRAMES 6  //speed of movement animation (increase to slow animation) 


///presets - don't change
  #define SB_IMG SBI_img
  #define SB_IMG_W 192
  #define SB_IMG_H 64
  #define SB_OFFSET_H SB_IMG_H/2
  #define SRC_OFFSET (-1*SB_IMG_H*sb[1])+SB_OFFSET_H
  #define DST_OFFSET (-1*SB_IMG_H*sb[0])+SB_OFFSET_H
  char sb[2]={3};
  unsigned short sBILast=0;
  long sBImovement=0;


///functions


void ClearDisplaySBI()
{
  displaySBI.firstPage();
  do {  } while ( displaySBI.nextPage() );  
}
  
void SetupSBI() 
{
  displaySBI.begin();
  /// Begin Picture loop ///
  displaySBI.firstPage();
  do {
    displaySBI.drawXBMP(OFFSETX-32, OFFSETY, SB_IMG_W, SB_IMG_H, SB_IMG); // Start with INOP
  } while ( displaySBI.nextPage() );
  /// End Picture loop ///
  delay(2000);
}

void UpdateSBI(byte pos) 
{
  if ((millis()-lastInput)>10000) 
  {
    if (!debugmode)            //display remains on in debugmode
    {
      ClearDisplaySBI();    //if no data was recieved within 10 seconds, display shuts down
      sb[0]=3;
      return;
    }
  }  
  unsigned short sBPos=atoi(datenfeld[pos].wert);
  if (sBPos!=sBILast)
  {
    sBImovement=millis();
     
  }
  
  sb[1] = sb[0];
  
  if ((datenfeld[0].wert[0]=='F') || (datenfeld[1].wert[0]=='F'))  
    {sb[0]=1;}      //player is either not in 3D or the a/c power is still off
  
  #ifndef ANIMATION   
  else if ( ((sBPos>600)&&(sBILast<600)) || ((sBPos<600)&&(sBILast>600)) ) //in non-animation mode, the off flag will show when the SBI is being moved from closed to open and vice versa
    {
      sb[0]=1;
      sBImovement=millis();
    }     
  #endif
  else if ( (sBPos>600) && (millis()-sBImovement>SBIDELAY) )    // SB more than 1% open
    {
      sb[0]=2;
    }      
  else if ( (sBPos<600) && (millis()-sBImovement>SBIDELAY) )    // SB less than 1% open
    {
      sb[0]=0;
    }      
  else
  {}
  
sBILast=sBPos; 

if ( sb[0] != sb[1] ) 
  { // if speedbreak value has changed (no need to re-render frame if it hadn't
    float offsetDelta = -1*((SRC_OFFSET) - (DST_OFFSET)) / OFFSET_FRAMES;

    #ifdef ANIMATION      //animated SBI movement
      for (short i = 0; i < OFFSET_FRAMES; i++) 
      {
        if (i == 3 || i ==4 ) 
        {
          break;
        }
        short frameOffset = (SRC_OFFSET) + (2*i * offsetDelta);
        displaySBI.firstPage();
        do 
        {
          displaySBI.drawXBMP(OFFSETX+frameOffset, OFFSETY, SB_IMG_W, SB_IMG_H, SB_IMG);
        }  while ( displaySBI.nextPage() );
      }
      displaySBI.firstPage();
      do {
        displaySBI.drawXBMP(OFFSETX+DST_OFFSET, OFFSETY, SB_IMG_W, SB_IMG_H, SB_IMG);
      } while ( displaySBI.nextPage() );
    #else           //non-animated SBI movement
      displaySBI.firstPage(); //show open/closed flag
      do {
        displaySBI.drawXBMP(OFFSETX+DST_OFFSET, OFFSETY, SB_IMG_W, SB_IMG_H, SB_IMG);
      } while ( displaySBI.nextPage() );
    #endif
  }
}
