#include <LiquidCrystal.h>
#include <Keypad.h>

const int rs = 13, en = 12, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char * LargestText = " Welcome to the ardruino kitchen timer. ";
int iLineNumber = 0;
int iCursor = 1;

String userInput = "";

const byte ROWS = 4; // four rows
const byte COLS = 4; // three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A0, A1, 11, 10}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; // connect to the column pinouts of the keypad
int LCDCol = 0;
int LCDRow = 0;

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

bool keyPressed = false;
bool timerRunning = false;
unsigned long startTime;
int countdownTime; // Countdown time in seconds
bool timeUp = false;

const int buzzerPin = A2; 

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  lcd.print("Press any key...");
  lcd.setCursor(LCDCol, LCDRow);
  pinMode(buzzerPin, OUTPUT); // Buzzer pin as output
  Serial.begin(9600);
}

void loop() {
  if (!keyPressed) {
    lcd.setCursor(1, 0);
    WelcomeDisplay();
    delay(150);
  }
  
  keyControl();
  
  if (timerRunning) {
    updateTimer();
  }

  if (timeUp) {
    tone(buzzerPin, 500); // Beep the buzzer continuously
  }
}

void WelcomeDisplay() {
  int iLenofLargeText = strlen(LargestText); // calculate the length of the string.
  if (iCursor == (iLenofLargeText - 1)) { // reset variable for rollover effect.
    iCursor = 0;
  }
  lcd.setCursor(0, iLineNumber);
  if (iCursor < iLenofLargeText - 16) { // this loop executed for normal 16 characters
    for (int iChar = iCursor; iChar < iCursor + 16; iChar++) {
      lcd.print(LargestText[iChar]); // print a message to the LCD.
    }
  } else {
    for (int iChar = iCursor; iChar < (iLenofLargeText - 1); iChar++) {
      // This code takes care of printing characters of the current string.
      lcd.print(LargestText[iChar]); // print a message to the LCD.
    }
    for (int iChar = 0; iChar <= 16 - (iLenofLargeText - iCursor); iChar++) {
      // remaining characters will be printed in this loop
      lcd.print(LargestText[iChar]); // print the message to the LCD
    }
  }
  iCursor++;
}

void keyControl() {
  char key = keypad.getKey();
  
  // Define the allowed characters
  const char allowedChars[] = "1234567890C*#";
  
  // Check if the pressed key is in the allowed characters list
  if (key && strchr(allowedChars, key) != NULL) {
    if (timeUp) {
      noTone(buzzerPin); // Stop the buzzer
      timeUp = false;
    }

    if (!keyPressed) {
      lcd.clear();
      keyPressed = true;
      LCDCol = 0;  // Reset to the first column
      LCDRow = 0;  // Reset to the first row
      userInput = "";
    }
    
    // Handle allowed keys
    if (key == 'C') {
      // Reset screen on 'C' press
      lcd.clear();
      LCDCol = 0;
      LCDRow = 0;
      iCursor = 0; // Reset welcome message cursor position
      userInput = "";
      timerRunning = false;
      noTone(buzzerPin); // Stop the buzzer if running
    } else {
      // Handle other allowed keys
      Serial.println(key);
      
      if (LCDCol > 4) { // Only allow 4 digits plus colon
        // Do nothing if more than 4 digits
        return;
      }
      
      if (LCDCol == 2) { // Insert colon after 2 digits
        lcd.setCursor(LCDCol, LCDRow);
        lcd.print(':');
        LCDCol++;
      }
      
      lcd.setCursor(LCDCol, LCDRow);
      lcd.print(key);
      userInput += key; // Append the key to the user input string
      LCDCol++;
      
      if (LCDCol == 5) { // Start timer when 4 digits are entered
        startTimer();
      }
    }
  }
}

void startTimer() {
  int minutes = userInput.substring(0, 2).toInt();
  int seconds = userInput.substring(2, 4).toInt();
  countdownTime = minutes * 60 + seconds;
  startTime = millis();
  timerRunning = true;
  timeUp = false;
}

void updateTimer() {
  unsigned long currentTime = millis();
  int elapsedTime = (currentTime - startTime) / 1000; // Elapsed time in seconds
  int remainingTime = countdownTime - elapsedTime;
  
  if (remainingTime <= 0) {
    timerRunning = false;
    timeUp = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time's up!");
    return;
  }
  
  int minutes = remainingTime / 60;
  int seconds = remainingTime % 60;
  
  lcd.setCursor(0, 0);
  lcd.print("Time left: ");
  if (minutes < 10) lcd.print('0');
  lcd.print(minutes);
  lcd.print(':');
  if (seconds < 10) lcd.print('0');
  lcd.print(seconds);
}
