// settings and functions to drive simple LED
// datenfeld.target defines the PIN of a single LED

void SetupLED()
{
   //all LED PINs are set to output
   for(byte a = 0; a < VARIABLENANZAHL; a++)
     {
     if ((datenfeld[a].typ==10) || (datenfeld[a].typ==11))
      {
      pinMode(datenfeld[a].target, OUTPUT);
      }
     }
}

//PIN is connect to GND 
void UpdateLED_PINHIGH(byte p)
{
  if ((datenfeld[p].wert[0]=='T'))        // if the first character is T(rue), the LED will be turned on
  {
    digitalWrite(datenfeld[p].target,HIGH);
  }
  else                                  // otherwise the LED will be turned off
  {
    digitalWrite(datenfeld[p].target,LOW);
  }
}

//PIN is connect to Vcc
void UpdateLED_PINLOW(byte p)
{
  if ((datenfeld[p].wert[0]=='T'))        // if the first character is T(rue), the LED will be turned on
  {
    digitalWrite(datenfeld[p].target,LOW);
  }
  else                                  // otherwise the LED will be turned off
  {
    digitalWrite(datenfeld[p].target,HIGH);
  }
}
