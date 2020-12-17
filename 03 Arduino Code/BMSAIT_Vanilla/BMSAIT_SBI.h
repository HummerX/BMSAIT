#include <U8g2lib.h>

//U8G2 constructor
U8G2_SSD1306_128X64_NONAME_2_4W_SW_SPI SBI_display(U8G2_R1, 2/*clock*/, 3/*data*/, 6/*cs*/,5/*dc*/, 4/*reset*/);

#define JITTER_ON

#define SBIFONT u8g2_font_crox4hb_tf
#define CLOSED_SBIFONT_H 14
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


#define SBIDELAY 250
byte power=99;
short oldPos=0;
bool SBopen=false;
bool OfforTransit=false;
  
void Closed()
{
  SBI_display.drawStr(CLOSED_OFFSETX, ((SBI_display.getDisplayHeight()-CLOSED_SBIFONT_H)/2)+CLOSED_OFFSETY, "CLOSE");
}

void Off(byte jitter_offset)
{
  bool color=true;
  for (byte x=1;x<OFF_STRIPES;x++)
  {
    if (color) 
      {SBI_display.drawLine(OFF_OFFSETX+OFF_STRIPES-x,jitter_offset+OFF_OFFSETY,OFF_OFFSETX,jitter_offset+OFF_OFFSETY+OFF_STRIPES-x);}
    else
      {SBI_display.drawLine(OFF_OFFSETX+OFF_STRIPES,jitter_offset+OFF_OFFSETY+x,OFF_OFFSETX+x,jitter_offset+OFF_OFFSETY+OFF_STRIPES); }
      
    if (x % OFF_THICKNESS==0){color=!color;}
  }
}

void Open(byte jitter_offset)
{
  for(byte x=1;x<4;x++)
  {
    for (byte y=1;y<4;y++)  
    {
      SBI_display.drawFilledEllipse(OPEN_OFFSETX+OPEN_DISTANCE*x,jitter_offset+OPEN_OFFSETY+OPEN_DISTANCE*y,OPEN_DIAMETER,OPEN_DIAMETER,U8G2_DRAW_ALL);
    }
  }
}

void SBIPrep()
{
  SBI_display.setFont(SBIFONT);
  SBI_display.setFontRefHeightExtendedText();
  SBI_display.setDrawColor(1);
  SBI_display.setFontPosTop();
  SBI_display.setFontDirection(0);
}
  
void SetupSBI() 
{
  SBI_display.begin();
  SBIPrep();
  for (int lauf=0;lauf<VARIABLENANZAHL;lauf++)
  {
    if (strcmp(datenfeld[lauf].ID, "1242")==0)  //memorize the position of the MainPower variable
    {power = lauf;}   
  }
}


//animated off-flag jitter
void jitter()
{
  const byte d=25; //jitter speed
  for (byte jinks=0;jinks<4;jinks++)
  {
    SBI_display.firstPage();
    do
    {
      Off(10);
    } while( SBI_display.nextPage() );
    delay(d);
    SBI_display.firstPage();
    do
    {
      Off(0);
    } while( SBI_display.nextPage() );
    delay(d);
    SBI_display.firstPage();
    do
    {
      Off(-10);
    } while( SBI_display.nextPage() );
    delay(d);
    SBI_display.firstPage();
    do
    {
      Off(0);
    } while( SBI_display.nextPage() );
  }
}


void UpdateSBI(byte pos) 
{
  short SBPos=(short)(100*atof(datenfeld[pos].wert));
  bool snap=false;
  
  #ifdef JITTER_ON   //displays stutters when transitioning from open to closed and vice versa
    if (SBPos!=oldPos)
      {
        if ((oldPos<=1)&&(SBPos>1)) 
          {
           jitter();
           Open(0);
           oldPos=SBPos;
           return;
          }
        if ((oldPos>1)&&(SBPos<=1))
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
    if (datenfeld[power].wert[0]=='F')
      {OfforTransit=true;}
    else
      {OfforTransit=false;}
    
    if (SBPos>1)
      {SBopen=true;}
    else
      {SBopen=false;}

    SBI_display.firstPage();  
    {
      do 
      {  
        if (OfforTransit) 
          Off(0);
        else if (SBopen) 
          Open(0);
        else 
          Closed();
      } while( SBI_display.nextPage() );
    }

}
