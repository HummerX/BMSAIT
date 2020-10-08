// This module allows to read analog inputs and send changes to the BMSAIT App 

#define ATH 3   //Analog Threshold. A change of the analog value will only be considered as a movement if a change in readings is above the threshold value

typedef struct //data field structure for storage of data variables
  {
    byte pIN;
    short command;
    int val;
  }AAchse;
  
AAchse analogaxis[] = {
   // PIN  Command   Value
    { A0,   2,         0   }
   
};
const int axisCount = sizeof(analogaxis)/sizeof(analogaxis[0]);


void SetupAnalog()
{
    for (int axisIndex=0; axisIndex < axisCount; axisIndex++)
    {  
      //save initial value for each axis
      analogaxis[axisIndex].val=analogRead(analogaxis[axisIndex].pIN);
    }
}

void ReadAnalogAxis()
{

    for (int axisIndex=0; axisIndex < axisCount; axisIndex++) //run through all axis
    {  

      int currAxisVal = analogRead(analogaxis[axisIndex].pIN);

      if (((currAxisVal+ATH)<analogaxis[axisIndex].val) || ((currAxisVal-ATH)>analogaxis[axisIndex].val)) //check if axis got moved
      {
        // send new value to the BMSAIT App
        char buf[9]="        ";
        sprintf (buf, "%03u,%04u",analogaxis[axisIndex].command,currAxisVal);
        SendMessage(buf,4);
        //Debug
          //SendMessage(buf,1);
        //Debug
        
        //memorize new value
        analogaxis[axisIndex].val=currAxisVal;
      }
    }


    
}
