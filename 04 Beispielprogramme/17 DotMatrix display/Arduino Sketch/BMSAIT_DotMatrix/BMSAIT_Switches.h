// This module provides functions to read switches and send commands to the BMSAIT windows application
// define any attached input controls (buttons, switches) and the command to be send back to Windows.
// V1.3.7 26.09.2021

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
  char command[3];      // command to be send (2 digits, i.e. "01")
  bool ext;              // defines if a command is used within the arduino enviroment or to be send to the Windows app
  uint16_t untergrenze;  // lowest value of the analog read that will initiate this command (0..1024)
  uint16_t obergrenze;   // highest value of the analog read that will initiate this command (0..1024)
} Rotary;


Switch schalter[]=
{
//Switch definition. If you add a switch, add a line to the following list 
//,{<PIN>,<description>,<type>,<rotarySwitchID>,0,<commandID when pressed>,<commandID when released>,<internal command>}
 ,{   9,   "CHAFF"  ,    2,          0,        0,        "24",                   "25",                   3}    //Chaff ON/OFF
};
const byte anzSchalter = sizeof(schalter)/sizeof(schalter[0]);    
    
//define commands for analog readings of a rotary switch or poti. This is an example for a 10-position switch

#define STATES 6  //number of positions of the rotary switch(es)
Rotary analogSchalter[][STATES]=
{
 {
  //  {<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {      "01",         true,               0,               102}    //MODE OFF
  ,{      "02",         true,             103,               307}    //MODE STDBY
  ,{      "03",         true,             308,               512}    //MODE MAN
  ,{      "04",         true,             513,               717}    //MODE SEMI
  ,{      "05",         true,             718,               922}    //MODE AUTO
  ,{      "06",         true,             923,              1024}    //MODE BYP
  
 },
 {
  //  {<CommandID>,<externalCommand>,<low threshold>,<high threshold>}
   {      "07",         true,               0,               128}    //PRGM BIT
  ,{      "08",         true,             129,               384}    //PRGM 1
  ,{      "09",         true,             385,               640}    //PRGM 2
  ,{      "10",         true,             641,               896}    //PRGM 3
  ,{      "11",         true,             897,              1024}    //PRGM 4
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

            if (((currentPINState > schalter[index].lastPINState + 50) || ((schalter[index].lastPINState > 50) && (currentPINState < schalter[index].lastPINState - 50))) || all)      //rotary switch got moved
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
