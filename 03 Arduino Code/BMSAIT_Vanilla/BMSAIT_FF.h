//FuelFlow code from the DEDunino software
// V1.3.7 26.09.2021

#include <U8g2lib.h>
#include "FalconFFIFont.h"   //load font

// Declare screen Object
// make sure to find the correct constructor here
#if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA)
  //arduino board with enough memory will use the unbuffered mode
  U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI DisplayFFI(U8G2_R0, 2/*clock (D0) */, 3/*data (D1) */, 6/*cs*/,5/*dc*/, 4/*reset*/); 
#else
  //arduino board with low memory will have to use the buffered mode
  U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI DisplayFFI(U8G2_R0, 2/*clock (D0) */, 3/*data (D1) */, 6/*cs*/,5/*dc*/, 4/*reset*/);
#endif

// Change these display sizes if needed
#define FFI_SCREEN_W 128
#define FFI_SCREEN_H 64

// FONT DEFINITIONS - Main fuel flow indicator digits
#define ffFont FalconFFI // Main font used for actual digits indicating current fuel flow
#define FF_CHAR_W 20  // - Define Character width 
#define FF_CHAR_H 30  // - Define Character height

#define FF_H_CONST 0 // - Define Character horizontal offset. By design this text is perfectly centered on the display. Offset only when needed.
#define FF_V_CONST 0 // - Define Character vertical offset. By design this text is perfectly centered on the display. Offset only when needed.


#if defined(MEGA) || defined(DUE) || defined(DUE_NATIVE)
  // disable the "expensive stuff" without actually asking the users
  #define REALFFI
  #define BEZEL
#endif

#ifdef BEZEL
  // FONT DEFINITIONS - "FUEL FLOW"-text in bezel
  // - Define Character width and height, pertains to this particular font, best not change.
  #define BEZEL_FF_CHAR_W 10
  #define BEZEL_FF_CHAR_H 9
  // - Define Character horizontal and vertical offset, based on trial and error, not math
  #define BEZEL_FF_H_CONST 14 // in this case this is not used as an offset, but as a left and right indent for both "FUEL" and "FLOW"!
  #define BEZEL_FF_V_CONST 0
  
  // FONT DEFINITIONS - "PPH"-text in bezel
  // - Define Character width and height
  #define BEZEL_PPH_CHAR_W 6
  #define BEZEL_PPH_CHAR_H 11
  // - Define Character horizontal and vertical offset. Since PPH is perfectly centered, no need to adjust here.
  #define BEZEL_PPH_H_CONST 1
  #define BEZEL_PPH_V_CONST 5
#endif



// NO NEED TO CHANGE ANYTHING BELOW THIS LINE !!
// Global Variable required
char FuelFlow[5];

// Some x,y positions are fixed throughout the running of the main loop. They never change.
// So, why not just calculate them once, store them into variables, and re-use these?
// It saves that tiny Arduino processor a whole lot of processing power, so instead of calculating
// coordinates over and over again, it can focus on just drawing.
// Things we need from here on:
// - middle of screen, horizontal and vertical
// - "FUEL"-position
// - "FLOW"-position
// - "PPH"-position
// - "PPH"-wiping-box-position and size
// - the 3 positions for the FF-digits
//

// Center of the used screen, based on screen size constants defined earlier
const unsigned short FFI_SCREEN_W_MID = FFI_SCREEN_W / 2;
const unsigned short FFI_SCREEN_H_MID = FFI_SCREEN_H / 2;

// Fuel flow number, X position for digits
const unsigned short FF_POS_X_1 = int(FFI_SCREEN_W_MID - ((FF_CHAR_W * 7) / 2)) + ((FF_CHAR_W + 1) * 1) + FF_H_CONST;
const unsigned short FF_POS_X_2 = int(FFI_SCREEN_W_MID - ((FF_CHAR_W * 7) / 2)) + ((FF_CHAR_W + 1) * 2) + FF_H_CONST;
const unsigned short FF_POS_X_3 = int(FFI_SCREEN_W_MID - ((FF_CHAR_W * 7) / 2)) + ((FF_CHAR_W + 1) * 3) + FF_H_CONST;
const unsigned short FF_POS_X_4 = int(FFI_SCREEN_W_MID - ((FF_CHAR_W * 7) / 2)) + ((FF_CHAR_W + 1) * 4) + FF_H_CONST;
const unsigned short FF_POS_X_5 = int(FFI_SCREEN_W_MID - ((FF_CHAR_W * 7) / 2)) + ((FF_CHAR_W + 1) * 5) + FF_H_CONST;
// Fuel flow number, Y position (base)
const unsigned short FF_POS_Y = FFI_SCREEN_H_MID + FF_V_CONST;

