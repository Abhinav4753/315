const int ledPin = 13; // LED pin
const int pirPin = 2; // PIR sensor pin
const int buttonPin = 3; // Button pin

int ledState = LOW; // LED state (LOW or HIGH)

void setup() {
  // Set LED pin as output
  pinMode(ledPin, OUTPUT);
  // Set PIR sensor pin as input
  pinMode(pirPin, INPUT);
  // Set button pin as input with pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);
  // Enable interrupt on rising edge of PIR sensor
  attachInterrupt(digitalPinToInterrupt(pirPin), PIR_motionDetected, RISING);
  // Start serial communication
  Serial.begin(9600);
}

void loop() {
  // Check button state
  if (!digitalRead(buttonPin)) { // Button pressed
    // Toggle LED state
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    Serial.println("Button pressed, LED toggled");
  }
}

void PIR_motionDetected() {
  // Turn on LED for 3 seconds
  digitalWrite(ledPin, HIGH);
  Serial.println("Motion detected!");
  delay(3000);
  digitalWrite(ledPin, LOW);
}
