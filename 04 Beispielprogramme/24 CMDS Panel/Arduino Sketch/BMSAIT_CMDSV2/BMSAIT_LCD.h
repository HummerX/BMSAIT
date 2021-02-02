// settings and functions to display data on a LCD screen (16x2 / 20x4)
// datenfeld.target defines the LCD device
// datenfeld.ref2 defines the line of the LCD display
// datenfeld.ref3 defines the number of characters of the variable to be displayed
// datenfeld.ref4 defines where in the line the variable is to be written (offset)
// datenfeld.ref5 defines the position of the decimal point

#include <LiquidCrystal_I2C.h>                      
#define LCD_CHARS  16           // max chars per line of your display
#define LCD_LINES  2            // max rows of your display
#define LCD_PAUSE  200          // pause between updates in milliseconds

LiquidCrystal_I2C lcd[0]= LiquidCrystal_I2C(0x27, LCD_CHARS, LCD_LINES);  
//LiquidCrystal_I2C lcd[1](/*I2C adress*/, /*CHARS*/, /*LCD_LINES*/);  

void SetupLCD()
{
  lcd[0].init();
  lcd[0].backlight();
  lcd[0].clear();
  lcd[0].print(ID);  //display the ID of this arduino
  
  //lcd[1].init();
  //lcd[1].backlight();
  //lcd[1].clear();
  //lcd[1].print(ID);  //display the ID of this arduino
  
  delay(2000);
  
  lcd[0].clear();
  //lcd[1].clear();
}

int charused(const char *data) {
    int c = 0;
    const char *p = data;
    while (*p) {
        c++;
        p++;
    }
    return c;
}

void Update_LCD(byte d)
{
  //mod
  CMDSUpdate(d);
  if (CMDSMain)
    {lcd[0].display();}
  else
    {lcd[0].noDisplay();}
  //mod
  
   //make sure that the size of the data variable does not exceed the display length of the LCD if you don't want to run into weired issues.
   
    if (datenfeld[d].ref2<LCD_LINES)  //only write data if row is valid
    {
      byte tab=0;
      byte cu= charused(datenfeld[d].wert);
      if (datenfeld[d].ref2> cu){ tab= datenfeld[d].ref2-cu;}  //rightbound display

      //clear fields for value
      lcd[datenfeld[d].target].setCursor(datenfeld[d].ref3, datenfeld[d].target);  
      for (byte lauf=0;lauf<datenfeld[d].ref2;lauf++)
        {lcd[datenfeld[d].target].print(' ');}

      //display the variable value  
      lcd[datenfeld[d].target].setCursor(datenfeld[d].ref3+tab, datenfeld[d].target);  
      char ergebnis[datenfeld[d].ref2]="";
      //memcpy(ergebnis, datenfeld[d].wert, datenfeld[d].ref2); 
      //mod
      memcpy(ergebnis, Wert, datenfeld[d].ref2); 
      //mod
      ergebnis[datenfeld[d].ref2]='\0';
      lcd[datenfeld[d].target].print(ergebnis);     
    }
}
