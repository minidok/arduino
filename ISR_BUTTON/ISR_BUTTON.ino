#define BUTTON_PIN 2

const unsigned long debounceInterval = 20; // in milliseconds
volatile unsigned long lastButtonPressed=0;
volatile byte countedClicks=0;

void setup()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Serial.begin(9600);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isrButtonPressed, FALLING);
}

void loop()
{
}

void isrButtonPressed()
{
    if((millis() - lastButtonPressed) > debounceInterval) {
        lastButtonPressed = millis();
        countedClicks += 1;
        Serial.println(countedClicks);
    }
}
