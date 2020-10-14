// settings and functions to display data on a LCD screen (16x2 / 20x4)

#include <LiquidCrystal_I2C.h>                      
#define LCD_CHARS  16           // max chars per line of your display
#define LCD_LINES  2            // max rows of your display
#define LCD_PAUSE  200          // pause between updates in milliseconds
LiquidCrystal_I2C lcd(0x27, LCD_CHARS, LCD_LINES);   

void SetupLCD()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print(ID);  //display the ID of this arduino
  delay(2000);
  lcd.clear();
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
  
  //make sure that the size of the data variable does not exceed the display length of the LCD if you don't want to run into weired issues.
  
  //mod
  CMDSUpdate(d);
  if (CMDSMain)
    {lcd.display();}
  else
    {lcd.noDisplay();}
  //mod
   
    if (datenfeld[d].ziel<LCD_LINES)  //only write data if row is valid
    {
      byte tab=0;
      byte cu= charused(datenfeld[d].wert);
      if (datenfeld[d].stellen> cu){ tab= datenfeld[d].stellen-cu;}  //rightbound display

      //clear fields for value
      lcd.setCursor(datenfeld[d].start, datenfeld[d].ziel);  
      for (byte lauf=0;lauf<datenfeld[d].stellen;lauf++)
        {lcd.print(" ");}

      //display the variable value  
      lcd.setCursor(datenfeld[d].start+tab, datenfeld[d].ziel);  
      char ergebnis[datenfeld[d].stellen]="";
      //memcpy(ergebnis, datenfeld[d].wert, datenfeld[d].stellen); 
      //mod
      memcpy(ergebnis, Wert, datenfeld[d].stellen); 
      //mod
      ergebnis[datenfeld[d].stellen]='\0';
      lcd.print(ergebnis);     
    }
}
