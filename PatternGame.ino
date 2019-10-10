#include <LiquidCrystal.h>
#include <LCDKeypad.h>

LCDKeypad lcd;

const int buzzerPin = 12;
const int datapin = 0;
const int clockpin = 1;
const int latchpin = 2;

byte data = 0;

void setup() {
  //random number setup
  randomSeed(analogRead(13));

  //hardware setup
  pinMode(buzzerPin, OUTPUT);
  pinMode(datapin, OUTPUT);
  pinMode(clockpin, OUTPUT);  
  pinMode(latchpin, OUTPUT);
  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  intro();
  gameplay();
  restart();
}

void shiftWrite(int desiredPin, boolean desiredState) {
  bitWrite(data,desiredPin,desiredState);
  shiftOut(datapin, clockpin, MSBFIRST, data);
  digitalWrite(latchpin, HIGH);
  digitalWrite(latchpin, LOW);
}

void intro() {
  lcd.clear();
  lcd.print("     Follow");
  lcd.setCursor(0, 1);
  lcd.print("  the Pattern!");
  goodTone();
  delay(2000);
  lcd.clear();
  lcd.print("Watch the lights");
  lcd.setCursor(0, 1);
  lcd.print("closely...");
  delay(2000);
}

void restart() {
  bool repeat = false;
  bool currentScreen = false;
  lcd.clear();
  lcd.print("     Again?");
  while (repeat == false) {
    if (currentScreen == false) {
      no();
    } else {
      repeat = yes();
    }
    currentScreen = !currentScreen;
  }
}

bool yes() {
  lcd.setCursor(0, 1);
  lcd.print("  *Yes     No      ");
  if (waitButton() == KEYPAD_SELECT) {
    return true;
  }
  return false;
}

void no() {
  lcd.setCursor(0, 1);
  lcd.print("   Yes    *No      ");
  if (waitButton() == KEYPAD_SELECT) {
    badTone();
    while(true){
      lcd.clear();
      lcd.print("    Goodbye!");
      delay(2000);
      lcd.clear();
      lcd.print(" Please unplug");
      lcd.setCursor(0, 1);
      lcd.print("     me...");
      delay(2000);
    }
  }
}

void gameplay() {
  for (int i = 0; i < 8; i++) {
    shiftWrite(i , LOW); // turn LED off
  }
  int history[8];
  for (int i = 0; i < 8; i++) {
    lcd.clear();
    lcd.print("    Light #");
    lcd.print(i + 1);
    lcd.setCursor(0, 1);
    lcd.print("    out of 8");
    history[i] = random(8);
    flashLED(history[i], 1000);
  }
  for (int i = 0; i < 8; i++) {
    lcd.clear();
    lcd.print(" Please select");
    lcd.setCursor(0, 1);
    lcd.print("   Light #");
    lcd.print(i + 1);
    waitButton();
    if (selectLED() == history[i]) {
      goodTone();
      lcd.clear();
      lcd.print("    CORRECT!");
      for (int i = 0; i < 4; i = i + 2) {
        shiftWrite(i, HIGH);  // turn LED on
      }
      for (int i = 5; i < 8; i = i + 2) {
        shiftWrite(i, HIGH);  // turn LED on
      }
      delay(2000);
      for (int i = 0; i < 4; i = i + 2) {
        shiftWrite(i, LOW);  // turn LED on
      }
      for (int i = 5; i < 8; i = i + 2) {
        shiftWrite(i, LOW);  // turn LED on
      }
    } else {
      badTone();
      lcd.clear();
      lcd.print("     WRONG,");
      lcd.setCursor(0, 1);
      lcd.print("   YOU LOSE!");
      for (int i = 1; i < 4; i = i + 2) {
        shiftWrite(i, HIGH);  // turn LED on
      }
      for (int i = 4; i < 8; i = i + 2) {
        shiftWrite(i, HIGH);  // turn LED on
      }
      delay(2000);
      for (int i = 1; i < 4; i = i + 2) {
        shiftWrite(i, LOW);  // turn LED on
      }
      for (int i = 4; i < 8; i = i + 2) {
        shiftWrite(i, LOW);  // turn LED on
      }
      lcd.clear();
      lcd.print("   Here's the");
      lcd.setCursor(0, 1);
      lcd.print("  Real Pattern");
      for (int j = 0; j< 8; j++) {
        flashLED(history[j], 1000);
      }
      return;
    }
  }
  lcd.clear();
  lcd.print("CONGRATULATIONS!");
  lcd.setCursor(0, 1);
  lcd.print("   YOU WON!");
  for (int i = 0; i < 5; i++) {
    marquee();
  }
}

int selectLED() {
  int selection = 0;
  int buttonPressed = 0;
  while (buttonPressed != KEYPAD_SELECT) {
    shiftWrite(selection, HIGH);  // turn LED on
    int temp = selection;
    buttonPressed = waitButton();
    if (buttonPressed == KEYPAD_UP || buttonPressed == KEYPAD_DOWN) {
      selection = (selection + 4) % 8;
    } else if (buttonPressed == KEYPAD_LEFT) {
        if (selection == 0 || selection == 4) {
          selection = selection + 3;
        } else {
          selection = selection - 1;
        }
    } else if (buttonPressed == KEYPAD_RIGHT) {
        if (selection == 3 || selection == 7) {
          selection = selection - 3;
        } else {
          selection = selection + 1;
        }
    }
    shiftWrite(temp, LOW); // turn LED off
  }
  return selection;
}

void flashLED(int index, int delayTime) {
  shiftWrite(index, HIGH);  // turn LED on
  delay(delayTime);   // pause to slow down the sequence
  shiftWrite(index, LOW); // turn LED off
  delay(delayTime);   // pause to slow down the sequence
}

int waitButton()
{
  int buttonPressed; 
  lcd.blink();
  while((buttonPressed=lcd.button())==KEYPAD_NONE)
  {
  }
  waitReleaseButton();
  delay(50);  
  lcd.noBlink();
  return buttonPressed;
}

void waitReleaseButton()
{
  delay(50);
  while(lcd.button()!=KEYPAD_NONE)
  {
    tone(buzzerPin, 784, 10);
  }
  delay(50);
}

void goodTone() {
  tone(buzzerPin, 294, 100);
  delay(100);
  tone(buzzerPin, 494, 100);
  delay(100);
  tone(buzzerPin, 784, 100);
}

void badTone() {
  tone(buzzerPin, 784, 100);
  delay(100);
  tone(buzzerPin, 494, 100);
  delay(100);
  tone(buzzerPin, 294, 100);
}

void marquee()
{
  int index;
  int delayTime = 200; // Time (milliseconds) to pause between LEDs
                       // Make this smaller for faster switching
  
  // Step through the first four LEDs
  // (We'll light up one in the lower 4 and one in the upper 4)
  
  for(index = 0; index <= 3; index++)
  {
    shiftWrite(index, HIGH);    // Turn a LED on
    shiftWrite(index+4, HIGH);  // Skip four, and turn that LED on
    delay(delayTime);    // Pause to slow down the sequence
    shiftWrite(index, LOW); // Turn both LEDs off
    shiftWrite(index+4, LOW);
  }
}
