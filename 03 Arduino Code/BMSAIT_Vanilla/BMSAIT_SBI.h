

//target= reference link to the line of the motorpoti table of this module
//ref2= not used
//ref3= not used
//ref4= not used
//ref5= not used

#include <U8g2lib.h>
#define SBIDELAY 250
#define THICKNESS 10

//my settings
  #define SBIFONT u8g2_font_crox4hb_tf
  #define SBIFONT_H 14
  #define BORDER  32
  #define DIAMETER 7
  #define DISTANCE 20
  
  #define SBI_PIN_ROTATION U8G2_R1
  #define SBI_PIN_CLOCK 2
  #define SBI_PIN_DATA 3
  #define SBI_PIN_CS 6
  #define SBI_PIN_DC 5
  #define SBI_PIN_RESET 4
  U8G2_SSD1306_128X64_NONAME_2_4W_SW_SPI u8g2(SBI_PIN_ROTATION, SBI_PIN_CLOCK, SBI_PIN_DATA, SBI_PIN_CS, SBI_PIN_DC, SBI_PIN_RESET);

//Giovannis settings
  //#define SBIFONT u8g2_font_fub11_tf
  //#define SBIFONT_H 11
  //#define BORDER  32
  //#define DIAMETER 6
  //#define DISTANCE 18
  
  //#define SBI_PIN_ROTATION U8G2_R3
  //#define SBI_PIN_CLOCK 4
  //#define SBI_PIN_DATA 9
  //U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(SBI_PIN_ROTATION, SBI_PIN_CLOCK, SBI_PIN_DATA);


short oldPos=0;
unsigned long transitOn=0;
bool SBopen=false;
bool OfforTransit=false;
  
void Closed()
{
  u8g2.drawStr(0, (u8g2.getDisplayHeight()-SBIFONT_H)/2, "CLOSE");
}

void Off()
{
  bool color=true;
  for (byte x=1;x<65;x++)
  {
    if (color) 
      {u8g2.drawLine(64-x,BORDER,0,BORDER+64-x);}
    else
      {u8g2.drawLine(64,BORDER+x,x,BORDER+64); }
      
    if (x % THICKNESS==0){color=!color;}
  }
}

void Open()
{
  for(byte x=1;x<4;x++)
  {
    for (byte y=1;y<4;y++)  
    {
      u8g2.drawFilledEllipse(DISTANCE*x-8,BORDER-8+DISTANCE*y,DIAMETER,DIAMETER,U8G2_DRAW_ALL);
    }
  }
}

void SBIPrep()
{
  u8g2.setFont(SBIFONT);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}
  
void SetupSBI() 
{
  // put your setup code here, to run once:
  u8g2.begin();
  SBIPrep();
}


void UpdateSBI(byte pos) 
{
  short SBPos=(short)(100*atof(datenfeld[pos].wert));
  if (SBPos!=oldPos)
    {
      transitOn=millis()+SBIDELAY;
      oldPos=SBPos;
    }
   
    if (datenfeld[1].wert[0]=='0')
      {OfforTransit=true;}
    //else if (transitOn>millis())
    //  {OfforTransit=true;}
    else
      {OfforTransit=false;}
    
    if (SBPos>1)
      {SBopen=true;}
    else
      {SBopen=false;}

  u8g2.firstPage();  
  do 
  {  
    if (OfforTransit) 
      Off();
    else if (SBopen) 
      Open();
    else 
      Closed();
    
  } while( u8g2.nextPage() );
  
}
