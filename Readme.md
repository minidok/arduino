# Den Takt angeben

Für die genannten Aufgaben integrieren die AVR-Microcontroller diverse Timer mit zugeordneten Zählregistern von 8 oder 16 Bits Breite. Diese Register starten mit einem initialen Wert von 0. Ihr Inkrementieren, also das eigentliche Hochzählen, erfolgt automatisch und periodisch. Laufen die jeweiligen Register über, wird ein Timer-Überlauf-Interrupt ausgelöst.

Ein häufiger Irrtum lautet übrigens, dass die CPU des Arduino die Timer antreibt. Die Timer eines Arduino bzw. eines AVR-Mikrocontrollers von ATmel sind von der CPU bzw. MCU unabhängig. Diese Tatsache sollten Sie sich bei den nachfolgenden Diskussionen vor Augen halten.

Um eine möglichst hohe Auflösung zu erhalten, könnte der erste Ansatz darin liegen, die Updates der Counter (d.h. deren Inkrementieren) synchron zum Prozessortakt vorzunehmen. Das hat allerdings einen entscheidenden Schönheitsfehler. Bei einer angenommenen Taktfrequenz von 16 MHz wäre der Überlauf eines 8-Bit-Timers nach 16 Mikrosekunden erreicht, der eines 16-Bit-Registers nach rund 4,1 Millisekunden.

Das ist natürlich vorteilhaft für kurzzeitige Zeitintervalle. Was aber, wenn wir längere Zeiträume überdecken wollen? Um dies zu ermöglichen, bieten die Mikrocontroller sogenannte Prescaler. Diese konfigurieren, nach wie vielen Taktzyklen das System ein Zählregister inkrementieren soll. Mögliche Werte liegen bei 8, 64, 256 oder 1024. Eine Prescale-Einstellung von 1024 führt beispielsweise zum Inkrementieren des Zählers jeweils nach 64 Mikrosekunden bzw. 1024 Taktzyklen bei 16 MHz Taktfrequenz, sodass ein 16-Bit-Zähler erst nach 4,2 Sekunden überläuft.

Um präzise Intervalle zu programmieren, lassen sich Timer-Register mit Zählerständen vorbelegen statt sie bei 0 starten zu lassen. Einmal angekommen, wir würden gerne alle 0,5 Sekunden eine LED abwechselnd ein und ausschalten. Die gewünschte Frequenz des Timers wäre somit 2 Hz. Ein Takt besteht im Auslösen eines Timer-Überlaufs. Wie genau lässt sich dies erreichen?

Wir haben es mit folgenden Parametern zu tun:

- *bits* definiert die Größe des Zählerregister in Bits, etwa 16 für einen 16-Bit-Timer.
- *maxcount* entspricht dem maximalen Zahlenwerts 2bits.
- *prescale* ist der oben erläuterte konfigurierbare Prescalewert, also die Zahl der Taktzyklen bis ein weiteres Inkrementieren des Timeregisters erfolgt.
- *cpufreq* repräsentiert die CPU-Frequenz. Der Taktzyklus berechnet sich folglich aus 1 / cpufreq.
- *initcount* ist der vorbelegte Startwert des Zählregisters.
- count ist die notwendige Zahl von Inkrementierungen, um einem Timeroverflow auszulösen. Es gilt: count = maxcount - initcount.
- *deltaT* bezeichnet das gewünschte Zeitintervall bis zum Auslösen des Timer-Overflows. Man könnte auch definieren: deltaT = 1 / timerfreq (gewünschte Zahl von Timer Overflows pro Sekunde).
Es gilt prescale / cpufreq * count = deltaT

=> count = deltaT * cpufreq / prescale

=> maxcount - initcount = deltaT * cpufreq / prescale

=> initcount = maxcount - deltaT * cpufreq / prescale

Beispielsrechnung: Alle 0,5 Sekunden soll ein Timer-Overflow-Interrupt stattfinden.

Wir verwenden einen 16-Bit-Timer: bits = 16 => maxcount = 216 = 65536.
Wir benötigen einen Timer Overflow pro halbe Sekunde. deltaT = 0,5 sec = 1 / timerfreq
Die Taktfrequenz des Arduino-Board beträgt cpufreq = 16 MHz = 16.000.000 Hz
Als Prescale-Wert liegt prescale = 256 vor.
Der Timer startet statt mit 0 mit folgendem Anfangszählerstand initcount = 65.536 - 8.000.000/256 = 34.286

Das Timer-Register muss initial mit 34.286 starten, damit bis zum Timer Overflow – bei Überschreiten von 65.636 – genau eine halbe Sekunde vergeht. In jedem Durchlauf der Interrrupt-Service-Routine ist der Zähler jeweils wieder mit 34.286 initialisieren.

Ein entsprechender Sketch ist in ISR_TimerOverflow enthalten.

## World of Timers

Ein Arduino weist nicht nur einen einzelnen Timer sondern mehrere Timer auf. Kein Wunder, sind Timer doch essenzielle Grundkomponenten für verschiedene Aufgaben eines Mikrocontrollers.

- Timer 0 ( 8 Bit) Verwendet für Funktionen wie delay(), millis(), micros()
- Timer 1 ( 16 Bit) Verwendet von der Servo-Bibliothek
- Timer 2 ( 8 Bit) Verwendet von der Tone-Bibliothek
- Timer 3 (16 Bit) Nur Mega
- Timer 4 (16 Bit) Nur Mega
- Timer 5 (16 Bit) Nur Mega

Beim Arduino:

