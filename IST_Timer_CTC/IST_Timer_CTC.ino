#define ledPin 13

void setup()
{
  pinMode(ledPin, OUTPUT);  // Ausgabe LED festlegen

  // Timer 1
  noInterrupts();           // Alle Interrupts temporär abschalten
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;                // Register mit 0 initialisieren
  OCR1A = 31250;            // Output Compare Register vorbelegen 
  TCCR1B |= (1 << CS12);    // 256 als Prescale-Wert spezifizieren
  TIMSK1 |= (1 << OCIE1A);  // Timer Compare Interrupt aktivieren
  interrupts();             // alle Interrupts scharf schalten
}
// Hier kommt die selbstdefinierte Interruptbehandlungsroutine 
// für den Timer Compare Interrupt
ISR(TIMER1_COMPA_vect)        
{
  TCNT1 = 0;                // Register mit 0 initialisieren   
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1); // LED ein und aus
}

void loop()
{
  // Wir könnten hier zusätzlichen Code integrieren
}
