#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h> // Include the ESPmDNS library

#define BUZZ_PIN 5
#define BUTTON_PIN 15
#define TOUCH_PIN 4
#define ADC_PIN 35

#define TIMER_ENABLED 1
#define SETUP_WEB_SERVER 0

#define TOUCH_THRESHOLD 40

const char* ssid = "KaTo_IoT";
const char* password = "iamrobot";

WebServer server(80);


// Handle the root path
void handleRoot() {
    String response = "<html><body>";
    response += "<h1>Hello, Local Network!</h1>";
    // response += "<p>LED State: " + String(LED_State == HIGH ? "ON" : "OFF") + "</p>";
    response += "</body></html>";
    server.send(200, "text/html", response);
}

// void IRAM_ATTR buttonISR()
// {
//     static uint64_t last_interrupt_time = 0;
//     if (millis() - last_interrupt_time > 250)
//     {
//         // mute buzzer
//         digitalWrite(BUZZ_PIN, LOW);
//         last_interrupt_time = millis();
//     }
// }

// void IRAM_ATTR touchISR()
// {
//     Serial.println("Touch detected!, Touch sensor value: " + String(touchRead(TOUCH_PIN)));
//     static uint64_t last_interrupt_time = 0;
//     if (millis() - last_interrupt_time > 250)
//     {
//         // set buzzer on
//         digitalWrite(BUZZ_PIN, HIGH);
//         last_interrupt_time = millis();
//     }
// }

#if TIMER_ENABLED == 1
    hw_timer_t *timer = NULL;
    volatile int ADC_Value = 0;
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

    void IRAM_ATTR onTimer() {
        portENTER_CRITICAL_ISR(&timerMux);
        ADC_Value = analogRead(ADC_PIN);
        Serial.println("ADC Value: " + String(ADC_Value));
        portEXIT_CRITICAL_ISR(&timerMux);
    }

    void setupTimer() {
        timer = timerBegin(0, 80, true); // Timer 0, prescaler 80, count up
        timerAttachInterrupt(timer, &onTimer, true); // Attach the ISR
        timerAlarmWrite(timer, 500000, true); // Set alarm to trigger every 500ms
        timerAlarmEnable(timer); // Enable the timer alarm
    }
#endif


#if SETUP_WEB_SERVER == 1
    void setupWebServer() {
        WiFi.begin(ssid, password); // Connect to Wi-Fi
        Serial.println("Connecting to Wi-Fi");

        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }

        Serial.println("\nConnected to Wi-Fi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP()); // Print ESP32's IP address

        if (!MDNS.begin("esp32")) { // Initialize mDNS with the hostname "esp32"
            Serial.println("Error setting up MDNS responder!");
        } else {
            Serial.println("mDNS responder started");
        }

        server.on("/", handleRoot);

        server.begin();
        Serial.println("HTTP server started");
    }
#endif

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200); // Start serial communication at 115200 baud
    Serial.println("ESP32 Serial Output Initialized!");

    // pinMode(BUZZ_PIN, OUTPUT);
    // pinMode(BUTTON_PIN, INPUT_PULLUP);
    // attachInterrupt(BUTTON_PIN, buttonISR, FALLING);
    // touchAttachInterrupt(TOUCH_PIN, touchISR, TOUCH_THRESHOLD);

    #if SETUP_WEB_SERVER == 1
        setupWebServer();
    #endif
    

    #if TIMER_ENABLED == 1
        setupTimer();
    #endif
}

void loop()
{
    // put your main code here, to run repeatedly:

    #if SETUP_WEB_SERVER == 1
        server.handleClient();
    #endif
}
