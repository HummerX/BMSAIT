//options - uncommend options you want to use to determine the backup UHD display
#define check_for_ac_power
#define check_for_UHF_Main_Switch
//#define check_for_UHF_Volume_On

//function declarations
void debug_readbackBUP(byte posID);
void CheckSwitchesBUPRadio();
void PullRequestBUP();

//modification for BUPRadio
byte mainPwr = 99;               //memorizes the line in the data container that contains the electric status
byte presetID = 99;              //memorizes the line in the data container that contains the BUP radio channel
byte manualID = 99;              //memorizes the line in the data container that contains the BUP radio frequency
bool UHFMain = false;            //stores the status of the UHF main switch false-off, true-main/both
byte UHFMode = 1;                //stores the status of the UHF mode switch 0-man, 1-pre, 2-guard 
bool UHFStatusMode = false;      //is true if the Status button is being pressed
bool UHFTestMode = false;        //is true if the Test button is being pressed
bool UHFVolumeOff =false;        //is true if the UHF Volume is turned off
char Wert[DATENLAENGE] = "";     //buffer for output on 7-Segment-Display
const char Guard[7] = "243000";  //preset value for BUP Frequency if UHF mode is in GRD mode 
byte BUPpull = 0;                //memorizes the last BUP freq that got an update during last PULL cycle
char lastchannel[DATENLAENGE]="";  //memorize the previous radio channel
long channel_got_changed=0;      //memorize the last time the UHF channel switch got moved

Datenfeld BUPRadioFreq[21]=
  {
   {"LEER ","0000",'s',99,0,0,0,0,0,"000000"}
  ,{"UHFP1","1611",'s',99,0,0,0,0,0,"000000"}  //Variable  1 - BUP UHF Ch1 
  ,{"UHFP2","1612",'s',99,0,0,0,0,0,"000000"}  //Variable  2 - BUP UHF Ch2 
  ,{"UHFP3","1613",'s',99,0,0,0,0,0,"000000"}  //Variable  3 - BUP UHF Ch3 
  ,{"UHFP4","1614",'s',99,0,0,0,0,0,"000000"}  //Variable  4 - BUP UHF Ch4 
  ,{"UHFP5","1615",'s',99,0,0,0,0,0,"000000"}  //Variable  5 - BUP UHF Ch5 
  ,{"UHFP6","1616",'s',99,0,0,0,0,0,"000000"}  //Variable  6 - BUP UHF Ch6 
  ,{"UHFP7","1617",'s',99,0,0,0,0,0,"000000"}  //Variable  7 - BUP UHF Ch7 
  ,{"UHFP8","1618",'s',99,0,0,0,0,0,"000000"}  //Variable  8 - BUP UHF Ch8 
  ,{"UHFP9","1619",'s',99,0,0,0,0,0,"000000"}  //Variable  9 - BUP UHF Ch9 
  ,{"UHF10","1620",'s',99,0,0,0,0,0,"000000"}  //Variable 10 - BUP UHF Ch10 
  ,{"UHF11","1621",'s',99,0,0,0,0,0,"000000"}  //Variable 11 - BUP UHF Ch11 
  ,{"UHF12","1622",'s',99,0,0,0,0,0,"000000"}  //Variable 12 - BUP UHF Ch12 
  ,{"UHF13","1623",'s',99,0,0,0,0,0,"000000"}  //Variable 13 - BUP UHF Ch13 
  ,{"UHF14","1624",'s',99,0,0,0,0,0,"000000"}  //Variable 14 - BUP UHF Ch14 
  ,{"UHF15","1625",'s',99,0,0,0,0,0,"000000"}  //Variable 15 - BUP UHF Ch15 
  ,{"UHF16","1626",'s',99,0,0,0,0,0,"000000"}  //Variable 16 - BUP UHF Ch16 
  ,{"UHF17","1627",'s',99,0,0,0,0,0,"000000"}  //Variable 17 - BUP UHF Ch17 
  ,{"UHF18","1628",'s',99,0,0,0,0,0,"000000"}  //Variable 18 - BUP UHF Ch18 
  ,{"UHF19","1629",'s',99,0,0,0,0,0,"000000"}  //Variable 19 - BUP UHF Ch19 
  ,{"UHF20","1630",'s',99,0,0,0,0,0,"000000"}  //Variable 20 - BUP UHF Ch20  
  };  
    

