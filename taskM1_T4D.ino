const int ledPin = 13;      // LED pin
const int greenLedPin = 11; // Green LED pin
const int pirPin = 2;       // PIR sensor pin
const int buttonPin = 3;    // Button pin
const int moisturePin = A0; // Moisture sensor pin
const int tempSensorPin = A1; // TMP36 Temperature sensor pin

volatile int sensorFocus = 0; // Variable to control sensor focus
volatile boolean buttonPressedFlag = false; // Flag to indicate button press

int ledState = LOW;         // LED state (LOW or HIGH)
int dryThreshold = 400;     // Adjust threshold for "dry" condition
int wetThreshold = 800;     // Adjust threshold for "wet" condition
unsigned long previousMillis = 0;
const long interval = 10000; // 10 seconds

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(moisturePin, INPUT);
  pinMode(tempSensorPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPressed, FALLING);

  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    timerInterrupt();
  }

  // Check if the button is pressed
  if (buttonPressedFlag) {
    buttonPressedFlag = false; // Reset the button press flag
    sensorFocus = (sensorFocus + 1) % 3; // Cycle between 0, 1, and 2 (moisture, PIR, and temperature)
    Serial.print("Switching focus to ");
    switch (sensorFocus) {
      case 0:
        Serial.println("moisture sensor");
        break;
      case 1:
        Serial.println("PIR sensor");
        break;
      case 2:
        Serial.println("temperature sensor");
        break;
    }
  }

  checkSensors();

  // Turn off the green LED outside the timer interrupt
  digitalWrite(greenLedPin, LOW);

  delay(500);
}

void checkSensors() {
  int moistureValue;
  int pirValue;
  int temperatureValue;
  float temperatureC;

  switch (sensorFocus) {
    case 0:
      // Focus on moisture sensor
      moistureValue = analogRead(moisturePin);
      moistureDetected(moistureValue);
      break;
    case 1:
      // Focus on PIR sensor
      pirValue = digitalRead(pirPin);
      if (pirValue == HIGH) {
        digitalWrite(ledPin, HIGH);
        ledState = HIGH;
        Serial.println("Motion detected");
        Serial.print("PIR state: ");
        Serial.println(ledState);
      } else {
        digitalWrite(ledPin, LOW);
        ledState = LOW;
      }
      break;
    case 2:
      // Focus on temperature sensor
      temperatureValue = analogRead(tempSensorPin);
      temperatureC = (temperatureValue * 0.004882814) * 100.0;
      Serial.print("Temperature: ");
      Serial.println(temperatureC);
      break;
  }
}


void buttonPressed() {
  buttonPressedFlag = true; // Set the button press flag
}

void moistureDetected(int moistureValue) {
  if (moistureValue < dryThreshold) {
    digitalWrite(ledPin, HIGH);
    ledState = HIGH;
    Serial.println("Soil is dry");
  } else if (moistureValue >= dryThreshold && moistureValue < wetThreshold) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
    Serial.println("Soil moisture is moderate");
  } else {
    digitalWrite(ledPin, LOW);
    ledState = LOW;
    Serial.println("Soil is wet");
  }
}

void timerInterrupt() {
  // Handle Timer Interrupt
  // Read data from other sensors or perform desired actions
  Serial.println("Timer interrupt occurred. Green Led Blinking");

  // Blink the green LED twice
  for (int i = 0; i < 2; i++) {
    digitalWrite(greenLedPin, HIGH);
    delay(500);
    digitalWrite(greenLedPin, LOW);
    delay(500);
  }
}
