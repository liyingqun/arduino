#include <Wire.h>
#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

int Button = 10;           // Push Button

const byte numRows = 4;    // Number of rows on the keypad
const byte numCols = 4;    // Number of columns on the keypad

char keymap[numRows][numCols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char keypressed;
char code[] = {'1', '2', '3', '4'}; // Default code
char check1[sizeof(code)];          // For new code input
char check2[sizeof(code)];          // For confirmation input

short a = 0, i = 0, s = 0, j = 0;

byte rowPins[numRows] = {2, 3, 4, 5}; // Row pins
byte colPins[numCols] = {6, 7, 8, 9}; // Column pins

LiquidCrystal_I2C lcd(0x27, 16, 2);
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
Servo myservo;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("*ENTER THE CODE*");
  lcd.setCursor(1, 1);
  lcd.print("TO opn (Door)!!");
  pinMode(Button, INPUT);
  myservo.attach(11);
  myservo.write(0);

  // Initialize EEPROM with default code (do this only once, then comment out)
  for (i = 0; i < sizeof(code); i++) {
    EEPROM.write(i, code[i]);
  }

  // Load code from EEPROM
  for (i = 0; i < sizeof(code); i++) {
    code[i] = EEPROM.read(i);
  }
}

void loop() {
  keypressed = myKeypad.getKey();
  if (keypressed == '*') { // Attempt to open lock
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("*ENTER THE CODE*");
    ReadCode();
    if (a == sizeof(code)) {
      OpenDoor();
    } else {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("CODE INCORRECT");
      delay(2000);
    }
    ResetLCD();
  }
  if (keypressed == '#') { // Change code
    ChangeCode();
    ResetLCD();
  } 
  if (digitalRead(Button) == HIGH) { // Open using push button
    myservo.write(0);
  }
}

void ReadCode() {
  char input[sizeof(code)] = {0};
  i = 0;
  a = 0;
  j = 0;
  while (keypressed != 'A') {
    keypressed = myKeypad.getKey();
    if (keypressed != NO_KEY && keypressed != 'A') {
      if (keypressed == 'D' && j > 0) { // Delete last character
        j--;
        lcd.setCursor(j, 1);
        lcd.print(" ");
        lcd.setCursor(j, 1);
      } else if (j < sizeof(code)) {
        lcd.setCursor(j, 1);
        lcd.print("*");
        input[j] = keypressed;
        j++;
      }
    }
  }
  // Verify code
  for (i = 0; i < sizeof(code); i++) {
    if (input[i] == code[i]) {
      a++;
    }
  }
}

void ChangeCode() {
  lcd.clear();
  lcd.print("Enter old code");
  ReadCode();

  if (a == sizeof(code)) { // Old code matches
    lcd.clear();
    lcd.print("Changing code...");
    delay(1000);

    GetNewCode1();
    GetNewCode2();

    s = 0;
    for (i = 0; i < sizeof(code); i++) {
      if (check1[i] == check2[i]) {
        s++;
      }
    }

    if (s == sizeof(code)) { // New codes match
      for (i = 0; i < sizeof(code); i++) {
        code[i] = check2[i];
        EEPROM.write(i, code[i]);
      }
      lcd.clear();
      lcd.print("Code Changed!");
    } else {
      lcd.clear();
      lcd.print("Codes mismatch!");
    }
  } else {
    lcd.clear();
    lcd.print("Old Code Wrong!");
  }
  delay(2000);
}

void GetNewCode1() {
  lcd.clear();
  lcd.print("Enter new code");
  delay(1000);
  lcd.clear();
  j = 0;
  keypressed = NO_KEY;

  while (keypressed != 'A') {
    keypressed = myKeypad.getKey();
    if (keypressed != NO_KEY && keypressed != 'A') {
      lcd.setCursor(j, 1);
      lcd.print("*");
      check1[j] = keypressed;
      j++;
    }
  }
}

void GetNewCode2() {
  lcd.clear();
  lcd.print("Confirm code");
  delay(1000);
  lcd.clear();
  j = 0;
  keypressed = NO_KEY;

  while (keypressed != 'A') {
    keypressed = myKeypad.getKey();
    if (keypressed != NO_KEY && keypressed != 'A') {
      lcd.setCursor(j, 1);
      lcd.print("*");
      check2[j] = keypressed;
      j++;
    }
  }
}

void OpenDoor() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Open!");
  myservo.write(90); // Unlock door
  delay(5000);
  myservo.write(0);  // Lock door
}

void ResetLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("*ENTER THE CODE*");
  lcd.setCursor(1, 1);
  lcd.print("TO _/_ (Door)!!");
}