const int motionSensorPin = 2;
const int ledPin = 7;

int motionDetected = LOW;

void setup() {
  pinMode(motionSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);  // Initialize serial communication
}

void loop() {
  int sensorValue = digitalRead(motionSensorPin);

  if (sensorValue == HIGH) {
    motionDetected = HIGH;
    digitalWrite(ledPin, HIGH);
    Serial.println("Motion detected!");
  } else {
    motionDetected = LOW;
    digitalWrite(ledPin, LOW);
    Serial.println("No motion detected.");
  }

  delay(1000);  // Delay between readings
}
