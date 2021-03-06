/* GardenProtector 
 * Protect your garden with water splash (fountain) that is activated with motion detector.
 * Parts:
 *  - Arduino Uno
 *  - HC-SR501 PIR Sensor
 *  - Relay NO 
 *  - Solenoid valve NC
 *  - 2 LEDs
 *  - 1 button
 *  - 1 switch
 */

const int alarmBtnPin = 2;
const int valveBtnPin = 3;  // switch that holds state
const int pirPin = 4;  // motion detector
const int alarmLedPin = 8;  // on if alarm can be on, blinks if alarm is on, off if alarm cannot be on 
const int valveLedPin = 9;  // on if valve is open
const int valvePin = 10;  // solenoid valve / electrovalve, normally close

byte alarmState = LOW;
byte alarmBtnState = LOW;
byte lastAlarmBtnState = LOW;
byte alarmLedState = LOW;

byte valveState = HIGH;
byte valveManualState = HIGH;
byte valveBtnState = LOW;
byte lastValveBtnState = LOW;

byte pirState = LOW;

unsigned long millisZero = 0;
unsigned long alarmStart = 0;

// half of period for blinking LED indicating active alarm
const long ledInterval = 1000;

// how long valve is opne when movement detected
const long alarmDuration = 10000;

// how often valve can be open
const long breakBetweenAlarmStarts = 90000;

void setup() {
  pinMode(alarmBtnPin, INPUT);
  pinMode(valveBtnPin, INPUT);
  pinMode(alarmLedPin, OUTPUT);
  pinMode(valveLedPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(valvePin, OUTPUT);
}

void loop() {
  // read button to acivate alarm
  byte alarmBtnState = digitalRead(alarmBtnPin);
  if (alarmBtnState != lastAlarmBtnState && (HIGH == valveManualState || HIGH == alarmState)) {
    lastAlarmBtnState = alarmBtnState;
    if (LOW == alarmBtnState) {
      alarmState = (HIGH == alarmState) ? LOW : HIGH;
      valveManualState = HIGH;
      alarmStart = millis();
    }
  }

  // read switch to open valve manually (switch holds state) 
  valveManualState = !digitalRead(valveBtnPin);
  if (LOW == valveManualState && HIGH == alarmState) {
    // if alarm is active and valve was opened - cancel alarm
    alarmState = LOW;
  }

  if (HIGH == alarmState) {
    // alarm active
    // blink LED
    unsigned long millisNow = millis();
    if (millisNow - millisZero >= ledInterval || millisZero > millisNow) {
      millisZero = millisNow;
      alarmLedState = (HIGH == alarmLedState) ? LOW : HIGH;
      digitalWrite(alarmLedPin, alarmLedState);
    } 
    
    pirState = digitalRead(pirPin);
    if (HIGH == pirState && (millisNow - alarmStart) > breakBetweenAlarmStarts) {
      // movement detected, open valve if last alarm was more than breakBetweenAlarmStarts ago
      // store alarm start time
      alarmStart = millisNow;
      valveState = LOW;  // valve is controlled with relay, normally open, that is closed on LOW
      valveManualState = LOW;  // force manual state for valve
      digitalWrite(valveLedPin, !valveState);  // LED need high to be on
      digitalWrite(valvePin, valveState);  // close switch = open valve 
    }
    
    if (millisNow - alarmStart >= alarmDuration) {
      // alarm should finish or vale manually closed
      valveState = HIGH;
      digitalWrite(valveLedPin, !valveState);
      digitalWrite(valvePin, valveState);
    } 
    
    if (alarmStart > millisNow) {
      // counter overflow 
      alarmStart = 0;
    }
  }
  else {
    // alarm deactivated
    digitalWrite(alarmLedPin, valveManualState);  // alarm can be activated only when valve is close
    digitalWrite(valveLedPin, !valveManualState);  // valve close on LOW switch
    digitalWrite(valvePin, valveManualState);  // valve close on LOW switch
  }
}
