// This module provides functions to read switches and send commands to the BMSAIT windows application
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
} Switch;
  
  
typedef struct //data field structure to define commands for rotary switches with analog reading
{
  char kommando[3];      // command to be send (2 digits, i.e. "01")
  bool ext;              // defines if a command is used within the arduino enviroment or to be send to the Windows app
  uint16_t untergrenze;  // lowest value of the analog read that will initiate this command (0..1024)
  uint16_t obergrenze;   // highest value of the analog read that will initiate this command (0..1024)
} Rotary;


Switch schalter[]=
{
//Switch definition. If you add a switch, add a line to the following list 
//,{<PIN>,<description>,<type>,<rotarySwitchID>,0,<commandID when pressed>,<commandID when released>,<internal command>}
  {   2,    "M-OFF"  ,    2,          0,        0,        "01",                   "00",                   0}    //MODE Rotary
 ,{   3,    "M-STB"  ,    2,          0,        0,        "02",                   "00",                   1}    //MODE Rotary
 ,{   4,    "M-MAN"  ,    2,          0,        0,        "03",                   "00",                   1}    //MODE Rotary
 ,{   5,    "M-SEM"  ,    2,          0,        0,        "04",                   "00",                   1}    //MODE Rotary
 ,{   6,    "M_AUT"  ,    2,          0,        0,        "05",                   "00",                   1}    //MODE Rotary
 ,{   7,    "M_BYP"  ,    2,          0,        0,        "06",                   "00",                   1}    //MODE Rotary 
 ,{   8,    "FLARE"  ,    2,          0,        0,        "07",                   "08",                   2}    //CHAFF ON/OFF 
 ,{   9,    "CHAFF"  ,    2,          0,        0,        "09",                   "10",                   3}    //FLARE ON/OFF
};
const byte anzSchalter = sizeof(schalter)/sizeof(schalter[0]);    
    
//define commands for analog readings of a rotary switch or poti. This is an example for a 10-position switch

#define STATES 10  //number of positions of the rotary switch(es)
Rotary analogSchalter[][STATES]=
{
 {
  //  {<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {      "17",         true,               0,               102}    
  ,{      "18",         true,             103,               307}    
  ,{      "19",         true,             308,               512}    
  ,{      "20",         true,             513,               717}    
  ,{      "21",         true,             718,               922}    
  ,{      "22",         true,             923,              1024}    
  ,{      "00",         false,              0,                 0}    
  ,{      "00",         false,              0,                 0}   
  ,{      "00",         false,              0,                 0}    
  ,{      "00",         false,              0,                 0}    
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
    if ((schalter[index].typ==1) || (schalter[index].typ==2))  //Digitaler Taster, Kippschalter
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
            if (testmode)
              {
              char buf[5];
              itoa(currentPINState,buf,10);
              SendMessage(buf,1);
              }
          }
        }
      } 
    }
  }
}