#ifdef BEZEL
  // FUEL-position X and Y not needed, these are constants already. No need to spend more memory on putting them in new variables
  
  // FLOW-position X. Y-position already is a constant, so no need to re-calculate
  const unsigned short FLOW_POS_X = FFI_SCREEN_W - BEZEL_FF_H_CONST/2 - uint8_t(BEZEL_FF_CHAR_W * 4);
  
  // PPH-position X and Y
  const unsigned short PPH_POS_X = uint8_t(FFI_SCREEN_W_MID - ((BEZEL_PPH_CHAR_W * 3) / 2)) + BEZEL_PPH_H_CONST + 1;
  const unsigned short PPH_POS_Y = FFI_SCREEN_H - BEZEL_PPH_V_CONST - BEZEL_PPH_CHAR_H + 1;
  
  // X,Y, Width and height of the PPH wiper box
  //const unsigned short WIPE_BOX_X = FFI_SCREEN_W_MID - uint8_t((BEZEL_PPH_CHAR_W * 3) / 2);
  const unsigned short WIPE_BOX_X = 0;
  const unsigned short WIPE_BOX_Y = PPH_POS_Y + BEZEL_PPH_V_CONST - 1;
  //const unsigned short WIPE_BOX_W = uint8_t((BEZEL_PPH_CHAR_W + 1) * 3);
  const unsigned short WIPE_BOX_W = FFI_SCREEN_W;
  //const unsigned short WIPE_BOX_H = BEZEL_PPH_CHAR_H + 2;
  const unsigned short WIPE_BOX_H = FFI_SCREEN_H - WIPE_BOX_Y;
  #ifdef REALFFI
    #define FF_WIPE_X 0
    #define FF_WIPE_TOP_Y 0
    const unsigned short FF_WIPE_BOTTOM_Y =  FF_POS_Y + (FF_CHAR_H / 2);
    const unsigned short FF_WIPE_BOX_W = (FF_POS_X_3 - 2);
    const unsigned short FF_WIPE_BOX_H = (FF_POS_Y - (FF_CHAR_H / 2));
  #endif
#endif


////////////////////
//// Functions ////
///////////////////
#ifdef BEZEL
  // Function drawBezel
  // My intention is to create re-usable code. Why code something twice (having to change it twice if in error), when one can fix it with a function?
  void drawBezel() {
    // Wipe the PPH block
    DisplayFFI.setColorIndex(0);
    DisplayFFI.drawBox(WIPE_BOX_X, WIPE_BOX_Y, WIPE_BOX_W, WIPE_BOX_H ); // Clear the area below the "PPH" so the hundreds digits dont show there
  #ifdef REALFFI
    // Wipe the higher digits roll
    DisplayFFI.drawBox(FF_WIPE_X, FF_WIPE_TOP_Y, FF_WIPE_BOX_W , FF_WIPE_BOX_H); //top box
    DisplayFFI.drawBox(FF_WIPE_X, FF_WIPE_BOTTOM_Y, FF_WIPE_BOX_W , FF_WIPE_BOX_H); //Bottom box
  #endif
    DisplayFFI.setColorIndex(1);
    // Draw the lettering
    DisplayFFI.drawStr(BEZEL_FF_H_CONST, BEZEL_FF_V_CONST, "FUEL");
    DisplayFFI.drawStr(FLOW_POS_X,  BEZEL_FF_V_CONST, "FLOW");
    DisplayFFI.drawStr(PPH_POS_X, PPH_POS_Y , "PPH");
  }
#endif

void ClearDisplayFFI()
{
  DisplayFFI.firstPage();
  do {  } while ( DisplayFFI.nextPage() );
}

