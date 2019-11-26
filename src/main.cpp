// MicroKeyring
// Portable users and passwords device emulating a real keyboard. It types your credentials like a real person.
// For DIYmore Beetle BadUSB Micro (atmega32u4 like Arduino Leonardo board)
// Author: Alfonso de Cala <alfonso@el-magnifico.org>

// Requirements
//   Set up Arduino IDE Board as Leonardo
//   Install this button library https://github.com/jonnieZG/EButton

// Action button:
//   - Short click: type the selected user
//   - Long press: type the selected password
//   - Doble click: type the additional data

// Select button:
//   - Short click: Select next user
//   - Long press: Switch on/off 'heartbeat' feature. Periodic small mouse move to avoid screensaver trigger
//   - Doble click: Restart with first user

#include <Arduino.h>

#include <Keyboard.h> 
#include <Mouse.h>
#include <EButton.h>

int ledPin = 13; 
EButton selectButton(11);
EButton actionButton(10);

const int maxUsers=3;
String users[maxUsers] = { "youruser1", "youruser2", "youruser3" };
String passwords[maxUsers] = { "secretpassword1", "secretpassword2", "secretpassword3" };
String others[maxUsers] = { "favouriteurl1", "creditcardnumber2", "macro3" };
int currentUser=0;

bool heartbeat = false;
const unsigned long heartbeatInterval = 30000;
unsigned long lasttime=millis();
unsigned long interval;

// String containing special chars that need translation, for example in non-US keyboards
// Check https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/
//       http://kbdlayout.info/
const String specialChars="@-_!/&;:";
const unsigned int scanCodes[][2]={
  {KEY_RIGHT_ALT,'2'},   // @
  {0x00, '/'},  // -
  {0x00, '?'},  // _
  {KEY_RIGHT_SHIFT, '0x01'},  // !
  {0x00, '&'},  // /
  {0x00, '^'},  // &
  {0x00, '<'},  // ;
  {0x00, '>'},  // :
  };

void typeSpecialChar(char c) {

}

void typeSpecialString(String s) {
  for (unsigned int c=0;c < s.length();c++) {
    int i=specialChars.indexOf(s[c]);
    if (i >= 0) {
      Keyboard.press(scanCodes[i][0]);
      Keyboard.press(scanCodes[i][1]);
      Keyboard.releaseAll();
    }  
    else {    
      Keyboard.write(s[c]);
    }  
  }
  
}


void flashLed(int flashes, int speed) {
    for (int i=0;i<flashes;i++) {
      digitalWrite(ledPin, HIGH);
      delay(100*speed);
      digitalWrite(ledPin, LOW);
      delay(100*speed);
    }
}

void selectButton_singleClick(EButton &btn) {
    Serial.println("Select click");
    currentUser++;
    if (currentUser == maxUsers) { currentUser=0; } 
    flashLed(currentUser+1,1);
}

void selectButton_doubleClick(EButton &btn) {
    Serial.println("Select double click");
    currentUser=0;
    flashLed(currentUser+1,1);
}

void selectButton_longPress(EButton &btn) {
    Serial.println("Select long press");
    typeSpecialChar('@');

//    heartbeat=!heartbeat;
    flashLed(5,5);
}

void actionButton_singleClick(EButton &btn) {
    Serial.println("Action click");
    typeSpecialString(users[currentUser]);
    flashLed(currentUser+1,1);
}

void actionButton_longPress(EButton &btn) {
    Serial.println("Action long press");
    typeSpecialString(passwords[currentUser]);
    flashLed(currentUser+1,1);
}

void actionButton_doubleClick(EButton &btn) {
    Serial.println("Action double click");
    typeSpecialString(others[currentUser]);
    flashLed(currentUser+1,1);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nMiniKeyring 1.0\n\n");

  pinMode(ledPin, OUTPUT);
  selectButton.attachSingleClick(selectButton_singleClick);
  selectButton.attachDoubleClick(selectButton_doubleClick);
  selectButton.attachLongPressStart(selectButton_longPress);
  actionButton.attachSingleClick(actionButton_singleClick);
  actionButton.attachLongPressStart(actionButton_longPress);
  actionButton.attachDoubleClick(actionButton_doubleClick);

  Keyboard.begin();
  Mouse.begin();
}

void loop() {
    selectButton.tick();
    actionButton.tick();
    
    if (heartbeat) {
      interval = millis() - lasttime;
      if (interval > heartbeatInterval) {
        lasttime=millis();
         Mouse.move(1,0,0);
      }
    }
}
