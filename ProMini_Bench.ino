#include <SoftwareSerial.h>
SoftwareSerial BTserial(0, 1);                    // Pro Mini Connection RX, TX

const byte LED_Pin = 13;                          //** o_0 ** Debug
const int RelayPin[4] = {A4, A5, A6, A7};         // Relay connections

// max length of command is 20 chrs
const byte numChars = 20;
char receivedChars[numChars];
boolean newData = false;

unsigned long prev_Millis[4] = {0, 0, 0, 0};      // Will store last time the relay was turned on - off doesn't have a timer
unsigned long RelayDelay[4] = {0, 0, 0, 0};       // Interval at which to delay turning on relay - 0 for 1st run, set in parseData() below
unsigned long currentMillis;                      // Grab current time for delay calcs

void setup() {
  pinMode(LED_Pin, OUTPUT);                       // LED Pin mounted on board ⺌∅‿∅⺌
  for (int i = 0; i > 4; i++) {                   // Relay outputs
    pinMode(RelayPin[i], OUTPUT);
    digitalWrite(RelayPin[i], LOW);               // Outputs off on start ** o_0 ** Debug
  }
  BTserial.begin(9600);                           // Start Bt serial connection
}

void SetRelays(int RelayNum) {
  switch (receivedChars[1]) {
    case 'H':                                                                 // Relay state to high
      if (currentMillis - prev_Millis[RelayNum] >= RelayDelay[RelayNum]) {    // Check for delay
        prev_Millis[RelayNum] = currentMillis;                                // Set to current to issue a cooldown based on RelayDelay
        digitalWrite(LED_Pin, HIGH);
        digitalWrite(RelayPin[RelayNum], HIGH); // ** o_0 ** Debug
        BTserial.write(receivedChars[0] + receivedChars[1]);                  // Echo value to android via BT ( app inventor see's it as ascii codes )
      } else {
        BTserial.write("Wait for Cooldown");
      }
      break;
    case 'L':                                                                 // Relay state to low
      digitalWrite(LED_Pin, LOW);
      digitalWrite(RelayPin[RelayNum], LOW); // ** o_0 ** Debug
      BTserial.write(receivedChars[0] + receivedChars[1]);                    // Echo value to android via BT ( app inventor see's it as ascii codes )
      break;
  }
}

void parseData() {
  newData = false;

  switch (receivedChars[0]) {
    case 'A':
      SetRelays(0);
      RelayDelay[0] = 60000;                    // Set timer to 1 min - was at zero for 1st run - search for ( milliseconds to minutes )
      break;

    case 'B':
      SetRelays(1);
      RelayDelay[1] = 1000;                     // Set timer to 1 sec - was at zero for 1st run - search for ( milliseconds to minutes )
      break;

    case 'C':
      SetRelays(2);
      RelayDelay[2] = 1000;                     // Set timer to 1 sec - was at zero for 1st run - search for ( milliseconds to minutes )
      break;

    case 'D':
      SetRelays(3);
      RelayDelay[3] = 1000;                     // Set timer to 1 sec - was at zero for 1st run - search for ( milliseconds to minutes )
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

  currentMillis = millis();                       // Update for timers

  if (BTserial.available() > 0) {                 // If BT data avialable go read it
    recvWithStartEndMarkers();
  }
  if (newData) {                                  // When new data available parse it and toggle relays as needed
    parseData();
  }
}



