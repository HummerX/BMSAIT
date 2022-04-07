// settings and functions to enable a software controlled backlighting
// V0.1 30.03.2022

// datenfeld.target defines the PIN to output power to a 5V backlighting circuit (max 40mA!)
bool lightsOn=false;

void SetupLighting()
{
  for (byte x;x<VARIABLENANZAHL;x++)
  {
    if (datenfeld[x].typ==80)
      {pinMode(datenfeld[x].target, OUTPUT);}
  }
}

void UpdateLighting(byte pos)
{
  uint16_t brightness=atoi(datenfeld[pos].wert);
  if (brightness==0)
  {
    digitalWrite(datenfeld[pos].target,0); 
    if (!lightsOn)
    {
      if (debugmode)SendMessage("Licht an",1);
      lightsOn=true;
    }  
  }
  else
  {
    digitalWrite(datenfeld[pos].target,1);
    if (lightsOn)
    {
      if (debugmode)SendMessage("Licht aus",1);
      lightsOn=false;
    }  
  }
}
