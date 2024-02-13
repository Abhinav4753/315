const int ledPin = 13;    // Red LED pin
const int greenLedPin = 12;  // Green LED pin
const int pirPin = 2;     // PIR sensor pin
const int buttonPin = 3;   // Button pin
const int moisturePin = A0;  // Moisture sensor pin
const int tempPin = A1;    // Temperature sensor pin

volatile boolean controlMoisture = true; // Flag to control sensor focus
volatile boolean ledBlinkFlag = false;  // Flag for LED blinking
volatile boolean timerInterruptFlag = false; // Flag for timer interrupt

int ledState = LOW;     // Red LED state (LOW or HIGH)
int greenLedState = LOW;   // Green LED state (LOW or HIGH)
int dryThreshold = 400;   // Adjust threshold for "dry" condition
int wetThreshold = 800;   // Adjust threshold for "wet" condition

void moistureDetected(int moistureValue);

void setup() {
  // Set pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(moisturePin, INPUT);
  pinMode(tempPin, INPUT);

  // Enable interrupt on button press (falling edge)
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPressed, FALLING);

  // Start serial communication
  Serial.begin(9600);

  // Set up Timer1 for LED blinking interrupt every 40,000ms (40 seconds)
  noInterrupts();
  TCCR1A = 0; // Set control register A to 0
  TCCR1B = 0; // Set control register B to 0
  TCNT1 = 0; // Initialize counter value to 0
  OCR1A = 62500; // Set compare match register for 40Hz frequency
  TCCR1B |= (1 << WGM12); // Configure timer in CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set prescaler to 1024
  TIMSK1 |= (1 << OCIE1A); // Enable Timer1 Compare A interrupt
  interrupts(); // Enable interrupts
}

void loop() {
  // Check for timer interrupt
  if (timerInterruptFlag) {
    // Print message when timer interrupt occurs
    Serial.println("Timer interrupt occurred!");
    
    // Blink the Green LED twice
    digitalWrite(greenLedPin, HIGH);
    delay(200);
    digitalWrite(greenLedPin, LOW);
    delay(200);
    digitalWrite(greenLedPin, HIGH);
    delay(200);
    digitalWrite(greenLedPin, LOW);
    
    timerInterruptFlag = false; // Reset flag
  }

  // Read moisture sensor value with delay
  int moistureValue = analogRead(moisturePin);
  delay(500); // Adjust delay as needed

  // Read temperature sensor value if needed
  int tempValue = analogRead(tempPin);

  if (controlMoisture) {
    // Call function to control LED based on moisture
    moistureDetected(moistureValue);
  } else {
    // Check for motion
    if (digitalRead(pirPin) == HIGH) {
      // Motion detected - turn on Red LED
      digitalWrite(ledPin, HIGH);
      ledState = HIGH;
      Serial.println("Motion detected");
    } else {
      // No motion - turn off Red LED
      digitalWrite(ledPin, LOW);
      ledState = LOW;
    }
  }

  // Print sensor values and LED state to serial monitor
  Serial.print("Moisture value: ");
  Serial.println(moistureValue);
  Serial.print("Temperature value: ");
  Serial.println(tempValue);
  Serial.print("Red LED state: ");
  Serial.println(ledState);

  delay(500); // Adjust delay as needed
}

void buttonPressed() {
  controlMoisture = !controlMoisture;
  Serial.print("Button pressed. Now focusing on ");
  Serial.println(controlMoisture ? "moisture sensor" : "PIR sensor");
}

void moistureDetected(int moistureValue) {
  if (moistureValue < dryThreshold) {
    // Dry condition - turn on Red LED
    digitalWrite(ledPin, HIGH);
    ledState = HIGH;
    Serial.println("Soil is dry");
  } else if (moistureValue >= dryThreshold && moistureValue < wetThreshold) {
    // Moderate condition - blink Green LED (adjust logic as needed)
    digitalWrite(greenLedPin, HIGH);
    delay(200);
    digitalWrite(greenLedPin, LOW);
    delay(200);
    Serial.println("Soil moisture is moderate");
  } else {
    // Wet condition - turn off Red LED
    digitalWrite(ledPin, LOW);
    ledState = LOW;
    Serial.println("Soil is wet");
  }
}

ISR(TIMER1_COMPA_vect) {
  // Timer1 Compare A interrupt service routine
  ledBlinkFlag = true;
  timerInterruptFlag = true; // Set the flag to indicate a timer interrupt
}
