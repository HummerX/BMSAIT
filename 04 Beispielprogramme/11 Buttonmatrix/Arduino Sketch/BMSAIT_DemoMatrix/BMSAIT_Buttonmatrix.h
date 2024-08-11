// This module allows to read button states in a matrix configuration


byte rows[] = {2,3,4};
const int rowCount = sizeof(rows)/sizeof(rows[0]);
 
byte cols[] = {10,11};
const int colCount = sizeof(cols)/sizeof(cols[0]);

byte keys[colCount][rowCount];

//define the signal to be send to the BMSAIT App when a button is pressed
char keyDown[colCount][rowCount][3]=
{
// row1       row2       row3 
 {   "01",    "02",    "03" } //col 1
,{   "04",    "06",    "00" } //col 2
};

//define the signal to be send to the BMSAIT App when a button is released
char keyUp[colCount][rowCount][3]=
{
// row1       row2       row3 
 {   "00",    "00",    "00" } //col 1
,{   "05",    "07",    "00" } //col 2
};

//memorize last switch position
byte keyStatus[colCount][rowCount];   


void SetupButtonMatrix() 
{
    for(int rowIndex=0; rowIndex<rowCount; rowIndex++)
    {
        pinMode( rows[rowIndex], INPUT_PULLUP);
    }
    for (int colIndex=0; colIndex<colCount; colIndex++) 
    {
        pinMode(cols[colIndex], OUTPUT);
        digitalWrite(cols[colIndex], HIGH);
    }    

    for (int colIndex=0; colIndex<colCount; colIndex++) 
    {
      for(int rowIndex=0; rowIndex<rowCount; rowIndex++)
      {
         keyStatus[colIndex][rowIndex]=0;
      }
    }
}

 
void ButtonMatrixRead() 
{  
  for (int colIndex=0; colIndex < colCount; colIndex++) 
  {
      // iterate through the columns
      byte curCol = cols[colIndex];
      
      digitalWrite(curCol, LOW);

      // iterate through the rows
      for (int rowIndex=0; rowIndex < rowCount; rowIndex++) 
      { 
          byte curRow = rows[rowIndex];
          if (digitalRead(curRow)!=keyStatus[colIndex][rowIndex])
          {
            delay(5); //debouncing
            if (digitalRead(curRow)!=keyStatus[colIndex][rowIndex])
            {
              if (keyStatus[colIndex][rowIndex]==1)
              {
                //key pressed
                keyStatus[colIndex][rowIndex]=0;
                if ((keyDown[colIndex][rowIndex][0] != '0') || (keyDown[colIndex][rowIndex][1] != '0')) // only send a message if a command is defined
                {
                  SendMessage(keyDown[colIndex][rowIndex],3);
                  delay(50);
                }
              }
              else
              {
                //key released
                keyStatus[colIndex][rowIndex]=1;
                if ((keyUp[colIndex][rowIndex][0] != '0') || (keyUp[colIndex][rowIndex][1] != '0')) // only send a message if a command is defined
                {
                  SendMessage(keyUp[colIndex][rowIndex],3);
                  delay(50);
                }
              }
            }  
          }
          //disable the row

      }
      // disable the column
      digitalWrite(curCol, HIGH);
  }
}
 
