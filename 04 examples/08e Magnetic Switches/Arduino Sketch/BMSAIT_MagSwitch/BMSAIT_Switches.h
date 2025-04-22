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
    { A0,     "AP_RU",     2,         0,          0,           "01",                     "02",                   0}          //Example button: "right Autopilot switch up" on A0
   ,{ A1,     "AP_RD",     2,         0,          0,           "03",                     "02",                   0}          //Example button: "right Autopilot switch down" on A1   
};
const byte numSwitches = sizeof(switches)/sizeof(switches[0]);        
    
//define commands for analog readings of a rotary switch or poti. This is an example for a 10-position switch
//{<CommandID>,<externalCommand>,<low threshold>,<high threshold>}

#define STATES 10  //number of positions of the rotary switch(es)
Rotary analogInput[][STATES]=
{
 {
  //example setting for a 3-position rotary switch
  //  {<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {    "06",         true,               0,             334}     //send command 06 if analog read is between 0 and 334 (of 1024)
  ,{    "07",         true,             335,             668}     //send command 07 if analog read is between 335 and 668 (of 1024)
  ,{    "08",         true,             669,            1024}     //send command 08 if analog read is between 669 and 1024 (of 1024)
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
   //activate this to add an additional rotary switch (example for a 10-position switch)
  /*
 ,{
   {    "09",         true,               0,              64}     //send command 04 if analog read is between 0 and 64 (of 1024)
  ,{    "10",         true,              65,             177}     //send command 05 if analog read is between 65 and 177 (of 1024)
  ,{    "11",         true,             178,             291}     //send command 06 if analog read is between 178 and 291 (of 1024)
  ,{    "12",         true,             292,             405}     //send command 07 if analog read is between 292 and 405 (of 1024)
  ,{    "13",         true,             406,             518}     //send command 08 if analog read is between 406 and 518 (of 1024)
  ,{    "14",         true,             519,             632}     //send command 09 if analog read is between 519 and 632 (of 1024)
  ,{    "15",         true,             633,             746}     //send command 10 if analog read is between 633 and 746 (of 1024)
  ,{    "16",         true,             747,             859}     //send command 11 if analog read is between 747 and 859 (of 1024)
  ,{    "17",         true,             860,             973}     //send command 12 if analog read is between 860 and 973 (of 1024)
  ,{    "18",         true,             974,            1024}     //send command 13 if analog read is between 974 and 1024 (of 1024)
 }
 */
 
  //activate this to add an additional rotary switch (example for a 5-position switch)
  /*    
  ,{
     {"19",true,0,177}      //send command 14 if analog read is between 0 and 64 (of 1024)
    ,{"20",true,178,405}    //send command 15 if analog read is between 178 and 291 (of 1024)
    ,{"21",true,406,632}    //send command 16 if analog read is between 406 and 518 (of 1024)
    ,{"22",true,633,859}    //send command 17 if analog read is between 633 and 746 (of 1024)
    ,{"23",true,860,1024}   //send command 18 if analog read is between 860 and 973 (of 1024)
    ,{"00",false,0,0}
  	,{"00",false,0,0}
	  ,{"00",false,0,0}
	  ,{"00",false,0,0}
	  ,{"00",false,0,0}
	};  
  */
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
