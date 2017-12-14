//keyscan code for TeensyLC controller

#define NUM_ROWS 6
#define NUM_COLS 7

#define NOTE_ON B10000000
#define NOTE_OFF B00000000

// input pins (direct in)
int rowPins[6] = {2, 3, 4, 5, 7, 8};
int colPins[7] = {9, 10, 11, 12, 13, 14, 15};

boolean keyPressed[NUM_ROWS][NUM_COLS];
byte keyToNote[NUM_ROWS][NUM_COLS];

void setup()
{
  byte note = 35                      ; //lowest on keyboard default

  for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
  {
    for (int rowCtr = 0; rowCtr < NUM_ROWS; ++rowCtr)
    {
      keyPressed[rowCtr][colCtr] = false; //default to not pressed
      keyToNote[rowCtr][colCtr] = note;
      note++;
    }
  }

  // setup pinmode
  for (int i = 0; i < NUM_ROWS; ++i) {
    pinMode(rowPins[i], INPUT);
  }

  for (int i = 0; i < NUM_COLS; ++i) {
    pinMode(colPins[i], OUTPUT);
  }

  Serial.begin(9600);
  Serial1.begin(9600);
  Serial1.setTX(1);
}


void loop()
{
  for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
  {
    //scan next column
    digitalWrite(colPins[colCtr], HIGH);

    //get row values at this column
    int rowValue[NUM_ROWS];
    for (int i = 0; i < NUM_ROWS; ++i) {
      rowValue[i] = digitalRead(rowPins[i]);
    }

    // process keys pressed
    for (int rowCtr = 0; rowCtr < NUM_ROWS; ++rowCtr)
    {
      if (rowValue[rowCtr] != 0 && !keyPressed[rowCtr][colCtr])
      {
        keyPressed[rowCtr][colCtr] = true;
        noteOn(rowCtr, colCtr);
      }
    }

    // process keys released
    for (int rowCtr = 0; rowCtr < NUM_ROWS; ++rowCtr)
    {
      if (rowValue[rowCtr] == 0 && keyPressed[rowCtr][colCtr])
      {
        keyPressed[rowCtr][colCtr] = false;
        noteOff(rowCtr, colCtr);
      }
    }
    digitalWrite(colPins[colCtr], LOW);
  }
}

void noteOn(int row, int col)
{
  Serial.println("on!");
  Serial.println((NOTE_ON + keyToNote[row][col]));
  Serial1.write(NOTE_ON + keyToNote[row][col]);
}

void noteOff(int row, int col)
{
  Serial.println("off!");
  Serial.println(NOTE_OFF + keyToNote[row][col]);
  Serial1.write(NOTE_OFF + keyToNote[row][col]);
}


