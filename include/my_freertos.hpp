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
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(5);
    xLastWakeTime = xTaskGetTickCount();

    auto halfWidth = SCREEN_WIDTH / 2;
    auto halfHeight = SCREEN_HEIGHT / 2;
    int16_t posX = halfWidth;
    int16_t posY = halfHeight;
    int16_t rad = 5;
    double scalingFactor = 0.3;

    while (1)
    {
        // calculate refresh rate and display it
        auto currTick = xTaskGetTickCount();
        display.clearDisplay(); // Clear the display buffer

        auto diff = currTick - xLastWakeTime;
        auto refreshRate = 1000;
        if (diff)
        {
            refreshRate = 1000 / diff;
        }

        xLastWakeTime = currTick;

        // Display refresh rate
        display.setCursor(0, 0);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.print(refreshRate);
        display.print(" fps");

        auto acc_x = accX * scalingFactor;
        auto acc_y = accY * scalingFactor;

        auto vel_x = -acc_y * diff;
        auto vel_y = -acc_x * diff;

        // calculate the new position of the circle
        posX += vel_x;
        posY += vel_y;

        // check if the circle is out of the screen
        if (posX <= rad)
        {
            posX = rad;
        }
        if (posX >= SCREEN_WIDTH - rad - 1)
        {
            posX = SCREEN_WIDTH - rad - 1;
        }
        if (posY <= rad)
        {
            posY = rad;
        }
        if (posY >= SCREEN_HEIGHT - rad - 1)
        {
            posY = SCREEN_HEIGHT - rad - 1;
        }

        display.drawCircle(posX, posY, rad, SSD1306_WHITE);
        display.display();


        vTaskDelay(xFrequency);
    }
}

void taskSampleIMU(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(10);

    sensors_event_t a, g, temp;
    xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        mpu.getEvent(&a, &g, &temp);

        // Serial.println("IMU Sensor Data:");
        // Print out the values
        Serial.print(">Acceleration X (m/s^2): ");
        Serial.println(a.acceleration.x);
        Serial.print(">Acceleration Y (m/s^2): ");
        Serial.println(a.acceleration.y);
        Serial.print(">Acceleration Z (m/s^2): ");
        Serial.println(a.acceleration.z);

        // Serial.print("Rotation X: ");
        // Serial.print(g.gyro.x);
        // Serial.print(", Y: ");
        // Serial.print(g.gyro.y);
        // Serial.print(", Z: ");
        // Serial.print(g.gyro.z);
        // Serial.println(" rad/s");

        // Serial.print("Temperature: ");
        // Serial.print(temp.temperature);
        // Serial.println(" degC");

        accX = a.acceleration.x;
        accY = a.acceleration.y;
        accZ = a.acceleration.z;
        // getAngle(a);
        // Serial.print(">Pitch:");
        // Serial.println(pitch);
        // Serial.print(">Roll:");
        // Serial.println(roll);
        

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