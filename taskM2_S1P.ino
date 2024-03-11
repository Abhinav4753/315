// C++ code
//

const byte LED_PIN = 13;
const byte METER_PIN = A4;


void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(METER_PIN, INPUT);
  
  ADMUX = 0; // Use ADC0 (A0) as analog input
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
      
  Serial.begin(9600);
  
}

void loop()
{ 
  int Value = analogRead(METER_PIN);
  
  // Mapping the potentiometer value to a frequency range (0.1 Hz to 10 Hz)
  double Frequency = map(Value, 0, 1023, 1, 100) / 10.0;   
  
  startTimer(Frequency);
  
  delay(2000);
}



void startTimer(double timerFrequency){
  noInterrupts();
  TCCR1A = 0; // Reseting the Timer1 Control Registers
  TCCR1B = 0;
  
  // Calculate the timer register value for the given frequency
  unsigned long prescaler = 1024;
  unsigned long timer = (F_CPU / prescaler) / timerFrequency;
  
  // Setting up the Timer1 Mode to CTC 
  TCCR1B |= (1 << WGM12);
  
  // Set Timer1 Prescaler to 1024 (Clock Divided by 1024)
  TCCR1B |= (1 << CS12) | (1 << CS10);
  
  // Setting up the Compare Match Register for a 2-second interval (0.5 Hz frequency)
  OCR1A = timer - 1;
  
  // Enabling the Timer1 Compare Match A Interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  
  interrupts();
}


ISR(TIMER1_COMPA_vect){
   digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
}
