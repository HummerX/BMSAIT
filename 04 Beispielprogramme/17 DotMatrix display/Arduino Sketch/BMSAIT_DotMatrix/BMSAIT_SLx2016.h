//Drive a 4-digit 5x7 dotmatrix module
// V1.0 5.12.2021

//target= device ID (if multiple displays are connected)
//ref2= not used
//ref3= number of figures to display
//ref4= offset position 
//ref5= not used

//PIN Layout
// 1-WR Write               8-D3 Data
// 2-A1 Digit Select        9-D4 Data
// 3-A0 Digit Select       10-D5 Data
// 4-Vcc                   11-D6 Data
// 5-D0 Data               12-BL Display Blank
// 6-D1 Data               13-CLR Clear
// 7-D2 Data               14-GND

typedef struct //data field structure for storage of data variables
{
  byte WR_PIN;                    //Arduino PIN that is connected to the PIN 1 of the module (WR)
  byte AdressPIN2;                //Arduino PIN that is connected to the PIN 2 of the module (A1)
  byte AdressPIN1;                //Arduino PIN that is connected to the PIN 3 of the module (A0)
  byte DataPIN[7];                //Arduino PINs that are connected to PIN 5-11 of the module (D0-D6)
} SLx2016Modul;

SLx2016Modul slxModules[] =
{ //WritePIN   Adress1+0    DataPINs
    {  A0,      A2, A3,     2, 3, 4, 5, 6, 7, 8     } //Modul1
  , {  A1,      A2, A3,     2, 3, 4, 5, 6, 7, 8     } //Modul2 (adress-PINs and data-PINs can be shared between multiple modules)
};
const int SLx2016ANZAHL = sizeof(slxModules) / sizeof(slxModules[0]);

 
void WriteSLx2016Display(byte SLDevice, char fig[],byte ZifferAnz=4, byte ZifferStart=0)
{
  digitalWrite(slxModules[SLDevice].WR_PIN, HIGH);   //Schreibkommando unterbrechen
  delayMicroseconds(200);
  for (byte SLZiffer = 0; SLZiffer < ZifferAnz; SLZiffer++)
  {
    digitalWrite(slxModules[SLDevice].AdressPIN1, bitRead(ZifferStart+SLZiffer, 0));  //Ziffer auswählen
    digitalWrite(slxModules[SLDevice].AdressPIN2, bitRead(ZifferStart+SLZiffer, 1));  //Ziffer auswählen 
    char SLZahl=fig[ZifferAnz-SLZiffer-1];

    for (byte dataBitPos = 0; dataBitPos < 7; dataBitPos++)
    {
      digitalWrite(slxModules[SLDevice].DataPIN[dataBitPos], bitRead(SLZahl, dataBitPos)); //Datenpins setzen
    }
    delayMicroseconds(200);
    digitalWrite(slxModules[SLDevice].WR_PIN, LOW); //write data to selected letter
    delayMicroseconds(100);
    digitalWrite(slxModules[SLDevice].WR_PIN, HIGH); //stop write mode again
  }
}

void SetupSLx2016()
{
  for (byte SLDevice = 0; SLDevice < SLx2016ANZAHL; SLDevice++)
  {
    for (byte SLZiffer = 0; SLZiffer < 7; SLZiffer++)
    {
      pinMode(slxModules[SLDevice].DataPIN[SLZiffer], OUTPUT);
    }
    pinMode(slxModules[SLDevice].AdressPIN1, OUTPUT);
    pinMode(slxModules[SLDevice].AdressPIN2, OUTPUT);
    pinMode(slxModules[SLDevice].WR_PIN, OUTPUT);
       
  }
  for (byte SLDevice = 0; SLDevice < SLx2016ANZAHL; SLDevice++)
  {
    WriteSLx2016Display(SLDevice,"BMS",3,0);
  }
  delay(1000);
  for (byte SLDevice = 0; SLDevice < SLx2016ANZAHL; SLDevice++)
  {
    WriteSLx2016Display(SLDevice,"AIT",3,1);
    WriteSLx2016Display(SLDevice," ",1,0);
  }
  delay(1000);
  for (byte SLDevice = 0; SLDevice < SLx2016ANZAHL; SLDevice++)
  {
    WriteSLx2016Display(SLDevice,"    ",4,0);
  }
} 


void UpdateSLx2016(byte p)
{
  //modifier for CMDS
  memcpy(Wert, datenfeld[p].wert, DATENLAENGE);
  CMDSUpdate(p); //look for modifiers
  //modifier for CMDS
  
  WriteSLx2016Display(datenfeld[p].target, Wert, datenfeld[p].ref3, datenfeld[p].ref4);      
}
