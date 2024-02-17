const byte LED_PIN = 13;
const byte METER_PIN = A4;
volatile double timerFrequency = 2.0; // Default frequency in Hz

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(METER_PIN, INPUT);
  Serial.begin(9600);
  startTimer(); // Initialize the timer with default frequency
}

void loop()
{
  // Read the potentiometer value
  int sensorValue = analogRead(METER_PIN);
  // Map the potentiometer value to the timer frequency range (0.5 Hz to 10 Hz)
  timerFrequency = map(sensorValue, 0, 1023, 0.5, 10);
}

void startTimer()
{
  // Calculate the timer period based on the frequency
  double timerPeriod = 1.0 / timerFrequency;
  uint16_t timerPrescaler = 1024; // Prescaler for Timer1
  uint16_t timerCompareValue = round((timerPeriod * 16000000.0) / timerPrescaler) - 1;

  noInterrupts(); // Disable interrupts

  TCCR1A = 0; // Set Timer1 control registers to zero
  TCCR1B = 0;

  // Set the Timer1 mode (CTC mode) and prescaler
  TCCR1B |= (1 << WGM12);                 // CTC mode, top value = OCR1A
  TCCR1B |= (1 << CS12) | (1 << CS10);    // Set prescaler to 1024

  // Set the Timer1 compare match value
  OCR1A = timerCompareValue;

  // Enable Timer1 compare match A interrupt
  TIMSK1 |= (1 << OCIE1A);

  interrupts(); // Enable interrupts
}

ISR(TIMER1_COMPA_vect)
{
  digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Toggle the LED
}
