#include <U8g2lib.h>

//settings
//#define JITTER
#define SBIDELAY 250
#define SBIFONT u8g2_font_crox4hb_tf
#define CLOSED_SBIFONT_H 14

// Declare screen Object
// make sure to find the correct constructor here
#if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA)
  //arduino board with enough memory will use the unbuffered mode
  U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI displaySBI(U8G2_R1, 2/*clock (D0) */, 3/*data (D1) */, 6/*cs*/,5/*dc*/, 4/*reset*/);

#else
  //arduino board with low memory will have to use the buffered mode
  U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI displaySBI(U8G2_R1, 2/*clock (D0) */, 3/*data (D1) */, 6/*cs*/,5/*dc*/, 4/*reset*/);

#endif


//Layout settings
                                                        //Rotation: U8G2_R0  U8G2_R1  U8G2_R2  U8G2_R3
                                                        //           (0°)    (90°CW)  (180°)   (270°CW) 
#define CLOSED_OFFSETX 0  //increase this to move CLOSED graphic     right     down     left      up
                          //decrease this to move CLOSED graphic     left      up       right    down  
#define CLOSED_OFFSETY -2 //increase this to move CLOSED graphic     down     right     up       left
                          //decrease this to move CLOSED graphic      up      left      down     right 

#define OFF_OFFSETX 3     //increase this to move OFF graphic        right    down      left      up
                          //decrease this to move OFF graphic        left      up       right    down
#define OFF_OFFSETY 23    //increase this to move OFF graphic        down     right     up       left
                          //decrease this to move OFF graphic         up      left      down     right    
#define OFF_STRIPES 64    //increase OFF graphic size
#define OFF_THICKNESS 10  //increase stripe thickness  

#define OPEN_OFFSETX -5   //increase this to move OPEN graphic       right     down      left       up
                          //decrease this to move OPEN graphic       left       up       right     down
#define OPEN_OFFSETY 23   //increase this to move OPEN graphic       down      right      up       left
                          //decrease this to move OPEN graphic        up       left      down      right  
#define OPEN_DIAMETER 7   // bubble size
#define OPEN_DISTANCE 20  // bubble distance




unsigned short oldPos=0;
bool SBopen=false;
bool OfforTransit=false;
long last=0;

  
void Closed()
{
  displaySBI.drawStr(CLOSED_OFFSETX, ((displaySBI.getDisplayHeight()-CLOSED_SBIFONT_H)/2)+CLOSED_OFFSETY, "CLOSE");
}

void Off(byte jitter_offset)
{
  bool color=true;
  for (byte x=1;x<OFF_STRIPES;x++)
  {
    if (color) 
      {displaySBI.drawLine(OFF_OFFSETX+OFF_STRIPES-x,jitter_offset+OFF_OFFSETY,OFF_OFFSETX,jitter_offset+OFF_OFFSETY+OFF_STRIPES-x);}
    else
      {displaySBI.drawLine(OFF_OFFSETX+OFF_STRIPES,jitter_offset+OFF_OFFSETY+x,OFF_OFFSETX+x,jitter_offset+OFF_OFFSETY+OFF_STRIPES); }
      
    if (x % OFF_THICKNESS==0){color=!color;}
  }
}

void Open(byte jitter_offset)
{
  for(byte x=1;x<4;x++)
  {
    for (byte y=1;y<4;y++)  
    {
      displaySBI.drawFilledEllipse(OPEN_OFFSETX+OPEN_DISTANCE*x,jitter_offset+OPEN_OFFSETY+OPEN_DISTANCE*y,OPEN_DIAMETER,OPEN_DIAMETER,U8G2_DRAW_ALL);
    }
  }
}

void ClearDisplaySBI()
{
  displaySBI.firstPage();
  do {  } while ( displaySBI.nextPage() );  
}


  
void SetupSBI() 
{
  displaySBI.begin();
  displaySBI.setFont(SBIFONT);
  displaySBI.setFontRefHeightExtendedText();
  displaySBI.setDrawColor(1);
  displaySBI.setFontPosTop();
  displaySBI.setFontDirection(0);
}


//animated off-flag jitter
void jitter()
{
  const byte d=25; //jitter speed
  for (byte jinks=0;jinks<4;jinks++)
  {
    displaySBI.firstPage();
    do
    {
      Off(10);
    } while( displaySBI.nextPage() );
    delay(d);
    displaySBI.firstPage();
    do
    {
      Off(0);
    } while( displaySBI.nextPage() );
    delay(d);
    displaySBI.firstPage();
    do
    {
      Off(-10);
    } while( displaySBI.nextPage() );
    delay(d);
    displaySBI.firstPage();
    do
    {
      Off(0);
    } while( displaySBI.nextPage() );
  }
}


void UpdateSBI(byte pos) 
{
  unsigned short SBPos=atoi(datenfeld[pos].wert);
  bool snap=false;
  #ifdef JITTER
  if (SBPos!=oldPos)
  {
    
    if ((oldPos<=600)&&(SBPos>600))  // 1% open
      {
       jitter();
       Open(0);
       oldPos=SBPos;
       return;
      }
    else if ((oldPos>=600)&&(SBPos<600)) // 1% closed
      {
       jitter();
       Closed();
       oldPos=SBPos;
       return;
      }
    else
    { oldPos=SBPos;} 
  }
  #endif

  if ((millis()-lastInput)>10000) 
  {
    if (!testmode)            //display remains on in testmode
    {
      ClearDisplaySBI();    //if no data was recieved within 10 seconds, display shuts down
      delay(1);
      return;
    }
  }
  else if ((datenfeld[0].wert[0]=='F')|| (datenfeld[1].wert[0]=='F'))  
    {OfforTransit=true;}    //player is either not in 3D or the a/c power is still off
  else
    {OfforTransit=false;}   //player is in 3D and a/c power is on
  
  if (SBPos>600) // 1% open
    {SBopen=true;}
  else
    {SBopen=false;}

  displaySBI.firstPage();  
  {
    do 
    {  
      if (OfforTransit) 
        Off(0);
      else if (SBopen) 
        Open(0);
      else 
        Closed();
    } while( displaySBI.nextPage() );
  }
}
