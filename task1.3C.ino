const int ledPin = 13;      // LED pin
const int pirPin = 2;       // PIR sensor pin
const int buttonPin = 3;    // Button pin
const int moisturePin = A0; // Moisture sensor pin

volatile boolean controlMoisture = true; // Flag to control sensor focus

int ledState = LOW;        // LED state (LOW or HIGH)
int dryThreshold = 400;    // Adjust threshold for "dry" condition
int wetThreshold = 800;     // Adjust threshold for "wet" condition

void setup() {
  // Set pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(moisturePin, INPUT);

  // Enable interrupt on button press (falling edge)
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPressed, FALLING);

  // Start serial communication
  Serial.begin(9600);
}

void loop() {
  if (controlMoisture) {
    // Read moisture sensor value
    int moistureValue = analogRead(moisturePin);

    // Call function to control LED based on moisture
    moistureDetected(moistureValue);
  } else {
    // Check for motion
    if (digitalRead(pirPin) == HIGH) {
      // Motion detected - turn on LED
      digitalWrite(ledPin, HIGH);
      ledState = HIGH;
      Serial.println("Motion detected");
    } else {
      // No motion - turn off LED
      digitalWrite(ledPin, LOW);
      ledState = LOW;
    }
  }

  // Print sensor values and LED state to serial monitor
  Serial.print("Moisture value: ");
  Serial.println(analogRead(moisturePin));
  Serial.print("LED state: ");
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
    // Dry condition - turn on LED
    digitalWrite(ledPin, HIGH);
    ledState = HIGH;
    Serial.println("Soil is dry");
  } else if (moistureValue >= dryThreshold && moistureValue < wetThreshold) {
    // Moderate condition - blink LED (adjust logic as needed)
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
    Serial.println("Soil moisture is moderate");
  } else {
    // Wet condition - turn off LED
    digitalWrite(ledPin, LOW);
    ledState = LOW;
    Serial.println("Soil is wet");
  }
}
