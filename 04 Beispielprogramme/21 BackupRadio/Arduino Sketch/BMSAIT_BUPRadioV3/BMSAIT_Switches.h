// This module provides functions to read switches and send commands to the BMSAIT windows application
// define any attached input controls (buttons, switches) and the command to be send back to Windows.


typedef struct //data field structure for switches and buttons
{
  byte pIN;                       //sets the PIN the switch is connected to
  char bezeichnung[6];            //short description (max. 5 characters)
  byte typ;                       //sets the type of switch:  (1) button/momentary switch, (2) permanent switch or (3)rotary switch with analog read
  byte switchID;                  //used to differentiate between multiple rotary switches w/ analog read
  uint16_t lastPINState;               //memorizes last state 
  char signalOn[3];               //sets the command that will be send to the windows app when the switch gets activated
  char signalOff[3];              //sets the command that will be send to the windows app when the switch gets deactivated
  byte intCommand;                 //placeholder to set an internal command (can be used to control actions within the arduino, i.e. turn on/off displays)
} Schalter;
  
  
typedef struct //data field structure to define commands for rotary switches with analog reading
{
  char kommando[3]; // command to be send (2 digits, i.e. "01")
  bool ext;         // defines if a command is used within the arduino enviroment or to be send to the Windows app
  uint16_t untergrenze;  // lowest value of the analog read that will initiate this command (0..1024)
  uint16_t obergrenze;   // highest value of the analog read that will initiate this command (0..1024)
} Drehschalter;


//Switch definition. If you add a switch, add a line to the following list 

Schalter schalter[]=
{
  // <PIN>,<description>,<type>,<rotSwitchID>,    0, <comID  press>,<commID rel>,<int command>
  {   7,     "STATU" ,      1,      0,            0,      "00",         "00",       6}        //STATUS button
 ,{   8,     "TEST"  ,      1,      0,            0,      "00",         "00",       7}        //TEST button
 ,{   9,     "UHFOF" ,      2,      0,            0,      "01",         "00",       1}        //UHF Main Switch - OFF
 ,{  10,     "UHFMA" ,      2,      0,            0,      "02",         "00",       2}        //UHF Main Switch - MAIN
 ,{  11,     "UHFBO" ,      2,      0,            0,      "03",         "00",       2}        //UHF Main Switch - BOTH
 ,{  12,     "MNL"   ,      2,      0,            0,      "04",         "05",       3}        //UHF Mode Switch Manual
 ,{  13,     "GRD"   ,      2,      0,            0,      "06",         "05",       5}        //UHF Mode Switch PRE
 ,{  A0,     "MNL1"   ,     3,      0,            0,      "00",         "00",       0}        //Man Freq Selector1
 ,{  A1,     "MNL2"   ,     3,      1,            0,      "00",         "00",       0}        //Man Freq Selector2
 ,{  A2,     "MNL3"   ,     3,      2,            0,      "00",         "00",       0}        //Man Freq Selector3
 ,{  A3,     "MNL4"   ,     3,      3,            0,      "00",         "00",       0}        //Man Freq Selector4
 ,{  A4,     "MNL5"   ,     3,      4,            0,      "00",         "00",       0}        //Man Freq Selector5
};
const byte anzSchalter = sizeof(schalter)/sizeof(schalter[0]);    
    
//define commands for analog readings of a rotary switch or poti. This is an example for a 10-position switch


#define STATES 10  //number of positions of the rotary switch(es)
Drehschalter analogSchalter[][STATES]=
{
 {//RotSwitch0: 3-position rotary switch X__.___(2,3)
  //{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {    "07",         true,               0,             512}     //send command 06 if analog read is between 0 and 511 (of 1024)
  ,{    "08",         true,             511,            1024}     //send command 07 if analog read is between 512 and 668 (of 1024)
  ,{    "00",        false,               0,               0}     
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
 },
 {//RotSwitch1: 10-position rotary switch _X_.___
  //{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {"09",true,  0, 64}       
  ,{"10",true, 65,177}   
  ,{"11",true,178,291}    
  ,{"12",true,292,405}  
  ,{"13",true,406,518}    
  ,{"14",true,519,632}   
  ,{"15",true,633,746}    
  ,{"16",true,747,859}    
  ,{"17",true,860,973}    
  ,{"18",true,974,1024}   
 },
 {//RotSwitch2: 10-position rotary switch __X.___
  //{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {"19",true,  0, 64}     
  ,{"20",true, 65,177}     
  ,{"21",true,178,291}   
  ,{"22",true,292,405}    
  ,{"23",true,406,518} 
  ,{"24",true,519,632}  
  ,{"25",true,633,746}
  ,{"26",true,747,859}
  ,{"27",true,860,973}
  ,{"28",true,974,1024}
 },
 {//RotSwitch3: 10-position rotary switch ___.X_
  //{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {"29",true,  0, 64} 
  ,{"30",true, 65,177}
  ,{"31",true,178,291}
  ,{"32",true,292,405}
  ,{"33",true,406,518}
  ,{"34",true,519,632} 
  ,{"35",true,633,746}
  ,{"36",true,747,859}
  ,{"37",true,860,973} 
  ,{"38",true,974,1024}
 },
  {//RotSwitch4: 4-position rotary switch ___._XX
   //{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {"39",true,   0,  255} 
  ,{"40",true, 256,  511}
  ,{"41",true, 512,  767}     
  ,{"42",true, 768, 1024}
  ,{"00",false,  0,    0}
  ,{"00",false,  0,    0}
  ,{"00",false,  0,    0}
  ,{"00",false,  0,    0}
  ,{"00",false,  0,    0}
  ,{"00",false,  0,    0}
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
    pinMode(schalter[index].pIN, INPUT_PULLUP);
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
        
        //modification for BUPRadio - call function from F4SME_BUPRadio.h to set internal commands
         CheckSwitchesBUPRadio(index);
        //modification for BUPRadio - set internal commands
        
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
     // experimental code for analog reading of rotary switches 
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
            //debug
            //char buf[5];
            //itoa(currentPINState,buf,10);
            //SendMessage(buf,1);
            //debug
          }
        }
      } 
    }
  }
}
