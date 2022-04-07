 // V2.0  12.3.22
 // Display of the DED or the PFL on a 256x64 OLED display
 // inspired by the DEDuino coding by Uri_ba (https://pit.uriba.org/tag/deduino/)

  #include <U8g2lib.h>
  #include "FalconDEDFont.h"   //load font

// Declare screen Object
#if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA) || defined(ESP)
  //arduino board with enough memory will use the unbuffered mode
   //U8G2_SSD1322_NHD_256X64_F_4W_SW_SPI displayDED(U8G2_R0, /* clock=*/ 7, /* data=*/ 6, /* cs=*/ 5, /* dc=*/ 3, /* reset=*/ 4);
   U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI displayDED(U8G2_R0, /* cs=*/ 2, /* dc=*/ 4, /* reset=*/ 0);
#else
  //arduino board with low memory will have to use the buffered mode  
   //U8G2_SSD1322_NHD_256X64_1_4W_SW_SPI displayDED(U8G2_R0, /* clock=*/ 7, /* data=*/ 6, /* cs=*/ 5, /* dc=*/ 3, /* reset=*/ 4);
   U8G2_SSD1322_NHD_256X64_1_4W_HW_SPI displayDED(U8G2_R0, /* cs=*/ 2, /* dc=*/ 4, /* reset=*/ 0);
#endif
  
// How many milliseconds go before the displays reset after initillizing (for user inspection)
  #define PRE_BOOT_PAUSE 1000
  #define POST_BOOT_PAUSE 1000
  #define DED_DRAW_PAUSE 200

unsigned long DED_last_comm = millis();
short Run = 0;
bool powerOn=false;
bool sw=false;

///draw empty display
void ClearDED()
{
  displayDED.firstPage();
  do 
  {
    for (byte line = 0; line < 5; line++ ) 
    {
      displayDED.drawStr(DED_H_CONST, line * DED_CHAR_H + DED_V_CONST, "                        ");
    }
  } while ( displayDED.nextPage() ); 
}

//send a request to the BMSAIT app to send DED/PFL data
void ReadDED()
{
  byte i1='\0';
  byte i2='\0';
  byte treffer=0;
  if (SERIALCOM.available()) ReadResponse();
  if (datenfeld[2].ID[0]=='0') {SendMessage("DED",6);} //data variable is set to 023x -> ask for PFL data
  if (datenfeld[2].ID[0]=='1') {SendMessage("PFL",6);} //data variable is set to 110x -> ask for PFL data
  long t=millis();
  treffer=0;
  while ((treffer<5) && (t+1000>millis()))
  {
    if (SERIALCOM.available()<2)
      {
        if (t+1000<millis()) break;
        delay(1);
      }
    else
    {
      i1= SERIALCOM.read();
      
      if (i1=='e') break; //BMSAIT reports that no PFL/DED data is available
      
      if (i1=='c') //BMSAIT reports that PFL/DED is blank
      {
        ClearDED();
        break; 
      }
      
      if (i1=='d') //BMSAIT sends PFL/DED data
      {
        i2= SERIALCOM.read(); //read PFL/DED line
        if ((i2>47)&&(i2<58))
        {
          if (SERIALCOM.available()<24)
            {
              if (t+1000<millis()) break;
              delay(1);
            }
          SERIALCOM.readBytes(datenfeld[i2-48+2].wert,24);
          datenfeld[i2-48+2].wert[24]='\0';
          treffer++;
        }
        else
        {
          SERIALCOM.println("e1");
          //error - expected DED/PFL line information not recieved
        }
      }
      else
      {
        SERIALCOM.println("e2");
        //error - expected end of line information not found
      }
    }
  }
  if (treffer==5) lastInput=millis();
}


///draw PFL/DED data from SharedMem
void RealDED()
{
    #if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA)
      displayDED.clearBuffer();     //powerful arduinos: command for full buffered mode
    #else
      displayDED.firstPage();       //weak arduinos: command for paged mode
      do {
    #endif
    
      for (byte line = 0; line < 5; line++) 
      {
        displayDED.drawStr(DED_H_CONST, line * DED_CHAR_H + DED_V_CONST, datenfeld[line+2].wert);
      }
    #if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA)
      displayDED.sendBuffer();           //powerful arduinos: command for full buffered mode
    #else
      } while ( displayDED.nextPage() ); //weak arduinos: command for paged mode
    #endif 
}

///draw a test picture 
void TestDED()
{
  #if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA)
    displayDED.clearBuffer();             //powerful arduinos: command for full buffered mode
  #else
    displayDED.firstPage();               //weak arduinos: command for paged mode
    do {
  #endif

      displayDED.drawFrame(1,1,255,63);
      displayDED.drawFrame(64,16,128,32);
      displayDED.drawStr(62+DED_H_CONST, DED_V_CONST+14+DED_CHAR_H , "BMSAIT - PFL");       
      displayDED.drawLine(128,1,128,16);
      displayDED.drawLine(128,48,128,63);
      displayDED.drawLine(1,32,63,32);
      displayDED.drawLine(192,32,255,32);
  
  #if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA)
    displayDED.sendBuffer();            //powerful arduinos: command for full buffered mode
  #else
    } while ( displayDED.nextPage() );  //weak arduinos: command for paged mode
  #endif
}

//setup the OLED
void SetupDED() 
{
  delay(PRE_BOOT_PAUSE); 
  
  displayDED.begin();
  displayDED.setFont(FalconDED);
  displayDED.setFontPosTop();
  TestDED();
  delay(POST_BOOT_PAUSE); 
  ClearDED();
  SERIALCOM.setTimeout(500);
}


///updates the output of the DED on the OLED
void DrawDED(bool mode) 
{
  if (mode)
    {RealDED();}
  else
    {TestDED();}
}


///update the DED
void UpdateDED() 
{ 
  if ((millis()-lastInput)>10000) //if no data was recieved within 10 seconds, shut down display
  {
    if (!debugmode) //in testmode, display remains on regardless of data transfer status
    {
      if (powerOn)
      {
        ClearDED();
        displayDED.setPowerSave(1); //show nothing
        powerOn=false; 
      }
    }
    return;
  }
  
  if (!powerOn)
  {
    displayDED.setPowerSave(0);
    powerOn=true;
  }
  
  if (datenfeld[0].wert[0]!='T')  //PC is not in 3D
  { DrawDED(false);}         //show testscreen
  else if (datenfeld[1].wert[0]!='T')  
  { ClearDED();}             //PC is in 3D, Avionics are off
  else                                 
  { DrawDED(true); }         //PC is in 3D, Avionics are on
}
