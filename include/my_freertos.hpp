#ifndef MY_FREERTOS_HPP
#define MY_FREERTOS_HPP

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "my_peripheras.hpp"

#define DELAY_1S 1000 / portTICK_PERIOD_MS
#define DELAY_500MS 500 / portTICK_PERIOD_MS
#define DELAY_100MS 100 / portTICK_PERIOD_MS
#define DELAY_50MS 50 / portTICK_PERIOD_MS
#define DELAY_10MS 10 / portTICK_PERIOD_MS
#define DELAY_5MS 5 / portTICK_PERIOD_MS
#define DELAY_1MS 1 / portTICK_PERIOD_MS

#define DELAY_33MS 33 / portTICK_PERIOD_MS
#define DELAY_16MS 16 / portTICK_PERIOD_MS

void taskDisplay(void *pvParameters)
{
    int8_t f, icons[NUMFLAKES][3];
    
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = DELAY_5MS;
    xLastWakeTime = xTaskGetTickCount();

    // Initialize 'snowflake' positions
    for (f = 0; f < NUMFLAKES; f++)
    {
        icons[f][XPOS] = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS] = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
        Serial.print(F("x: "));
        Serial.print(icons[f][XPOS], DEC);
        Serial.print(F(" y: "));
        Serial.print(icons[f][YPOS], DEC);
        Serial.print(F(" dy: "));
        Serial.println(icons[f][DELTAY], DEC);
    }

    while (1)
    {
        // calculate refresh rate and display it
        auto currTick = xTaskGetTickCount();
        display.clearDisplay(); // Clear the display buffer

        auto refreshRate = 1000 / (currTick - xLastWakeTime);

        xLastWakeTime = currTick;

        // Display refresh rate
        display.setCursor(0, 0);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.print("Refresh Rate: ");
        display.print(refreshRate);
        display.print(" Hz");

        // Draw each snowflake:
        for (f = 0; f < NUMFLAKES; f++)
        {
            display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, SSD1306_WHITE);
        }

        display.display(); // Show the display buffer on the screen

        // Then update coordinates of each flake...
        for (f = 0; f < NUMFLAKES; f++)
        {
            icons[f][YPOS] += icons[f][DELTAY];
            // If snowflake is off the bottom of the screen...
            if (icons[f][YPOS] >= display.height())
            {
                // Reinitialize to a random position, just off the top
                icons[f][XPOS] = random(1 - LOGO_WIDTH, display.width());
                icons[f][YPOS] = -LOGO_HEIGHT;
                icons[f][DELTAY] = random(1, 6);
            }
        }
        vTaskDelay(xFrequency);
    }
}

void taskSampleIMU(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = DELAY_5MS;

    xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);


        Serial.println("IMU Sensor Data:");
        // Print out the values
        Serial.print("Acceleration X: ");
        Serial.print(a.acceleration.x);
        Serial.print(", Y: ");
        Serial.print(a.acceleration.y);
        Serial.print(", Z: ");
        Serial.print(a.acceleration.z);
        Serial.println(" m/s^2");

        Serial.print("Rotation X: ");
        Serial.print(g.gyro.x);
        Serial.print(", Y: ");
        Serial.print(g.gyro.y);
        Serial.print(", Z: ");
        Serial.print(g.gyro.z);
        Serial.println(" rad/s");

        Serial.print("Temperature: ");
        Serial.print(temp.temperature);
        Serial.println(" degC");

        getAngle(a.acceleration.x, a.acceleration.y, a.acceleration.z);
        Serial.print("Pitch: ");
        Serial.print(pitch);
        Serial.print(", Roll: ");
        Serial.println(roll);
        

        Serial.println("");
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void setupTasks()
{
    // xTaskCreate(
    //     taskDisplay, /* Task function. */
    //     "Display",   /* String with name of task. */
    //     10000,       /* Stack size in bytes. */
    //     NULL,        /* Parameter passed as input of the task */
    //     2,           /* Priority of the task. */
    //     NULL);       /* Task handle. */

    // xTaskCreate(
    //     taskSampleIMU, /* Task function. */
    //     "IMU",         /* String with name of task. */
    //     10000,         /* Stack size in bytes. */
    //     NULL,          /* Parameter passed as input of the task */
    //     1,             /* Priority of the task. */
    //     NULL);         /* Task handle. */
    xTaskCreatePinnedToCore(
        taskDisplay, /* Task function. */
        "Display",   /* String with name of task. */
        10000,       /* Stack size in bytes. */
        NULL,        /* Parameter passed as input of the task */
        1,           /* Priority of the task. */
        NULL,        /* Task handle. */
        0);          /* Core ID */
    
    xTaskCreatePinnedToCore(
        taskSampleIMU, /* Task function. */
        "IMU",         /* String with name of task. */
        10000,         /* Stack size in bytes. */
        NULL,          /* Parameter passed as input of the task */
        1,             /* Priority of the task. */
        NULL,          /* Task handle. */
        1);            /* Core ID */

}

#endif