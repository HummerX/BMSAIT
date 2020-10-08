// This module allows to read button states in a matrix configuration


byte rows[] = {2,3,4};
const int rowCount = sizeof(rows)/sizeof(rows[0]);
 
byte cols[] = {10,11};
const int colCount = sizeof(cols)/sizeof(cols[0]);

byte keys[colCount][rowCount];

//define the signal to be send to the BMSAIT App when a button is pressed
byte keysignal[colCount][rowCount]=

{
// row1  row2   row3 
 {   1,    2,    3 } //col 1
,{   4,    5,    6 } //col 2
};

void SetupButtonMatrix() 
{
    for(int rowIndex=0; rowIndex<rowCount; rowIndex++)
    {
        pinMode(rows[rowIndex], INPUT);
    }
    for (int colIndex=0; colIndex<colCount; colIndex++) 
    {
        pinMode(cols[colIndex], INPUT_PULLUP);
    }    
}
 
void ButtonmatrixRead() 
{
    for (int colIndex=0; colIndex < colCount; colIndex++) 
    {
        // interate through the columns
        byte curCol = cols[colIndex];
        pinMode(curCol, OUTPUT);
        digitalWrite(curCol, LOW);
 
        // interate through the rows
        for (int rowIndex=0; rowIndex < rowCount; rowIndex++) 
        {
            byte rowCol = rows[rowIndex];
            pinMode(rowCol, INPUT_PULLUP);
            if (digitalRead(rowCol)==0)
            {
              if (keys[colIndex][rowIndex]==0)
              {
                delay(10); //wait to make sure that this is real button press
                if (keys[colIndex][rowIndex]==0)
                {
                  char buf[4]; 
                  itoa(keysignal[colIndex][rowIndex],buf,10);
                  buf[3]='\0';
                  SendMessage(buf,3);
                  keys[colIndex][rowIndex] = 1;
                }
              }  
            }
            else
            {
              keys[colIndex][rowIndex] = 0;
            }
            //disable the row
            pinMode(rowCol, INPUT);
        }
        // disable the column
        pinMode(curCol, INPUT);
    }
}
 
