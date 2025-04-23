// V1.2 24.04.2022
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
 // <PIN>,<description>,<type>,<rotarySwitchID>,  0, <commandID when pressed>,<commandID when released>,<internal command> 
  {   A0,   "MODE"   ,    3,          0,        0,        "00",                   "00",                   1}    //MODE Rotary
 ,{   A1,   "PGRM"   ,    3,          1,        0,        "00",                   "00",                   0}    //PGRM Rotary
 ,{   11,   "MWS"    ,    2,          0,        0,        "22",                   "23",                   0}    //MWS ON/OFF 
 ,{   6,    "JETT"   ,    2,          0,        0,        "16",                   "17",                   0}    //JETT ON/OFF
 ,{   12,   "JMR"    ,    2,          0,        0,        "24",                   "25",                   0}    //JMR ON/OFF
 ,{   13,   "RWR"    ,    2,          0,        0,        "26",                   "27",                   0}    //RWR ON/OFF
 ,{   10,   "O1"     ,    2,          0,        0,        "20",                   "21",                   0}    //O1 ON/OFF
 ,{   9,    "O2"     ,    2,          0,        0,        "18",                   "19",                   0}    //O2 ON/OFF
 ,{   8,    "CHAFF"  ,    2,          0,        0,        "09",                   "10",                   3}    //Chaff ON/OFF
 ,{   7,    "FLARE"  ,    2,          0,        0,        "07",                   "08",                   2}    //FLARE ON/OFF
};
const byte numSwitches = sizeof(switches)/sizeof(switches[0]);        
     
//define commands for analog readings of a rotary switch or poti. This is an example for a 10-position switch
//{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}

#define STATES 6  //number of positions of the rotary switch(es)
Rotary analogInput[][STATES]=
{
 {
  //  {<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {      "01",         true,               0,               152}    //MODE OFF
  ,{      "02",         true,             153,               307}    //MODE STDBY
  ,{      "03",         true,             308,               512}    //MODE MAN
  ,{      "04",         true,             513,               717}    //MODE SEMI
  ,{      "05",         true,             718,               922}    //MODE AUTO
  ,{      "06",         true,             923,              1024}    //MODE BYP
 },
 {
  //  {<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {      "11",         true,               0,               128}    //PRGM BIT
  ,{      "12",         true,             129,               384}    //PRGM 1
  ,{      "13",         true,             385,               640}    //PRGM 2
  ,{      "14",         true,             641,               896}    //PRGM 3
  ,{      "15",         true,             897,              1024}    //PRGM 4
  ,{      "00",         false,              0,                 0}    
 
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
