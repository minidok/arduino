#define ledPin 13

void setup()
{
  pinMode(ledPin, OUTPUT);  // Ausgabe LED festlegen

  // Timer 1
  noInterrupts();           // Alle Interrupts temporär abschalten
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 34286;            // Timer nach obiger Rechnung vorbelegen
  TCCR1B |= (1 << CS12);    // 256 als Prescale-Wert spezifizieren
  TIMSK1 |= (1 << TOIE1);   // Timer Overflow Interrupt aktivieren
  interrupts();             // alle Interrupts scharf schalten
}
// Hier kommt die selbstdefinierte Interruptbehandlungsroutine 
// für den Timer Overflow
ISR(TIMER1_OVF_vect)        
{
  TCNT1 = 34286;             // Zähler erneut vorbelegen
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1); // LED ein und aus
}

void loop()
{
  // Wir könnten hier zusätzlichen Code integrieren
}
