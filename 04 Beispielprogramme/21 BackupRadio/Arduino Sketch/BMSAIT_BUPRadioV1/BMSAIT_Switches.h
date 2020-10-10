// This module provides functions to read switches and send commands to the F4SME windows application
// define any attached input controls (buttons, switches) and the command to be send back to Windows.


typedef struct //data field structure for switches and buttons
{
  byte pIN;                       //sets the PIN the switch is connected to
  char bezeichnung[6];            //short description (max. 5 characters)
  byte typ;                       //sets the type of switch:  (1) button/momentary switch, (2) permanent switch or (3)rotary switch with analog read
  byte switchID;                  //used to differentiate between multiple rotary switches w/ analog read
  uint16_t lastPINState;          //memorizes last state 
  char signalOn[3];               //sets the command that will be send to the windows app when the switch gets activated
  char signalOff[3];              //sets the command that will be send to the windows app when the switch gets deactivated
  byte intCommand;                //placeholder to set an internal command (can be used to control actions within the arduino, i.e. turn on/off displays)
} Schalter;
  
  
typedef struct //data field structure to define commands for rotary switches with analog reading
{
  char kommando[3]; // command to be send (2 digits, i.e. "01")
  bool ext;         // defines if a command is used within the arduino enviroment or to be send to the Windows app
  uint16_t untergrenze;  // lowest value of the analog read that will initiate this command (0..1024)
  uint16_t obergrenze;   // highest value of the analog read that will initiate this command (0..1024)
} Drehschalter;


Schalter schalter[]=
{
//Switch definition. If you add a switch, add a line to the following list 
//,{<PIN>,<description>,<type>,<rotarySwitchID>,0,<commandID when pressed>,<commandID when released>,<internal command>}
  {   6,    "STATU" ,     1,          0,        0,        "00",                   "00",                   6}    //STATUS button
 ,{   7,    "TEST"  ,     1,          0,        0,        "00",                   "00",                   7}    //TEST button
 ,{   3,    "UHFOF" ,     2,          0,        0,        "01",                   "00",                   1}    //UHF Main Switch - OFF
 ,{   4,    "UHFMA" ,     2,          0,        0,        "02",                   "00",                   2}    //UHF Main Switch - MAIN
 ,{   5,    "UHFBO" ,     2,          0,        0,        "03",                   "00",                   2}    //UHF Main Switch - BOTH
 ,{   8,    "MNL"   ,     2,          0,        0,        "04",                   "05",                   3}    //UHF Mode Switch MAN
 ,{   9,    "GRD"   ,     2,          0,        0,        "06",                   "05",                   5}    //UHF Mode Switch GRD
};
const byte anzSchalter = sizeof(schalter)/sizeof(schalter[0]);    
    
//define commands for analog readings of a rotary switch or poti. This is an example for a 10-position switch


#define STATES 10  //number of positions of the rotary switch(es)
Drehschalter analogSchalter[][STATES]=
{
 {
  //  {<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {      "07",         true,               0,               64}       //send command 14 if analog read is between 0 and 64 (of 1024)
  ,{      "08",         true,              65,              177}     //send command 05 if analog read is between 65 and 177 (of 1024)
  ,{      "09",         true,             178,              291}    //send command 06 if analog read is between 178 and 291 (of 1024)
  ,{      "10",         true,             292,              405}    //send command 07 if analog read is between 292 and 405 (of 1024)
  ,{      "11",         true,             406,              518}    //send command 08 if analog read is between 406 and 518 (of 1024)
  ,{      "12",         true,             519,              632}    //send command 09 if analog read is between 519 and 632 (of 1024)
  ,{      "13",         true,             633,              746}    //send command 10 if analog read is between 633 and 746 (of 1024)
  ,{      "14",         true,             747,              859}    //send command 11 if analog read is between 747 and 859 (of 1024)
  ,{      "15",         true,             860,              973}    //send command 12 if analog read is between 860 and 973 (of 1024)
  ,{      "16",         true,             974,              1024}   //send command 13 if analog read is between 974 and 1024 (of 1024)
 }
};

void SetupSwitches()
{
  for (byte index = 0; index < anzSchalter; index++)
  {
    if ((schalter[index].typ==1) || (schalter[index].typ==2))
    {
    //setup for digital reading of PIN x
    pinMode(schalter[index].pIN, INPUT_PULLUP);
    schalter[index].lastPINState=digitalRead(schalter[index].pIN);
    }
    else
    {
    //setup for analog reading of PIN x
    pinMode(schalter[index].pIN, INPUT);
    schalter[index].lastPINState=analogRead(schalter[index].pIN);  
    }
  }  
}

void CheckSwitches() 
{
  //Check all buttons/switches for changes and send signals to Windows
  for (byte index = 0; index < anzSchalter; index++)
  {
    if ((schalter[index].typ==1) || (schalter[index].typ==2))  //Digitaler Taster, Kippschalter, Drehschalter
    {
      int currentPINState = digitalRead(schalter[index].pIN);
      if (currentPINState != schalter[index].lastPINState)
      { 
        delay(5);        
        schalter[index].lastPINState= currentPINState;
        
        if (currentPINState==0)
        {
          if (!(schalter[index].signalOn[0]=='0') || !(schalter[index].signalOn[1]=='0'))
          {
            SendMessage(schalter[index].signalOn , 3);
          }
        }
        else
        {
          if (!(schalter[index].signalOff[0]=='0') || !(schalter[index].signalOff[1]=='0'))
          {
            SendMessage(schalter[index].signalOff , 3);
          }          
        }
      }
    }
    else if (schalter[index].typ==3)  //analoger Drehschalter
    {
     //read analog value from rotary switches 
      uint16_t currentPINState = analogRead(schalter[index].pIN);
      
      if (((currentPINState > schalter[index].lastPINState+50) || (currentPINState < schalter[index].lastPINState-50) ))     //rotary switch got moved
      { 
        delay(20); 
        currentPINState= analogRead(schalter[index].pIN);   
        schalter[index].lastPINState = currentPINState;
        for (byte lauf=0;lauf<STATES;lauf++)
        {
          if ((currentPINState>=analogSchalter[schalter[index].switchID][lauf].untergrenze) && (currentPINState<analogSchalter[schalter[index].switchID][lauf].obergrenze))
          {
            SendMessage(analogSchalter[schalter[index].switchID][lauf].kommando ,3);
            //if (testmode)
            //  {
            //  char buf[5];
            //  itoa(currentPINState,buf,10);
            //  SendMessage(buf,1);
            //  }
          }
        }
      } 
    }
  }
}
