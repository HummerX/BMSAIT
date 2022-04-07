// settings and functions to drive simple LED
// V1.3.7 26.09.2021

// datenfeld.target defines the PIN of a single LED
// datenfeld.ref2 sets LED brightness (0..255) -->works on pwm PINs only!
// datenfeld.ref2 sets a timemark for fast blink (2 switches per second)
// datenfeld.ref3 sets a timemark for slow blink (1 switch per second)
#ifdef ESP  
  #include <analogWrite.h>
#endif

#define BLINKSPEED 500  //pause (in ms) between on/off for fast blinking. Slow blinking will be 50%

unsigned long LEDTimer;

void SetupLED()
{
   //all LED PINs are set to output
  for(byte a = 0; a < VARIABLENANZAHL; a++)
  {
    if (datenfeld[a].typ==10)
    {
      pinMode(datenfeld[a].target, OUTPUT);
      digitalWrite(datenfeld[a].target,LOW);
    }
    if (datenfeld[a].typ==11)
    {
      pinMode(datenfeld[a].target, OUTPUT);
      digitalWrite(datenfeld[a].target,HIGH);
    } 
  }
  LEDTimer=millis();;
}

void UpdateBlink()
{
  LEDTimer=millis();
  for(byte a = 0; a < VARIABLENANZAHL; a++)
  {
    if ((datenfeld[a].typ==10) || (datenfeld[a].typ==11))
    {
      if (datenfeld[a].ref3==1)
      {
        datenfeld[a].ref3=0;
        if (datenfeld[a].ref4==0)
          {datenfeld[a].ref4=1;}
        else
          {datenfeld[a].ref4=0;}
      }
      else
      {
        datenfeld[a].ref3=1;
      }
    }
  }
}


void LED_On(byte pIN, byte brightness, boolean currDir)
{
  if (brightness>0) //set brightness (only works on pwm PINs)
  {
    if (currDir)
      analogWrite(pIN,brightness);
    else
      analogWrite(pIN,255-brightness); 
  }
  else
  {  
    digitalWrite(pIN,currDir); 
  }
}

void LED_Off(byte pIN, boolean currDir)
{
  digitalWrite(pIN,!currDir); 
}

void UpdateLED(byte p, boolean currDir)
{
  // if the first character is T(rue) or 1 (on, no blink), the LED will be turned on
  if ((datenfeld[p].wert[0]=='T') || (datenfeld[p].wert[0]=='1'))        
  { 
    LED_On(datenfeld[p].target,datenfeld[p].ref2,currDir); 
  }
  else if (datenfeld[p].wert[0]=='3') //fast blinking LED
  {
    if (datenfeld[p].ref3==1)
    { LED_On(datenfeld[p].target,datenfeld[p].ref2,currDir); }
    else
    { LED_Off(datenfeld[p].target,currDir); }
  }
  else if (datenfeld[p].wert[0]=='2') //slow blinking LED
  {
    if (datenfeld[p].ref4==1)
    { LED_On(datenfeld[p].target,datenfeld[p].ref2,currDir); }
    else
    { LED_Off(datenfeld[p].target,currDir);}
  }
  else         // otherwise the LED will be turned off                         
  {
    LED_Off(datenfeld[p].target,currDir);  
  }
  
  if (millis()>LEDTimer+BLINKSPEED)
  { UpdateBlink(); }
}
