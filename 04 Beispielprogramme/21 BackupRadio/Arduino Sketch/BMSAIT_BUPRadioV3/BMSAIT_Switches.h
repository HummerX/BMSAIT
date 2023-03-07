// This module provides functions to read switches and send commands to the BMSAIT windows application
// define any attached input controls (buttons, switches) and the command to be send back to Windows.


typedef struct //data field structure for switches and buttons
{
  byte pIN;                       //sets the PIN the switch is connected to
  char bezeichnung[6];            //short description (max. 5 characters)
  byte typ;                       //sets the type of switch:  (1) button/momentary switch, (2) permanent switch, (3)rotary switch with analog read (4) Rotary encoder
  byte switchID;                  //used to differentiate between multiple rotary switches w/ analog read
  uint16_t lastPINState;          //memorizes last state 
  char signalOn[3];               //sets the command that will be send to the windows app when the switch gets activated
  char signalOff[3];              //sets the command that will be send to the windows app when the switch gets deactivated
  byte intCommand;                //placeholder to set an internal command (can be used to control actions within the arduino, i.e. turn on/off displays)
} Switch;
  
  
typedef struct //data field structure to define commands for rotary switches with analog reading
{
  char command[3];      // command to be send (2 digits, i.e. "01")
  bool ext;             // defines if a command is used within the arduino enviroment or to be send to the Windows app
  uint16_t lbound;      // lowest value of the analog read that will initiate this command (0..1024)
  uint16_t ubound;      // highest value of the analog read that will initiate this command (0..1024)
} Rotary;



//Switch definition. If you add a switch, add a line to the following list 

Switch switches[]=
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
 ,{  A5,     "COM1V" ,      2,      0,            0,      "07",         "08",       8}        //COM1 Volume switch ON/OFF
};
const byte numSwitches = sizeof(switches)/sizeof(switches[0]);   
    
//define commands for analog readings of a rotary switch or poti. This is an example for a 10-position switch


