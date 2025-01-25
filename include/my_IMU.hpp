#ifndef MY_IMU_HPP
#define MY_IMU_HPP

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

double pitch, roll;

void getAngle(int Ax, int Ay, int Az)
{
    double x = Ax;
    double y = Ay;
    double z = Az;
    pitch = atan(x / sqrt((y * y) + (z * z)));
    roll = atan(y / sqrt((x * x) + (z * z)));
    pitch = pitch * (180.0 / 3.14);
    roll = roll * (180.0 / 3.14);
}

void setupGyro()
{
    if (!mpu.begin())
    {
        Serial.println("Failed to find MPU6050 chip");
        for (;;)
            ; // Don't proceed, loop forever
    }
    Serial.println("MPU6050 Found!");

    // set accelerometer range to +-8G
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

    // set gyro range to +- 500 deg/s
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);

    // set filter bandwidth to 21 Hz
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    delay(100);
}

#endif