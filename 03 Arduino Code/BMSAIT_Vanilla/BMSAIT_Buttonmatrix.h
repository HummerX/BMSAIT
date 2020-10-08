// This module allows to read button states in a matrix configuration


byte rows[] = {2,3,4,5,6,7,8,9};
const int rowCount = sizeof(rows)/sizeof(rows[0]);
 
byte cols[] = {10,11,12,14,15};
const int colCount = sizeof(cols)/sizeof(cols[0]);

byte keys[colCount][rowCount];

//define the signal to be send to the BMSAIT App when a button is pressed
byte keysignal[colCount][rowCount]=

{
// row1  row2  row3 row4 row5 row6 row7 row8
 {   1,    2,    3,   4,   5,   6,   7,   8, } //col 1
,{   9,   10,   11,  12,  13,  14,  15,  16, } //col 2
,{  17,   18,   19,  20,  21,  22,  23,  24, } //col 3
,{  25,   26,   27,  28,  29,  30,  31,  32, } //col 4
,{  33,   34,   35,  36,  37,  38,  39,  40, } //col 5
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
 