void SetupFFI() 
{
  DisplayFFI.begin();
  //  ffDisp.disableCursor(); //disable cursor, enable cursore use: enableCursor();
  DisplayFFI.setFont(ffFont);
  DisplayFFI.setFontPosCenter();
  
  /// Begin Picture loop ///
  DisplayFFI.firstPage();
  do 
  {
    #ifdef BEZEL
      drawBezel(); // Moved all this bezel stuff into a function to prevent us from needing to change everything twice
    #endif

    // I've centered this in the same fashion as the "PPH"
    DisplayFFI.drawStr(FF_POS_X_1, FF_POS_Y, "00000");
      
  } while ( DisplayFFI.nextPage() );
  /// End Picture loop ///
  lastInput=millis();  // make sure the initial screen shows for 10 seconds
}


void UpdateFFI(byte x) 
{
  if ((millis()-lastInput)>10000) //if no data was recieved within 10 seconds, shut down display
  {
    if (!debugmode) //display remains on in debugmode
    {
      ClearDisplayFFI();
      delay(1);
      return;
    }
  }

  if (datenfeld[0].wert[0]=='T')
  {
    for (byte c=0;c<4;c++)
     {FuelFlow[c]=datenfeld[x].wert[c];} //display FF value only if player is in 3D
    FuelFlow[4]='0';
  }
  else
  {
    DisplayFFI.firstPage();
    do 
    {
      DisplayFFI.setCursor(FF_POS_X_1, FFI_SCREEN_H_MID + FF_V_CONST);
      DisplayFFI.print("00000");
      #ifdef BEZEL
          drawBezel();
      #endif
    } while ( DisplayFFI.nextPage() );
    return;
  }
    
  // get the actual fuelflow from the flightdata
  char FFtt = FuelFlow[0];
  char FFt = FuelFlow[1];
  char FFh = FuelFlow[2];

  // Now for the animation:
  //Find the previous digit.
  char FFhPrev;
  if (FFh == 48)  
  {
    FFhPrev = 57; //if FFh=0, the prev value is 9
  } 
  else 
  {
    FFhPrev = FFh - 1; //otherwise subtract one from FFh
  }
  
  //Find the next digit.
  char FFhNext;
  if (FFh == 57) 
  { 
    FFhNext = 48; //if it's a 9 then next is 0
  } 
  else 
  {
    FFhNext = FFh + 1; //otherwise add one to FFh
  }
  
  //Find the next but one digit.
  char FFhTwoOver;
  if (FFhNext == 57) 
  {
    FFhTwoOver = 48; //if FFhNext=9, the next but one value is 0
  } 
  else 
  {
    FFhTwoOver = FFhNext + 1; //otherwise add one to FFhNext
  }


#ifdef REALFFI
  byte RollOverFlags = 0; // to save memory, we will be using a single byte with bit flags
  #define FFtRollOver (RollOverFlags & 0x01) // FFt is about to roll over
  #define FFttRollOver (RollOverFlags & 0x02) // FFtt is about to roll over

  // FFt
  char FFtNext;
  char FFtPrev;

  if (FFt == 48) 
  {
    FFtPrev = 57; //if FFt=0, the prev value is 9
  } 
  else 
  {
    FFtPrev = FFt - 1; //otherwise subtract one from FFt
  }

  if (FFt == 57)  
  { 
    FFtNext = 48;   //if it's a 9 then next is 0
  }  
  else 
  {
    FFtNext = FFt + 1; //otherwise add one to FFt
  }

  //FFtt
  char FFttNext;
  char FFttPrev;

  if (FFtt == 57) 
  { 
    FFttNext = 48;  //if it's a 9 then next is 0
  } 
  else 
  {
    FFttNext = FFtt + 1; //otherwise add one to FFtt
  }
  
  if (FFtt == 48) 
  {
    FFttPrev = 47; 
    FFtt = 47;
  } 
  else if (FFtt == 49) 
  {
    FFttPrev = 47;
  } 
  else 
  {
    FFttPrev = FFtt - 1;
  }

  if (FFh == 57)  //If FFh is ASCII 9 we are at roll over (up or down doesn't matter)
  { 
    RollOverFlags |= 0x01; //if FFh is 9 I'm going up
    // find above and below values
    if (FFt == 57)  // if FFt is 9 and rolling over the FFt is rolling over
    {
      RollOverFlags |= 0x02;
    }
  }
#endif

  // use tens digit to calculate the vertical offset for animation (tens and singles are always 0 on the gauge)
  short offset = short((FuelFlow[3] - '0') * FF_CHAR_H / 10); 

  
  /// Begin Picture loop ///
  DisplayFFI.firstPage();
  do 
  {
    #ifdef REALFFI
      // Draw FFtt
      if (FFttRollOver) { // if rolling over - Animate
        DisplayFFI.setCursor(FF_POS_X_1, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * -1)) + offset + FF_V_CONST);
        DisplayFFI.print(FFttNext);
        DisplayFFI.setCursor(FF_POS_X_1, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * 0)) + offset + FF_V_CONST);
        DisplayFFI.print(FFtt);
        DisplayFFI.setCursor(FF_POS_X_1, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * 1)) + offset + FF_V_CONST);
        DisplayFFI.print(FFttPrev);
      } else { // just print
        DisplayFFI.setCursor(FF_POS_X_1, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * -1)) + FF_V_CONST);
        DisplayFFI.print(FFttNext);
        DisplayFFI.setCursor(FF_POS_X_1, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * 0)) + FF_V_CONST);
        DisplayFFI.print(FFtt);
        DisplayFFI.setCursor(FF_POS_X_1, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * 1)) + FF_V_CONST);
        DisplayFFI.print(FFttPrev);
      }
      // Draw FFt
      if (FFtRollOver)  //FFtRollOver rollOver is true then something is about to change - draw both up and down
      {
        DisplayFFI.setCursor(FF_POS_X_2, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * -1)) + offset + FF_V_CONST);
        DisplayFFI.print(FFtNext);
        DisplayFFI.setCursor(FF_POS_X_2, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * 0)) + offset + FF_V_CONST);
        DisplayFFI.print(FFt);
        DisplayFFI.setCursor(FF_POS_X_2, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * 1)) + offset + FF_V_CONST);
        DisplayFFI.print(FFtPrev);
      } 
      else  //FFtRollOver is false - draw normal digits
      { 
        DisplayFFI.setCursor(FF_POS_X_2, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * -1)) + FF_V_CONST);
        DisplayFFI.print(FFtNext);
        DisplayFFI.setCursor(FF_POS_X_2, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * 0)) + FF_V_CONST);
        DisplayFFI.print(FFt);
        DisplayFFI.setCursor(FF_POS_X_2, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * 1)) + FF_V_CONST);
        DisplayFFI.print(FFtPrev);
      }
    #else
      // Draw normal FFtt and FFt
      DisplayFFI.setCursor(FF_POS_X_1, FFI_SCREEN_H_MID + FF_V_CONST);
      DisplayFFI.print(FFtt); // First two digits
      DisplayFFI.setCursor(FF_POS_X_2, FFI_SCREEN_H_MID + FF_V_CONST);
      DisplayFFI.print(FFt); // First two digits
    #endif

    // print the FFh animation
    DisplayFFI.setCursor(FF_POS_X_3, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * -2)) + offset + FF_V_CONST);
    DisplayFFI.print(FFhTwoOver);
    DisplayFFI.setCursor(FF_POS_X_3, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * -1)) + offset + FF_V_CONST);
    DisplayFFI.print(FFhNext);
    DisplayFFI.setCursor(FF_POS_X_3, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * 0)) + offset + FF_V_CONST);
    DisplayFFI.print(FFh);
    DisplayFFI.setCursor(FF_POS_X_3, FFI_SCREEN_H_MID + short(((FF_CHAR_H + 1) * 1)) + offset + FF_V_CONST);
    DisplayFFI.print(FFhPrev);

    // Print the statics
    // thousand and tens of thousands
    // and two 0 for the end - two commants are for even spacing (at a cost of performance
    DisplayFFI.drawStr(FF_POS_X_4, FFI_SCREEN_H_MID + FF_V_CONST, "0"); // Last two digits
    DisplayFFI.drawStr(FF_POS_X_5, FFI_SCREEN_H_MID + FF_V_CONST, "0"); // Last two digits

    #ifdef BEZEL
        drawBezel();
    #endif
  } while ( DisplayFFI.nextPage() );
    /// End Picture loop ///

}
