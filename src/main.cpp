#include <Arduino.h>
#define LED_PIN 5
#define BUTTON_PIN 4
// put function declarations here:
// int myFunction(int, int);

void IRAM_ATTR buttonISR()
{
    static uint8_t button_state = HIGH;
    static uint64_t last_interrupt_time = 0;
    if (millis() - last_interrupt_time > 250)
    {
        button_state = !button_state;
        Serial.println("Button Pressed, LED State: " + String(button_state));
        digitalWrite(LED_PIN, button_state);
        last_interrupt_time = millis();
    }
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200); // Start serial communication at 115200 baud
    Serial.println("ESP32 Serial Output Initialized!");
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(BUTTON_PIN, buttonISR, FALLING);
}

void loop()
{
    // put your main code here, to run repeatedly:

    // Serial.println("Hello World");
    // delay(1000);
    // digitalWrite(LED_PIN, LOW);
    // delay(1000);
}

// put function definitions here:
int myFunction(int x, int y)
{
    return x + y;
}