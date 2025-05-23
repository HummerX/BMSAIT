// V1.3.7 26.09.2021
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
  char command[3];        // command to be send (2 digits, i.e. "01")
  bool ext;                // defines if a command is used within the arduino enviroment or to be send to the Windows app
  uint16_t untergrenze;    // lowest value of the analog read that will initiate this command (0..1024)
  uint16_t obergrenze;     // highest value of the analog read that will initiate this command (0..1024)
} Rotary;


//Switch definition. If you add a switch, add a line to the following list 
Switch schalter[]=
{
//  <PIN>,<description>,<type>,<rotarySwitchID>,    0, <commandID when pressed>,<commandID when released>,<internal command>
    { 11,     "CalS1",     1,         0,            0,           "00",                     "00",                   51}          //calibrate Stepper motor 1
   ,{ 12,     "CalS2",     1,         0,            0,           "00",                     "00",                   52}          //calibrate Stepper motor 2  
};
const byte anzSchalter = sizeof(schalter)/sizeof(schalter[0]);    
    


#define STATES 10  //max number of positions of the rotary switch(es)
Rotary analogSchalter[][STATES]=
{
 {
  //example setting for a 3-position rotary switch
  //  {<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {    "06",         true,               0,             334}     //send command 04 if analog read is between 0 and 334 (of 1024)
  ,{    "07",         true,             335,             668}     //send command 05 if analog read is between 335 and 668 (of 1024)
  ,{    "08",         true,             669,            1024}     //send command 06 if analog read is between 669 and 1024 (of 1024)
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
  ,{    "00",        false,               0,               0}
 }
  
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

void CheckSwitches(bool all)
{
    //Check all buttons/switches for changes and send signals to Windows
    for (byte index = 0; index < anzSchalter; index++)
    {
        if ((schalter[index].typ == 1) || (schalter[index].typ == 2))  //Digitaler Taster, Kippschalter, Drehschalter
        {
            int currentPINState = digitalRead(schalter[index].pIN);
            if ((currentPINState != schalter[index].lastPINState) || all)
            {
                delay(5);
                schalter[index].lastPINState = currentPINState;
                if (currentPINState == 0)
                {
                    if (!(schalter[index].signalOn[0] == '0') || !(schalter[index].signalOn[1] == '0'))
                    {
                        SendMessage(schalter[index].signalOn, 3);
                        if (debugmode)
                        {
                            char buf1[6];
                            byte offset = 0;
                            if (schalter[index].pIN < 10)offset = 0; else offset = 1;
                            itoa(schalter[index].pIN, buf1, 10);
                            buf1[1 + offset] = ',';
                            buf1[2 + offset] = 'O';
                            buf1[3 + offset] = 'n';
                            buf1[4 + offset] = ' ';
                            SendMessage(buf1, 1);
                        }
                        if ((schalter[index].intCommand==253) && !all)
                          {SendMessage("1", 7);} //send internal command to BMSAIT to initiate switch sync
                        if ((schalter[index].intCommand==254) && !all)
                          {SendMessage("2", 7);} //send internal command to BMSAIT to fast calibrate motors
                        if ((schalter[index].intCommand==255) && !all)
                          {SendMessage("3", 7);} //send internal command to BMSAIT for full motor calibration
                    }

                }
                else
                {
                    if (!(schalter[index].signalOff[0] == '0') || !(schalter[index].signalOff[1] == '0'))
                    {
                        SendMessage(schalter[index].signalOff, 3);
                        if (debugmode)
                        {
                            char buf1[6];
                            byte offset = 0;
                            if (schalter[index].pIN < 10)offset = 0; else offset = 1;
                            itoa(schalter[index].pIN, buf1, 10);
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
        else if (schalter[index].typ == 3)  //analoger Drehschalter
        {
            uint16_t currentPINState = analogRead(schalter[index].pIN);

            if (((currentPINState > schalter[index].lastPINState + 50) || ((schalter[index].lastPINState > 50) && (currentPINState < schalter[index].lastPINState - 50))) || all)     //rotary switch got moved
            {
                delay(20);
                currentPINState = analogRead(schalter[index].pIN);
                schalter[index].lastPINState = currentPINState;
                for (byte lauf = 0; lauf < STATES; lauf++)
                {
                    if ((currentPINState >= analogSchalter[schalter[index].switchID][lauf].untergrenze) && (currentPINState <= analogSchalter[schalter[index].switchID][lauf].obergrenze))
                    {
                        SendMessage(analogSchalter[schalter[index].switchID][lauf].command, 3);
                        if (debugmode)
                        {
                            char buf1[4];
                            byte offset = 0;
                            if (schalter[index].pIN < 10)offset = 0; else offset = 1;
                            itoa(schalter[index].pIN, buf1, 10);
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