#define STATES 10  //number of positions of the rotary switch(es)
Rotary analogInput[][STATES]=
{
 {//RotSwitch0: 2-position rotary switch X__.___(2,3)
  //{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {    "09",         true,               0,             512}     //send command 06 if analog read is between 0 and 511 (of 1024)
  ,{    "10",         true,             511,            1024}     //send command 07 if analog read is between 512 and 668 (of 1024)
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
   {"11",true,  0, 64}       
  ,{"12",true, 65,177}   
  ,{"13",true,178,291}    
  ,{"14",true,292,405}  
  ,{"15",true,406,518}    
  ,{"16",true,519,632}   
  ,{"17",true,633,746}    
  ,{"18",true,747,859}    
  ,{"19",true,860,973}    
  ,{"20",true,974,1024}   
 },
 {//RotSwitch2: 10-position rotary switch __X.___
  //{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {"21",true,  0, 64}     
  ,{"22",true, 65,177}     
  ,{"23",true,178,291}   
  ,{"24",true,292,405}    
  ,{"25",true,406,518} 
  ,{"26",true,519,632}  
  ,{"27",true,633,746}
  ,{"28",true,747,859}
  ,{"29",true,860,973}
  ,{"30",true,974,1024}
 },
 {//RotSwitch3: 10-position rotary switch ___.X_
  //{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {"31",true,  0, 64} 
  ,{"32",true, 65,177}
  ,{"33",true,178,291}
  ,{"34",true,292,405}
  ,{"35",true,406,518}
  ,{"36",true,519,632} 
  ,{"37",true,633,746}
  ,{"38",true,747,859}
  ,{"39",true,860,973} 
  ,{"40",true,974,1024}
 },
  {//RotSwitch4: 4-position rotary switch ___._XX
   //{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {"41",true,   0,  255} 
  ,{"42",true, 256,  511}
  ,{"43",true, 512,  767}     
  ,{"44",true, 768, 1024}
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
  for (byte index = 0; index < numSwitches; index++)
  {
    if ((switches[index].typ==1) || (switches[index].typ==2))
    {
    //setup for digital reading of PIN x
    pinMode(switches[index].pIN, INPUT_PULLUP);
    switches[index].lastPINState=digitalRead(switches[index].pIN);
    }
    else if (switches[index].typ==3)
    {
    //setup for analog reading of PIN x
    pinMode(switches[index].pIN, INPUT_PULLUP);
    switches[index].lastPINState=analogRead(switches[index].pIN);  
    }
    else if (switches[index].typ==4)
    {
    //setup rotary encoders
    }    
    else
    {}
  }  
}

void CheckSwitches(bool all)
{
    //Check all buttons/switches for changes and send signals to Windows
    for (byte index = 0; index < numSwitches; index++)
    {
        if ((switches[index].typ == 1) || (switches[index].typ == 2))  //Digitaler Taster, Kippschalter, Drehschalter
        {
            int currentPINState = digitalRead(switches[index].pIN);
            if ((currentPINState != switches[index].lastPINState) || all)
            {
                delay(5);
                switches[index].lastPINState = currentPINState;
                if (currentPINState == 0)
                {
                    if (!(switches[index].signalOn[0] == '0') || !(switches[index].signalOn[1] == '0'))
                    {
                        SendMessage(switches[index].signalOn, 3);
                        if (debugmode)
                        {
                            char buf1[6];
                            byte offset = 0;
                            if (switches[index].pIN < 10)offset = 0; else offset = 1;
                            itoa(switches[index].pIN, buf1, 10);
                            buf1[1 + offset] = ',';
                            buf1[2 + offset] = 'O';
                            buf1[3 + offset] = 'n';
                            buf1[4 + offset] = ' ';
                            SendMessage(buf1, 1);
                        }
                        if ((switches[index].intCommand==253) && !all)
                          {SendMessage("1", 7);} //send internal command to BMSAIT to initiate switch sync
                        if ((switches[index].intCommand==254) && !all)
                          {SendMessage("2", 7);} //send internal command to BMSAIT to fast calibrate motors
                        if ((switches[index].intCommand==255) && !all)
                          {SendMessage("3", 7);} //send internal command to BMSAIT for full motor calibration
                    }
                }
                else
                {
                    if (!(switches[index].signalOff[0] == '0') || !(switches[index].signalOff[1] == '0'))
                    {
                        SendMessage(switches[index].signalOff, 3);
                        if (debugmode)
                        {
                            char buf1[6];
                            byte offset = 0;
                            if (switches[index].pIN < 10)offset = 0; else offset = 1;
                            itoa(switches[index].pIN, buf1, 10);
                            buf1[1 + offset] = ',';
                            buf1[2 + offset] = 'O';
                            buf1[3 + offset] = 'f';
                            buf1[4 + offset] = 'f';
                            SendMessage(buf1, 1);
                        }
                    }
                }
            }
        }
        else if (switches[index].typ == 3)  //analog rotary switch
        {
            uint16_t currentPINState = analogRead(switches[index].pIN);

            if (((currentPINState > switches[index].lastPINState + 50) || ((switches[index].lastPINState > 50) && (currentPINState < switches[index].lastPINState - 50))) || all)     //rotary switch got moved
            {
                delay(20);
                currentPINState = analogRead(switches[index].pIN);
                switches[index].lastPINState = currentPINState;
                for (byte lauf = 0; lauf < STATES; lauf++)
                {
                    if ((currentPINState >= analogInput[switches[index].switchID][lauf].lbound) && (currentPINState <= analogInput[switches[index].switchID][lauf].ubound))
                    {
                        SendMessage(analogInput[switches[index].switchID][lauf].command, 3);
                        if (debugmode)
                        {
                            char buf1[4];
                            byte offset = 0;
                            if (switches[index].pIN < 10)offset = 0; else offset = 1;
                            itoa(switches[index].pIN, buf1, 10);
                            buf1[1 + offset] = ',';
                            buf1[2 + offset] = '\0';
                            char buf2[5];
                            itoa(currentPINState, buf2, 10);
                            char buf3[9] = "";
                            strcpy(buf3, buf1);
                            strcat(buf3, buf2);
                            SendMessage(buf3, 1);
                        }
                    }
                }
            }
        }
    }
}
