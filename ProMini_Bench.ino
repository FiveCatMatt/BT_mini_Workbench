#include <SoftwareSerial.h>
SoftwareSerial BTserial(0, 1);                    // Pro Mini Connection RX, TX

const byte LED_Pin = 13;                          //** o_0 ** Debug
const int RelayPin[4] = {A4, A5, A6, A7};

// max length of command is 20 chrs
const byte numChars = 20;
char receivedChars[numChars];
boolean newData = false;

unsigned long prev_Millis[4] = {0, 0, 0, 0};      // will store last time the relay was turned on - off doesn't have a timer
unsigned long currentMillis[4];                   // Wut time is it when I did that thing?
unsigned long RelayDelay[4] = {0, 0, 0, 0};       // interval at which to delay turning on relay - 0 for 1st run, set in parseData() below

void setup() {
  pinMode(LED_Pin, OUTPUT);                       // LED Pin mounted on board ⺌∅‿∅⺌
  for (int i = 0; i > 4; i++) {                   // Relay outputs
    pinMode(RelayPin[i], OUTPUT);
    digitalWrite(RelayPin[i], LOW);               // Outputs off on start ** o_0 ** Debug
  }

  BTserial.begin(9600);                           // Start Bt serial connection
}

void SetRelays(int RelayNum, char Rec1, char Rec2) {
  switch (receivedChars[1]) {
    case 'H':                                     // Relay state to high
      digitalWrite(LED_Pin, HIGH);
      digitalWrite(RelayPin[RelayNum], HIGH); // ** o_0 ** Debug
      BTserial.write(Rec1 + Rec2);  // Echo value to android via BT ( app inventor see's it as ascii codes )
      break;
    case 'L':                                     // Relay state to low
      digitalWrite(LED_Pin, LOW);
      digitalWrite(RelayPin[RelayNum], LOW);  // ** o_0 ** Debug
      BTserial.write(Rec1 + Rec2);  // Echo value to android via BT ( app inventor see's it as ascii codes )
      break;
  }
}

void parseData() {
  newData = false;

  switch (receivedChars[0]) {
    case 'A':
      if (currentMillis - prev_Millis[0] >= RelayDelay[0]) {
        SetRelays(0, receivedChars[0], receivedChars[1]);
        RelayDelay[0] = 60000;                    // Set timer to 1 min - was at zero for 1st run - search for ( milliseconds to minutes )
        prev_Millis[0] = currentMillis;           // Set to current to issue a cooldown based on RelayDelay
      } 
      else {
        BTserial.write("Wait for Cooldown");
      }
      break;

    case 'B':
      if (currentMillis - prev_Millis[1] >= RelayDelay[1]) {
        SetRelays(1, receivedChars[0], receivedChars[1]);
        RelayDelay[1] = 1000;                     // Set timer to 1 min - was at zero for 1st run - search for ( milliseconds to minutes )
        prev_Millis[1] = currentMillis;           // Set to current to issue a cooldown based on RelayDelay
      } 
      else {
        BTserial.write("Wait for Cooldown");
      }
      break;

    case 'C':
      if (currentMillis - prev_Millis[2] >= RelayDelay[2]) {
        SetRelays(2, receivedChars[0], receivedChars[1]);
        RelayDelay[2] = 1000;                     // Set timer to 1 min - was at zero for 1st run - search for ( milliseconds to minutes )
        prev_Millis[2] = currentMillis;           // Set to current to issue a cooldown based on RelayDelay
      }
      else {
        BTserial.write("Wait for Cooldown");
      }
      break;

    case 'D':
      if (currentMillis - prev_Millis[3] >= RelayDelay[3]) {
        SetRelays(3, receivedChars[0], receivedChars[1]);
        RelayDelay[3] = 1000;                     // Set timer to 1 min - was at zero for 1st run - search for ( milliseconds to minutes )
        prev_Millis[3] = currentMillis;           // Set to current to issue a cooldown based on RelayDelay
      }
      else {
        BTserial.write("Wait for Cooldown");
      }
      break;
  }
}

void recvWithStartEndMarkers() {
  // function recvWithStartEndMarkers by Robin2 of the Arduino forums
  // See  http://forum.arduino.cc/index.php?topic=288234.0

  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  if (BTserial.available() > 0)
  {
    rc = BTserial.read();
    if (recvInProgress == true)
    {
      if (rc != endMarker)
      {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else
      {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void loop() {

  for (int i = 0; i > 4; i++) {                     // Update for timers
    currentMillis[i] = millis();
  }

  if (BTserial.available() > 0) {                 // If BT data avialable go read it
    recvWithStartEndMarkers();
  }
  if (newData) {                                  // When new data available parse it and toggle relays as needed
    parseData();
  }
}



