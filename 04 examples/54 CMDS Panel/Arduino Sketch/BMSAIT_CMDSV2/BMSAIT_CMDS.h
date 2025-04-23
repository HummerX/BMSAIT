//modifications for CMDS Panel
//V1.1 16.10.2023

byte mainPwr = 6;               //memorizes the line in the data container that contains the a/c power status
byte ChaffID = 0;               //memorizes the line in the data container that contains the chaff count
byte FlareID = 1;               //memorizes the line in the data container that contains the flare count
byte ChLoID = 7;               //memorizes the line in the data container that contains the chaff LOW warning
byte FlLoID = 8;               //memorizes the line in the data container that contains the flare LOW warning

byte ChaffON = false;            //memorizes the position of the chaff switch
byte FlareON = false;            //memorizes the position of the flare switch
bool CMDSMain = false;           //stores the status of the CMDS MAIN switch (OFF-false; ON - all other positions)
char Wert[DATENLAENGE] = "";     //buffer for output on 7-Segment-Display

void SetupCMDS()
{
  //search for the variables in the data containers that will control the display of chaff/flare qty and status
  bool gefunden[5]={false,false,false,false,false};
  
  for (int lauf=0;lauf<variableCount;lauf++)
  {
    
    if (strcmp(datenfeld[lauf].ID, "1243")==0)  //memorize the position of the MainPower variable
    {
      mainPwr = lauf;
      gefunden[0]=true;
    }
    if (strcmp(datenfeld[lauf].ID, "0150")==0)  //memorize the position of the chaff quantity variable
    {
      ChaffID = lauf;
      gefunden[1]=true;
    }                      
    if (strcmp(datenfeld[lauf].ID, "0160")==0)  //memorize the position of the flare quantity variable
    {
      FlareID = lauf;
      gefunden[2]=true;
    }  
    if (strcmp(datenfeld[lauf].ID, "1551")==0)  //memorize the position of the chaff low warning  variable
    {
      ChLoID = lauf;
      gefunden[3]=true;
    }  
    if (strcmp(datenfeld[lauf].ID, "1552")==0)  //memorize the position of the flare low warning variable
    {
      FlLoID = lauf;
      gefunden[4]=true;
    }              
  }
 if (!gefunden[0] || !gefunden[1] || !gefunden[2]|| !gefunden[3]|| !gefunden[4]) SendMessage("Fehler in Funktion SetupCMDS",1);
}


///switches will be checked for internal commands. Internal commands will affect the 7-segment-displays 
void CheckSwitchesCMDS()
{
  CMDSMain=false;
  
  for(byte sw=0;sw<numSwitches;sw++)
  {  
    if (switches[sw].intCommand==1)  //check MAIN switch
    {
      if ((switches[sw].typ==1)||(switches[sw].typ==2))  // digital reading
      {
        if(switches[sw].lastPINState==0) //switch is active (ON)
           {CMDSMain=true;}        
      }
      else                    // analog reading
      {
        if (switches[sw].lastPINState>153)  //rotary is out of leftmost position (OFF)
          {CMDSMain=true;}        
      }
    }
    
    if (switches[sw].intCommand==2)  //check flare switch
    {
      if(switches[sw].lastPINState==0) //switch is active (ON)
        {FlareON=true;}
      else
        {FlareON=false;} 
    }  
    
    if (switches[sw].intCommand==3)  //check chaff switch
    {
      if(switches[sw].lastPINState==0) //switch is active (ON)
        {ChaffON=true;}
      else
        {ChaffON=false;} 
    } 
  } 
}

///check if a display is supposed to be illuminated
bool checkPowerOn(byte disp)
{
  if (datenfeld[mainPwr].wert[0]=='F'){return false;} //turn off displays if a/c power is off
  
  if (!CMDSMain){return false;}                       //turn off both displays if CMDS Power is off

  if ((disp==FlareID)&&(FlareON==false)){return false;}  //turn off flare qty if flare switch is off
  if ((disp==FlLoID)&&(FlareON==false)){return false;}   //turn off flare Lo warning if flare switch is off
  if ((disp==ChaffID)&&(ChaffON==false)){return false;}  //turn off chaff qty if chaff switch is off
  if ((disp==ChLoID)&&(ChaffON==false)){return false;}   //turn off chaff Lo warning if chaff switch is off  

  return true;
}


///adds functions to the 7-Segement update to simulate the CMDS more accurately
void CMDSUpdate(byte p)
{
  CheckSwitchesCMDS(); //check switch positions to determine display behaviour
  
  bool power=checkPowerOn(p);
  if (!power)  // if no power is applied the display remains blank
  {
    for (int lauf=0;lauf<DATENLAENGE-1;lauf++)
      {Wert[lauf]=' ';}
    Wert[DATENLAENGE-1]='\0';
    return; 
  }
  if (p==FlLoID)
  {
    if (datenfeld[FlLoID].wert[0]=='T')  //Flare LOW warning is on
    {
      Wert[0]='L';
      Wert[1]='o';
    }
    else
    {
      Wert[0]=' ';
      Wert[1]=' ';
    }
  }
  if (p==ChLoID)
  {
    if (datenfeld[ChLoID].wert[0]=='T') //Chaff LOW warning is on
    {
      Wert[0]='L';
      Wert[1]='o';
    }
    else
    {
      Wert[0]=' ';
      Wert[1]=' ';
    }
  }
  if ((p==ChaffID)&&(datenfeld[ChaffID].wert[0]=='-')) //if a/c is out of chaff, the sharedmem value switches to -1, but display should be 0
    {
      Wert[0]=' ';
      Wert[1]='0';
    }
  if ((p==ChaffID)&&(datenfeld[ChaffID].wert[1]=='.')) //remove decimal point if chaff is below 10
    {
      Wert[1]=Wert[0];
      Wert[0]=' ';
    }
  if ((p==FlareID)&&(datenfeld[FlareID].wert[0]=='-')) //if a/c is out of flares, the sharedmem value switches to -1, but display should be 0
    {
      Wert[0]=' ';
      Wert[1]='0';
    }
  if ((p==FlareID)&&(datenfeld[FlareID].wert[1]=='.')) //remove decimal point if flare is below 10
    {
      Wert[1]=Wert[0];
      Wert[0]=' ';
    }
}