void SetupBUPRadio()
{
  //search for the variables in the data container that will control the display of channels/frequencies
  bool gefunden[3]={false,false,false};
  
  for (int lauf=0;lauf<VARIABLENANZAHL;lauf++)
  {
    if (strcmp(datenfeld[lauf].ID, "1241")==0)  //memorize the position of the MainPower variable
    {
      mainPwr = lauf;
      gefunden[0]=true;
    }
    if (strcmp(datenfeld[lauf].ID, "1120")==0)  //memorize the position of the preset channel variable
    {
      manualID = lauf;
      gefunden[1]=true;
    }                      
    if (strcmp(datenfeld[lauf].ID, "1130")==0)  //memorize the position of the BUP freq variable
    {
     presetID = lauf;
      gefunden[2]=true;
    }          
  }
 if (!gefunden[0] || !gefunden[1] || !gefunden[2]) SendMessage("Fehler in Funktion SetupBUPRadio",1);
}

///check if a display is supposed to be illuminated
bool checkPowerOn(byte disp)
{
  #ifdef check_for_ac_power
    if (datenfeld[mainPwr].wert[0]!='T') return false; //turn off both displays if emergency power is off
  #endif
  
  #ifdef check_for_UHF_Main_Switch
    if (!UHFMain) return false;                       //turn off both displays if UHF Power is off
  #endif
  
  #ifdef check_for_UHF_Volume_On
    if (UHFVolumeOff) return false;                   //turn off both displays if UHF Volume is turned off
  #endif
  
  if ((datenfeld[presetID].wert[0]!= lastchannel[0]) || (datenfeld[presetID].wert[1]!= lastchannel[1])) //check if preset got changed
  {
    lastchannel[0]= datenfeld[presetID].wert[0];
    lastchannel[1]= datenfeld[presetID].wert[1];
    lastchannel[2]= datenfeld[presetID].wert[2];
    channel_got_changed=millis();
  }
  
  if (disp==presetID)  //PresetChannel display check
  {
    if (UHFMode==1) return true; //turn on channel display if Mode switch is in PRE 
    if (UHFStatusMode || UHFTestMode) return true; //turn on channel display if status or test button is being pressed
    if (channel_got_changed > millis()- 5000) return true; //turn on channel display if the channel selector switch recently got moved
    
    return false;  //otherwise, turn off display
  }
  return true;
}


//adds functions to the 7-Segement update to simulate the BUPRadio more accurately
void BUPRadioUpdate(byte p)
{
  PullRequestBUP();  //adds another request to update the BUPFreq container

  CheckSwitchesBUPRadio(); //check switch positions to determine display behaviour

  bool power=checkPowerOn(p);  //check if internal commands require a powerdown of the current display

  if (!power)  // no power is applied to the UHF panel. Display remains blank
    {
    for (int lauf=0;lauf<DATENLAENGE-1;lauf++)
    {Wert[lauf]=' ';}
    Wert[DATENLAENGE-1]='\0';
    return; 
  }

  if (UHFTestMode)  
  {
    //if testbutton is pressed, all displays show '8'  
    for (int lauf=0;lauf<DATENLAENGE-1;lauf++)
    {Wert[lauf]='8';}
    Wert[DATENLAENGE-1]='\0';
    return;
  }

  if ((p==manualID) && (UHFStatusMode==1)) 
  {
     //if statusbutton is pressed, the stored manual frequency for the preset will be shown
     //the presetID marks the variable that contains the current backup channel. this marks the line of the BUPRadio container that holds the manual backup freq
     memcpy(Wert, BUPRadioFreq[atoi(datenfeld[presetID].wert)].wert, DATENLAENGE); 
     return;
  }
  
  if ((p==manualID) && (UHFMode==2)) 
  {
    //display GRD Freq if in GRD mode
    memcpy(Wert, Guard, 7); 
    return;
  }
  
  //no execptions found, no modifications applied (display data as set in UserConfig)
}