- PWM Pins 5 und 6 kontrolliert durch Timer 0
- PWM Pins 9 und 10 kontrolliert durch Timer 1
- PWM Pins 3 und 11 kontrolliert durch Timer 2
- 
Beim Arduino Mega:

- PWM Pins 4 und 13 kontrolliert durch Timer 0
- PWM Pins 11 und 12 kontrolliert durch Timer 1
- PWM Pins 9 und 10 kontrolliert durch Timer 2
- PWM Pins 2, 3 und 5 kontrolliert durch Timer 3
- PWM Pins 6, 7 und 8 kontrolliert durch Timer 4
- PWM Pins 44, 45 und 45 kontrolliert durch Timer 5

Es gibt zusätzlich diverse Einschränkungen zu beachten:

- Pin 11 des Arduino ist zugleich Pin mit PWM-Fähigkeit und Master-Out-Slave-In-Pin des SPI-Busses. Demzufolge lassen sich beide Funktionen nicht gleichzeitig nutzen.
- Für die Tonerzeugung ist mindestens Timer 2 im Einsatz. Daher lassen sich die Pins 3, 11 (Arduino) bzw. 9, 10 (Arduino Mega) nicht für PWM nutzen, solange die Funktion tone() im Einsatz ist.
- Beim Anschluss von Servos müssen sich Timer exklusiv dieser Aufgabe widmen, weshalb sich die Zahl der Digitalpins mit PMW-Unterstützung reduziert.

## Timer-Register

Wie im Sketch weiter oben ersichtlich, erfolgt die Steuerung der Timer-Funktionalität über verschiedene Register. Das Symbol µ repräsentiert die Nummer des jeweiligen Timers, also 0, 1, 2, ..., . . TCNTµ ist daher das Zählregister von Timer µ. Das Zählregister für Timer 1 lautet dementsprechend TCNT1, das für Timer 0 TCNT0.

Der Einfachheit halber beziehen sich die nachfolgenden Diskussionen wie auch die beiden Beispiel-Sketches auf Timer 1. Des Weiteren erspare ich Ihnen eine Aufzählung sämtlicher Details, sondern fokussiere mich auf die relevanten Eigenschaften.

TCCR1A (Timer Counter/Control Register): die Flags PWM10 und PWM11 erlauben eine Festlegung der Auflösung für den Fall, dass Timer 1 zur PWM-Steuerung dient. Ausgangsbasis sei die Vereinbarung TCCR1A = 0;:

- Kein PWM: no-op
- 8-Bit PWM: TCCR1A |= (1 << PWM10);
- 9-Bit PWM: TCCR1A |= (1 << PWM11);
- 10-Bit PWM: TCCR1A |= (1 << PWM 10); TCCR1A |= (1 << PWM11);

TCCR1B (Timer Counter/Control Register): Konfiguration des Prescaler.

- Kein Prescaler: TCCR1B = 0; TCCR1B |= (1 << CS10);
- Prescale = 8: TCCR1B = 0; TCCR1B |= (1 << CS11);
- Prescale = 64: TCCR1B = 0; TCCR1B |= (1 << CS10); TCCR1B |= (1 << CS11);
- Prescale = 256: TCCR1B = 0; TCCR1B |= (1 << CS12);
- Prescale = 1024: TCCR1B = 0; TCCR1B |= (1 << CS10); TCCR1B |= (1 << CS12);

Weitere Kombinationen ermöglichen die externe Steuerung über den T1-Pin.

TCNT1 (Timer/Counter Register): d.h. der eigentliche Zähler.

OCR1 (Output Compare Register): Ist der Zähler in TCNT1 gleich dem Inhalt des OCR1, erfolgt ein Timer Compare Interrupt.

ICR1 (Input Capture Register, nur für 16-Bit-Register): Messung der Zeit zwischen zwei Flanken des Input Capture Pins, die durch externe Schaltungen zustande kommen. Lässt sich auch zur Messung der Umdrehungszahl eines Motors einsetzen. Wird auch über Einstellungen von TCCR1A mit beeinflusst.

TIMSK1 (Timer/Counter Interrupt Mask Register): hier lassen sich Timer Interrupts unterbinden oder erlauben.

- Scharf schalten des Output Compare Interrupts: TIMSK1 |= (1 << OCIE1A)
- Scharf schalten des Timer Overflow Interrupts (16 Bit): TIMSK1 |= (1 << TOIE1)
- Scharf schalten des Timer Overflow Interrupts (16 Bit): TIMSK1 |= (1 << TOIE0)

TIFR1 (Timer/Counter Interrupt Flag Register): Hier lassen sich noch unverarbeitete Interrupts feststellen. Die Bits korrespondieren mit denen von TIMSK1.

## Alternative Methode CTC

Statt einen Interrupt bei Überlauf eines Timer-Registers auszulösen wie im oberen Sketch, gibt es es die alternative Option namens CTC (Clear Timer on Compare Match). Bei dieser vergleicht der Mikrocontroller, ob der Inhalt des Zählerregisters identisch mit dem Inhalt des zum Timer gehörigen OCR (Output Compare Registers) ist. Falls ja, wird ein Timer Compare Interrupt ausgelöst und das Register auf 0 zurückgesetzt. Wiederum soll jede halbe Sekunde ein Interrupt stattfinden.

Bei einem Prescaling von 256 und einer Taktfrequenz von 16 MHz können wir die obige Formel für count anwenden: count = deltaT * cpufreq / prescale = 0.5 * 16.000.000 / 256 = 31.256.

Siehe ISR_TIMER_CTC.