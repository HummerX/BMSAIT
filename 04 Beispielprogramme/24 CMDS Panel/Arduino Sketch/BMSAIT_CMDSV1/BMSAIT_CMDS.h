//modification for BUPRadio
byte mainPwr = 99;               //stores the status of the main power of the a/c
byte ChaffID = 99;               //memorizes the line in the data container that contains the chaff count
byte FlareID = 99;               //memorizes the line in the data container that contains the flare count
byte ChaffON = false;            //memorizes the position of the chaff switch
byte FlareON = false;            //memorizes the position of the flare switch
bool CMDSMain = false;           //stores the status of the CMDS MAIN switch (OFF-false; ON - all other positions)
char Wert[DATENLAENGE] = "";     //buffer for output on 7-Segment-Display

void SetupCMDS()
{
  //search for the variables in the data container that will control the display of channels/frequencies
  bool gefunden[3]={false,false,false};
  
  for (int lauf=0;lauf<VARIABLENANZAHL;lauf++)
  {
    if (strcmp(datenfeld[lauf].ID, "1260")==0)  //memorize the position of the MainPower variable
    {
      mainPwr = lauf;
      gefunden[0]=true;
    }
    if (strcmp(datenfeld[lauf].ID, "0150")==0)  //memorize the position of the preset channel variable
    {
      ChaffID = lauf;
      gefunden[1]=true;
    }                      
    if (strcmp(datenfeld[lauf].ID, "0160")==0)  //memorize the position of the BUP freq variable
    {
      FlareID = lauf;
      gefunden[2]=true;
    }          
  }
 if (!gefunden[0] || !gefunden[1] || !gefunden[2]) SendMessage("Fehler in Funktion SetupBUPRadio",1);
}


///switches will be checked for internal commands. Internal commands will affect the 7-Segment-Displays 
void CheckSwitchesCMDS()
{
  for(byte sw=0;sw<anzSchalter;sw++)
  {
    
    if (schalter[sw].intCommand==1)  //check MAIN switch
    {
      if ((schalter[sw].typ==1)&&(schalter[sw].typ==2))  // digital reading
      {
        if(schalter[sw].lastPINState==0) //switch is active (OFF)
          {CMDSMain=false;}
        else
          {CMDSMain=true;} 
      }
      else                    // analog reading
      {
        if (schalter[sw].lastPINState<(1024/STATES))  //rotary is in leftmost position (OFF)
          {CMDSMain=false;}
        else
          {CMDSMain=true;} 
      }
    }
    
    if (schalter[sw].intCommand==2)  //check flare switch
    {
      if(schalter[sw].lastPINState==0) //switch is active (ON)
        {FlareON=true;}
      else
        {FlareON=false;} 
    }  
    
    if (schalter[sw].intCommand==3)  //check chaff switch
    {
      if(schalter[sw].lastPINState==0) //switch is active (ON)
        {ChaffON=true;}
      else
        {ChaffON=false;} 
    } 
  }
}


///check if a display is supposed to be illuminated
bool checkPowerOn(byte disp)
{
  if (datenfeld[mainPwr].wert[0]=='0') return false; //turn off displays if a/c power is off
  
  if (!CMDSMain) return false;                       //turn off both displays if CMDS Power is off
  
  if ((disp==FlareID)&&(FlareON==false)){return false;}  //turn off flare display if flare switch is off
  if ((disp==ChaffID)&&(ChaffON==false)){return false;}  //turn off chaff display if chaff switch is off

  return true;
}


///adds functions to the 7-Segement update to simulate the CMDS more accurately
void CMDSUpdate(byte p)
{
  CheckSwitchesCMDS(); //check switch positions to determine display behaviour
  
  if (!checkPowerOn(p))  // no power is applied the display remains blank
    {
    for (int lauf=0;lauf<DATENLAENGE-1;lauf++)
    {Wert[lauf]=' ';}
    Wert[DATENLAENGE-1]='\0';
    return; 
  }

}