//switches will be checked for internal commands. Internal commands will affect the 7-Segment-Displays 
void CheckSwitchesBUPRadio()
{
  //set default settings
  UHFMode=1;            //set a mark that UHF Mode switch is in PRE position (unless it is in PRE or GRD)  
  UHFStatusMode=false;  //disable status mode on bupradio (unless the button is being pressed)  
  UHFTestMode=false;    //disable test mode on bupradio (unless the button is being pressed) 
  UHFVolumeOff=true;   //assume the UHF volume is turned up
  
  for (byte index=0;index<anzSchalter;index++)
  {
    //check for commands
    if (schalter[index].lastPINState==0)  //switch is set / button is being pressed
    {
      if (schalter[index].intCommand==1) {UHFMain=false;}  //set a mark that UHF Main switch is in off position
      if (schalter[index].intCommand==2) {UHFMain=true;}   //set a mark that UHF Main switch is in main or both position
      if (schalter[index].intCommand==3) {UHFMode=0;}      //set a mark that UHF Mode switch is in MNL position  
      if (schalter[index].intCommand==5) {UHFMode=2;}      //set a mark that UHF Mode switch is in GRD position
      if (schalter[index].intCommand==6) {UHFStatusMode=true;} //activate status mode on bupradio
      if (schalter[index].intCommand==7) {UHFTestMode=true;}   //activate test mode on bupradio
      if (schalter[index].intCommand==8) {UHFVolumeOff=false;}   //UHF Volume is turned off
    }
  }
}


///Loop through the data container and send a message to the BMSAIT App to request data update for each entry
void PullRequestBUP()
{
  //in every loop run, only one data entry update will be requested. This avoids data congestion and flickering of attached displays
  if (BUPpull>=20)
    {BUPpull=1;}
  else
    {BUPpull++;}
  
  //build message string <pos>,<vartype>,<varID>
  char nachricht[12]="";
  char pos[2]="";
  itoa(BUPpull,pos,10);  //write data container position as character
  if (BUPpull<10) 
    {
      nachricht[0]='1'; 
      nachricht[1]='0';
      nachricht[2]=pos[0];
    }
  else 
    {
      nachricht[0]='1'; 
      nachricht[1]=pos[0];
      nachricht[2]=pos[1];
    }
  nachricht[3]=',';
  nachricht[4]=BUPRadioFreq[BUPpull].format;  //add the variable type
  nachricht[5]=',';
  for (byte lauf=0;lauf<5;lauf++)
  {nachricht[6+lauf]=BUPRadioFreq[BUPpull].ID[lauf];} //add the variable ID
  nachricht[11]='\0';
  SendMessage(nachricht,2);
  byte x=0;
  while ((SERIALCOM.available()<6) && (x<40)) //wait for answer, but no longer than 40ms
  {
    delay(1);
    x++;  
  }
  while(SERIALCOM.available()>1)  //read incoming data     
  {
    delay(2);
    ReadResponse();
  }
}

//mod
void DebugReadbackBUP(byte posID)
{
  byte laenge=sizeof(BUPRadioFreq[posID].wert);
  char antwort[laenge+4];
  char pos[4]="";
  itoa(posID,pos,10);
  if (posID<10)
  {
    antwort[0]=pos[0];
    antwort[1]=' ';
    antwort[2]=' ';
  }
  else
  {
    antwort[0]=pos[0];
    antwort[1]=pos[1];
    antwort[2]=' ';
  }
  for (byte lauf=0;lauf<laenge;lauf++)
    {antwort[lauf+3]=BUPRadioFreq[posID].wert[lauf];}
  antwort[laenge+3]='\0';
  SendMessage(antwort,1);
}
//mod
