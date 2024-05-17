/* GardenProtector 
 * Protect your garden with water splash (fountain) that is activated with motion detector.
 */

const int pirPin = 4;     // motion detector
const int valvePin = 10;  // solenoid valve / electrovalve, normally close

byte valveState = HIGH;
byte pirState = LOW;

unsigned long alarmStart = 0;
const long alarmDuration = 10000;            // ms
const long breakBetweenAlarmStarts = 120000;  // ms

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(valvePin, OUTPUT);
}

void loop() {
  unsigned long millisNow = millis();  
  pirState = digitalRead(pirPin);
  if (HIGH == pirState && (millisNow - alarmStart) > breakBetweenAlarmStarts) {
    alarmStart = millisNow;
    valveState = LOW;                    // valve is controlled with relay, normally open, that is closed on LOW
    digitalWrite(valvePin, valveState);  // close switch = open valve 
  }  
  if (millisNow - alarmStart >= alarmDuration) {
    valveState = HIGH;
    digitalWrite(valvePin, valveState);
  }   
  if (alarmStart > millisNow) {
    // counter overflow 
    alarmStart = 0;
  }
}
